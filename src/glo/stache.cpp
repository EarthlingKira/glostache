#include "glo/stache.h"

#include <stdexcept>

#include <assert.h>


using std::string_literals::operator""s;



// #include <iostream>




namespace glo {
namespace stache {




static inline void add_string_html_encoded(std::string_view v, bool nl2br, std::string& o)
{
//     o.reserve(o.size() + v.size());

    for (auto c: v) {

        switch (c) {
            case '&':
                o += "&amp;";
                break;
            case '<':
                o += "&lt;";
                break;
            case '>':
                o += "&gt;";
                break;
            case '\n':
                if (nl2br)
                    o += "<br>";
                else
                    o += ' ';
                break;
            case '\t':
                o += " &nbsp; &nbsp;";
                break;
            default:
                if (!iscntrl(c))
                    o += c;
                break;
        }
    }
}



void shave(std::string& output, const Mustache& mustache, const Stash& stash, const Partials& partials, Section_list& sections)
{
    enum State {
        literal,
        waiting_for_second_open_brace,
        waiting_for_tag_type,
        parsing_comment,
        parsing_tag_name,
        waiting_for_first_closing_brace,
        waiting_for_second_closing_brace
    } state{literal};

    enum Tag_type {
        comment,
        variable,
        self_variable,
        section,
        inverted_section,
        close_section,
        partial
    } tag_type{comment};


    size_t sections_excluded_from = 0;

    size_t inherited_section_count = sections.size();

    auto find_node = [&](std::string_view tag_name){
        for (auto section_it = sections.rbegin(); section_it != sections.rend(); ++section_it)
            if (const Value* v = section_it->value_; v && v->is_stash_ptr())
                if (const Node* node = v->get_stash_ptr()->find(tag_name); node)
                    return node;

        return stash.find(tag_name);
    };

    bool unescaped = false;
    std::string tag_name;

    if (output.empty())
        output.reserve(mustache.string_.size());

    std::string::const_iterator it{mustache.string_.begin()};

    while (it != mustache.string_.end()) {

        char c = *it;

        switch (state) {
            case literal:
                switch (c) {

                    case '{':
                        state = waiting_for_second_open_brace;
                    break;

                    default:
                        if (sections_excluded_from)
                            break;

                        output += c;
                    break;
                }
            break;


            case waiting_for_second_open_brace:
                switch (c) {
                    case '{':
                        state = waiting_for_tag_type;
                    break;

                    default:
                        if (sections_excluded_from)
                            break;

                        output += '{';
                        output += c;

                        state = literal;
                    break;
                }
            break;


            case waiting_for_tag_type:
                switch (c) {
                    case ' ': break;
                    case '{': if (unescaped) throw std::runtime_error{"Fourth opening brace"}; unescaped = true; break;
                    case '!': tag_type = comment; state = parsing_comment; break;
                    case '#': tag_type = section; tag_name.clear(); state = parsing_tag_name; break;
                    case '^': tag_type = inverted_section; tag_name.clear(); state = parsing_tag_name; break;
                    case '/': tag_type = close_section; tag_name.clear(); state = parsing_tag_name; break;
                    case '>': tag_type = partial; tag_name.clear(); state = parsing_tag_name; break;
                    case '.': tag_type = self_variable; state = waiting_for_first_closing_brace; break;
                    default: tag_type = variable; tag_name = c; state = parsing_tag_name; break;
                }
            break;


            case parsing_comment:
                switch (c) {
                    case '}': state = waiting_for_second_closing_brace; break;
                    default: break;
                }
            break;


            case parsing_tag_name:
                switch (c) {
                    case '}': state = waiting_for_second_closing_brace; break;
                    case ' ': if (!tag_name.empty()) state = waiting_for_first_closing_brace; break;
                    default: tag_name += c; break;
                }
            break;


            case waiting_for_first_closing_brace:
                switch (c) {
                    case '}': state = waiting_for_second_closing_brace; break;
                    case ' ': break;
                    default: throw std::runtime_error{"Unfinished tag "s + tag_name};
                }
            break;


            case waiting_for_second_closing_brace:
                switch (c) {

                    case '}':

                        if (unescaped && it + 1 != mustache.string_.end()) {
                            ++it;
                            c = *it;
                            if (c != '}')
                                throw std::out_of_range{"Missing third closing brace for tag "s + tag_name};
                        }

                        switch (tag_type) {

                            case comment:
                                state = literal;
                            break;

                            case self_variable: {

                                if (sections_excluded_from) {
                                    state = literal;
                                    break;
                                }

                                if (!sections.empty()) {
                                    if (sections.back().value_ && sections.back().value_->is_string()) {
                                        if (unescaped) {
                                            output += sections.back().value_->get_string();
                                        }
                                        else {
                                            add_string_html_encoded(sections.back().value_->get_string(), false, output);
                                        }
                                    }
                                }

                                state = literal;
                                break;
                            }


                            case variable: {
                                if (sections_excluded_from) {
                                    state = literal;
                                    break;
                                }

                                const Node* node = find_node(tag_name);

                                if (node) {
                                    const Value& v = node->value_;
                                    if (v.is_string()) {
                                        if (unescaped) {
                                            output += v.get_string();
                                        }
                                        else {
                                            add_string_html_encoded(v.get_string(), false, output);
                                        }
                                    }
                                }

                                state = literal;
                            }
                            break;


                            case section: {

                                if (sections_excluded_from) {
                                    sections.push_back({std::move(tag_name), nullptr, it + 1});
                                    state = literal;
                                    break;
                                }

                                const Node* node = find_node(tag_name);
                                sections.push_back({std::move(tag_name), node ? &node->value_ : nullptr, it + 1});

                                if (!node || !node->value_) {
                                    sections_excluded_from = sections.size();
                                }
                                else if (node->value_.is_value_list()) {
                                    assert(node->value_.get_value_list().size() >= 1);
                                    sections.back().value_list_ = &node->value_.get_value_list();
                                    sections.back().value_list_it_ = sections.back().value_list_->begin();
                                    sections.back().value_ = sections.back().value_list_it_.base();
                                }

                                state = literal;
                            }
                            break;


                            case inverted_section: {

                                if (sections_excluded_from) {
                                    sections.push_back({std::move(tag_name), nullptr, it + 1});
                                    state = literal;
                                    break;
                                }

                                const Node* node = find_node(tag_name);
                                sections.push_back({std::move(tag_name), nullptr, it + 1});

                                if (node && node->value_)
                                    sections_excluded_from = sections.size();

                                state = literal;
                            }
                            break;


                            case close_section:

                                if (sections.size() <= inherited_section_count)
                                    throw std::runtime_error{"Closing one too many section "s + tag_name};

                                assert(!sections.empty());

                                if (sections.back().name_ != tag_name)
                                    throw std::runtime_error{"Unclosed section "s + tag_name};

                                if (!sections_excluded_from &&
                                    sections.back().value_list_ &&
                                    sections.back().value_list_it_ != sections.back().value_list_->end()) {

                                        ++sections.back().value_list_it_;

                                        if (sections.back().value_list_it_ != sections.back().value_list_->end()) {
                                            it = sections.back().begin_ - 1;
                                            sections.back().value_ = sections.back().value_list_it_.base();

                                            state = literal;
                                            break;
                                        }
                                }

                                sections.pop_back();

                                if (sections_excluded_from && sections.size() < sections_excluded_from)
                                    sections_excluded_from = 0;

                                state = literal;
                            break;

                            case partial:

                                if (sections_excluded_from) {
                                    state = literal;
                                    break;
                                }

                                if (const Partial* p = partials.find(tag_name)) {
                                    shave(output, p->mustache_, stash, partials, sections);
                                }

                                state = literal;
                            break;
                        }

                        unescaped = false;

                    break;

                    default: throw std::runtime_error{"Unfinished tag "s + tag_name};
                }
            break;
        }

        ++it;
    }


    switch (state) {


            case literal:
            break;


            case waiting_for_second_open_brace:
                output += '{';
            break;


            case waiting_for_tag_type:  [[fallthrough]];
            case parsing_comment: [[fallthrough]];
            case parsing_tag_name: [[fallthrough]];
            case waiting_for_first_closing_brace: [[fallthrough]];
            case waiting_for_second_closing_brace:
                throw std::runtime_error{"Unfinished tag "s + tag_name};
            break;
    }


    if (sections.size() > inherited_section_count)
        throw std::runtime_error{"Unfinished section "s + sections.back().name_};
}



}
}
