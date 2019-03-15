#include "glo/stache.h"

#include <stdexcept>

#include <assert.h>

#include <fstream>


using std::string_literals::operator""s;



// #include <iostream>


template<unsigned char delimiter>
static inline std::string join(const std::vector<std::string>& vector)
{
    switch (vector.size()) {
        case 0:
            return {""};
        
        case 1:
            return vector.back();
        
        default: {
            assert(vector.size() >= 2);
            
            size_t size = vector.size() - 1;
            for (const std::string& string: vector)
                size += string.size();
            
            std::string out;
            out.reserve(size);
            
            auto it = vector.begin();
            out = *it;
            
            while (++it != vector.end()) {
                out += *it;
            }
            
            return out;
        }
    }
}



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



glo::stache::Partial load_partial_by_name(std::filesystem::path folder, std::string name)
{
    std::filesystem::path path = folder / (name + ".mustache"s);
    std::ifstream ifstream{path.string()};
    return {
        name,
        std::string{std::istreambuf_iterator<char>{ifstream},
                    std::istreambuf_iterator<char>{}}};
}



void shave(std::string& output, const Mustache& mustache, const Object& object, const Partials& partials, Section_list& sections)
{
    enum State {
        literal,
        waiting_for_second_open_brace,
        waiting_for_tag_type,
        parsing_comment,
        waiting_for_tag_name,
        parsing_tag_name,
        waiting_for_first_closing_brace,
        waiting_for_second_closing_brace
    } state{literal};

    enum Tag_type {
        comment,
        variable,
        implicit_variable,
        section,
        implicit_section,
        inverted_section,
        close_section,
        partial
    } tag_type{comment};


    size_t sections_excluded_from = 0;

    size_t inherited_section_count = sections.size();

    const char* tag_name_begin = nullptr;
    const char* tag_name_part_begin = nullptr;
    
    std::string_view tag_name;
    std::vector<std::string_view> tag_name_parts = {};
    
    size_t whitespace_count_on_line = 0;
    size_t other_char_count_on_line = 0;
    size_t section_tag_count_on_line = 0;
    size_t non_section_tag_count_on_line = 0;
    
    auto reset_tag_name = [&]{
        tag_name = {};
        tag_name_parts = {};
        tag_name_begin = nullptr;
        tag_name_part_begin = nullptr;
    };
    
    auto new_line = [&]{
        if (!sections_excluded_from && other_char_count_on_line == 0 && section_tag_count_on_line == 1 && non_section_tag_count_on_line == 0) {
            output.resize(output.size() - whitespace_count_on_line);
        }
        else {
            output += '\n';
        }
        whitespace_count_on_line = 0;
        other_char_count_on_line = 0;
        section_tag_count_on_line = 0;
        non_section_tag_count_on_line = 0;
    };
    
    auto add_char = [&](char c){
        switch (c) {
            case '\r': [[fallthrough]];
            case ' ':
                output += c;
                ++whitespace_count_on_line;
                break;
            
            case '\n':
                new_line();
                break;
            
            default:
                output += c;
                ++other_char_count_on_line;
                break;
        }
    };
    
    auto find_node_recursively_in_object = [&](const Object& o) -> const Node*{
        assert(tag_name_parts.size() >= 1);
        
        auto it = tag_name_parts.begin();
        const Node* node = o.find(*it);
        
        while (node && ++it != tag_name_parts.end()) {
            if (!node->value_.is_object())
                return nullptr;
            
            node = node->value_.get_object().find(*it);
        }
        
        return node;
    };
    
    
    
    auto find_node = [&]{
        
        switch (tag_name_parts.size()) {
            
            case 1:
                for (auto section_it = sections.rbegin(); section_it != sections.rend(); ++section_it) {
                    if (const Value* v = section_it->value_; v && v->is_object()) {
                        if (const Node* node = v->get_object().find(tag_name)) {
                            return node;
                        }
                    }
                }

                return object.find(tag_name);
                break;
                
            default:
        
                for (auto section_it = sections.rbegin(); section_it != sections.rend(); ++section_it) {
                    if (const Value* v = section_it->value_; v && v->is_object()) {
                        if (const Node* node = find_node_recursively_in_object(v->get_object())) {
                            return node;
                        }
                    }
                }

                return find_node_recursively_in_object(object);
                break;
        }
    };

    bool unescaped = false;

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

                        add_char(c);
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

                        add_char('{');
                        add_char(c);

                        state = literal;
                    break;
                }
            break;

            

            case waiting_for_tag_type:
                switch (c) {
                    
                    case ' ':
                        // Ignore whitespace
                        break;
                    
                    case '{':
                        if (unescaped)
                            throw std::runtime_error{"Fourth opening brace"};
                        
                        unescaped = true;
                        break;
                    
                    case '!':
                        tag_type = comment;
                        state = parsing_comment;
                        break;
                    
                    case '#':
                        tag_type = section;
                        state = waiting_for_tag_name;
                        break;
                    
                    case '^':
                        tag_type = inverted_section;
                        state = waiting_for_tag_name;
                        break;
                    
                    case '/':
                        tag_type = close_section;
                        state = waiting_for_tag_name;
                        break;
                    
                    case '>':
                        tag_type = partial;
                        state = waiting_for_tag_name;
                        break;
                    
                    case '.':
                        tag_type = implicit_variable;
                        reset_tag_name();
                        tag_name = {it.base(), 1};
                        state = waiting_for_first_closing_brace;
                        break;
                    
                    default:
                        tag_type = variable;
                        reset_tag_name();
                        tag_name_part_begin = tag_name_begin = it.base();
                        state = parsing_tag_name;
                        break;
                }
            break;

            

            case parsing_comment:
                switch (c) {
                    case '}': state = waiting_for_second_closing_brace; break;
                    default: break;
                }
            break;
            
            
            
            case waiting_for_tag_name:
                switch (c) {
                        
                    case '}':
                        throw std::runtime_error{"Unfinished/Invalid tag"};
                        
                    case '.':
                        switch (tag_type) {
                            case section:
                                tag_type = implicit_section;
                                reset_tag_name();
                                tag_name = {it.base(), 1};
                                state = waiting_for_first_closing_brace;
                                break;
                                
                            case close_section:
                                reset_tag_name();
                                tag_name = {it.base(), 1};
                                state = waiting_for_first_closing_brace;
                                break;
                            
                            default: 
                                throw std::runtime_error{"Unfinished/Invalid tag"};
                        }
                        break;
                        
                    case '\r': [[fallthrough]];
                    case '\n':
                        throw std::runtime_error{"Unfinished/Invalid tag"};
                        
                    case ' ':
                        break;
                    
                    default:
                        reset_tag_name();
                        tag_name_part_begin = tag_name_begin = it.base();
                        state = parsing_tag_name;
                        break;
                }
            break;


            
            case parsing_tag_name:
                switch (c) {
                    case '}':
                        tag_name = {tag_name_begin, static_cast<size_t>(it.base() - tag_name_begin)};
                        tag_name_parts.push_back({tag_name_part_begin, static_cast<size_t>(it.base() - tag_name_part_begin)});
                        state = waiting_for_second_closing_brace;
                        break;
                    
                    case ' ':
                        tag_name = {tag_name_begin, static_cast<size_t>(it.base() - tag_name_begin)};
                        tag_name_parts.push_back({tag_name_part_begin, static_cast<size_t>(it.base() - tag_name_part_begin)});
                        state = waiting_for_first_closing_brace;
                        break;
                    
                    case '.':
                        
                        if (tag_name_part_begin == it.base())
                            throw std::runtime_error{"Double dot in tag"s};
                            
                        tag_name_parts.push_back({tag_name_part_begin, static_cast<size_t>(it.base() - tag_name_part_begin)});
                        tag_name_part_begin = it.base() + 1;
                        break;
                        
                    case '\r': [[fallthrough]];
                    case '\n':
                        throw std::runtime_error{"Unfinished/Invalid tag"};
                    
                    default:
                        break;
                }
            break;

            

            case waiting_for_first_closing_brace:
                switch (c) {
                    case '}':
                        state = waiting_for_second_closing_brace;
                        break;
                        
                    case ' ':
                        break;
                        
                    default:
                        throw std::runtime_error{"Unfinished/Invalid tag"s};
                }
            break;


            
            case waiting_for_second_closing_brace:
                switch (c) {

                    case '}':

                        if (unescaped && it + 1 != mustache.string_.end()) {
                            ++it;
                            c = *it;
                            if (c != '}')
                                throw std::out_of_range{"Missing third closing brace for tag"s};
                        }

                    
                        
                        switch (tag_type) {

                            case comment:
                                ++non_section_tag_count_on_line;
                                
                                state = literal;
                                break;

                            case implicit_variable: {

                                if (sections_excluded_from) {
                                    state = literal;
                                    break;
                                }
                                
                                ++non_section_tag_count_on_line;

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
                                
                                ++non_section_tag_count_on_line;

                                const Node* node = find_node();

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
                                break;
                            }


                            
                            case implicit_section: {

                                if (sections_excluded_from) {
                                    sections.push_back({tag_name, nullptr, it + 1});
                                    state = literal;
                                    break;
                                }
                                
                                ++section_tag_count_on_line;
                                
                                if (sections.empty() || !sections.back().value_) {
                                    sections.push_back({tag_name, nullptr, it + 1});
                                    sections_excluded_from = sections.size();
                                }
                                else {
                                    const Value* parent_value = sections.back().value_;
                                    
                                    sections.push_back({tag_name, parent_value, it + 1});
                                    
                                    if (sections.back().value_->is_array()) {
                                    
                                        assert(sections.back().value_->get_array().size() >= 1);
                                        sections.back().array_ = &sections.back().value_->get_array();
                                        sections.back().array_it_ = sections.back().array_->begin();
                                        sections.back().value_ = sections.back().array_it_.base();
                                    }
                                }
                                
                                state = literal;
                                break;
                            }


                            
                            case section: {

                                if (sections_excluded_from) {
                                    sections.push_back({tag_name, nullptr, it + 1});
                                    state = literal;
                                    break;
                                }
                                
                                ++section_tag_count_on_line;

                                const Node* node = find_node();
                                sections.push_back({tag_name, node ? &node->value_ : nullptr, it + 1});

                                if (!node || !node->value_) {
                                    sections_excluded_from = sections.size();
                                }
                                else if (node->value_.is_array()) {
                                    assert(node->value_.get_array().size() >= 1);
                                    sections.back().array_ = &node->value_.get_array();
                                    sections.back().array_it_ = sections.back().array_->begin();
                                    sections.back().value_ = sections.back().array_it_.base();
                                }

                                state = literal;
                                break;
                            }


                            
                            case inverted_section: {

                                if (sections_excluded_from) {
                                    sections.push_back({tag_name, nullptr, it + 1});
                                    state = literal;
                                    break;
                                }
                                
                                ++section_tag_count_on_line;

                                const Node* node = find_node();
                                sections.push_back({tag_name, nullptr, it + 1});

                                if (node && node->value_)
                                    sections_excluded_from = sections.size();

                                state = literal;
                                break;
                            }


                            
                            case close_section:

                                if (sections.size() <= inherited_section_count)
                                    throw std::runtime_error{"Closing one too many section"s};

                                assert(!sections.empty());
                                
                                ++section_tag_count_on_line;

                                if (sections.back().name_ != tag_name)
                                    throw std::runtime_error{"Unclosed section"s};

                                if (!sections_excluded_from &&
                                    sections.back().array_ &&
                                    sections.back().array_it_ != sections.back().array_->end()) {

                                        ++sections.back().array_it_;

                                        if (sections.back().array_it_ != sections.back().array_->end()) {
                                            it = sections.back().begin_ - 1;
                                            sections.back().value_ = sections.back().array_it_.base();

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
                                
                                ++non_section_tag_count_on_line;

                                if (const Partial* p = partials.find(tag_name)) {
                                    shave(output, p->mustache_, object, partials, sections);
                                }

                                state = literal;
                                break;
                        }

                        unescaped = false;

                    break;

                    default:
                        throw std::runtime_error{"Unfinished tag"s};
                }
            break;
        }

        ++it;
    }


    switch (state) {


            case literal:
                if (!sections_excluded_from && other_char_count_on_line == 0 && section_tag_count_on_line == 1 && non_section_tag_count_on_line == 0 && whitespace_count_on_line >= 1) {
                    output.resize(output.size() - whitespace_count_on_line);
                }
            break;


            case waiting_for_second_open_brace:
                output += '{';
            break;


            case waiting_for_tag_type: [[fallthrough]];
            case parsing_comment: [[fallthrough]];
            case waiting_for_tag_name: [[fallthrough]];
            case parsing_tag_name: [[fallthrough]];
            case waiting_for_first_closing_brace: [[fallthrough]];
            case waiting_for_second_closing_brace:
                throw std::runtime_error{"Unfinished tag"s};
            break;
    }


    if (sections.size() > inherited_section_count)
        throw std::runtime_error{"Unfinished section"s};
}



}
}
