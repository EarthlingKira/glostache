#include "glo/stache.h"

#include <stdexcept>

#include <assert.h>


using std::string_literals::operator""s;



// #include <iostream>




namespace glo {
namespace stache {    
    



static inline void add_string_html_encoded(std::string_view v, bool nl2br, std::string& o)
{
    o.reserve(o.size() + v.size());

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


    
struct Section {
    std::string name_;
    const Value* value_;
    std::string::const_iterator begin_;
    
    const Value_list* value_list_{nullptr};
    Value_list::const_iterator value_list_it_{};
};



using Section_list = std::vector<Section>;
    
    
    
void shave(std::string& output, const Mustache& mustache, const Stash& stash, const Partials& partials, Closure_values closure_values)
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
        section,
        inverted_section,
        close_section,
        partial
    } tag_type{comment};
    
    
    size_t sections_excluded_from = 0;
    
    
    Section_list sections;
    
    auto find_node = [&](std::string_view tag_name){
        for (auto section_it = sections.rbegin(); section_it != sections.rend(); ++section_it)
            if (const Value* v = section_it->value_; v && v->is_stash_ptr())
                if (const Node* node = v->get_stash_ptr()->find(tag_name); node)
                    return node;
                
        for (auto closure_val_it = closure_values.rbegin(); closure_val_it != closure_values.rend(); ++closure_val_it)
            if (const Value* v = *closure_val_it; v && v->is_stash_ptr())
                if (const Node* node = v->get_stash_ptr()->find(tag_name); node)
                    return node;
        
        return stash.find(tag_name);
    };
    
    auto generate_closure_values = [&]{
        Closure_values v;
        for (const Section& s: sections)
            v.push_back(s.value_);
        return v;
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
                    case '#': tag_type = section; tag_name = ""; state = parsing_tag_name; break;
                    case '^': tag_type = inverted_section; tag_name = ""; state = parsing_tag_name; break;
                    case '/': tag_type = close_section; tag_name = ""; state = parsing_tag_name; break;
                    case '>': tag_type = partial; tag_name = ""; state = parsing_tag_name; break;
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
                        if (unescaped) {
                            unescaped = false;
                            break;
                        }
                        
                        switch (tag_type) {
                            
                            case comment:
                                state = literal;
                            break;
                            
                            
                            case variable: {
                                if (sections_excluded_from) {
                                    state = literal;
                                    break;
                                }
                                
                                if (tag_name == ".") {
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
                                    else if (!closure_values.empty()) {
                                        if (closure_values.back() && closure_values.back()->is_string()) {
                                            if (unescaped) {
                                                output += closure_values.back()->get_string();
                                            }
                                            else {
                                                add_string_html_encoded(closure_values.back()->get_string(), false, output);
                                            }
                                        }
                                    }
                                    
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
                                const Node* node = find_node(tag_name);
                                sections.push_back({tag_name, node ? &node->value_ : nullptr, it + 1});
                                
                                if (sections_excluded_from) {
                                    state = literal;
                                    break;
                                }
                                
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
                                const Node* node = find_node(tag_name);
                                sections.push_back({tag_name, node ? &node->value_ : nullptr, it + 1});
                                
                                if (sections_excluded_from) {
                                    state = literal;
                                    break;
                                }
                                
                                if (node && node->value_)
                                    sections_excluded_from = sections.size();
                                
                                state = literal;
                            }
                            break;
                            
                            
                            case close_section:
                                
                                if (sections.empty() || sections.back().name_ != tag_name)
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
                                if (const Partial* p = partials.find(tag_name)) {
                                    shave(output, p->mustache_, stash, partials, generate_closure_values());
                                }
                                
                                state = literal;
                            break;
                        }
                        
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
    
    
    if (!sections.empty())
        throw std::runtime_error{"Unfinished section "s + sections.back().name_};
}



std::string shave(const Mustache& mustache, const Stash& stash, const Partials& partials, Closure_values closure_values)
{
    std::string output;
    shave(output, mustache, stash, partials, std::move(closure_values));
    return output;
}


    
}
}
