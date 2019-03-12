#pragma once

#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <memory>
#include <optional>



namespace glo {
namespace stache {
    
    
    
struct Value;
struct Stash;

using Stash_ptr = std::unique_ptr<Stash>;



/**
 * A special string list, optimization for simple lists (usually mixing objects and strings doesn't make much sense).
 */
// using String_list = std::vector<std::string>;



/**
 * A variable value list, can contain different types.
 */
using Value_list = std::vector<Value>;



enum class Strong_boolean {
    True,
    False
};



/**
 * Value type for Mustache rendering, usually one variable used in the Mustache views.
 */
struct Value {
    using Variant = std::variant<std::string,
//                                  String_list,
                                 Stash_ptr,
                                 Strong_boolean,
                                 Value_list>;

    
    Value() noexcept: v_{Strong_boolean::False}
    {
    }

    Value(const Value& v) noexcept
    {
        if (v.is_string())
            v_ = v.get_string();
        else if (v.is_stash_ptr())
            v_ = std::make_unique<Stash>(*v.get_stash_ptr());
        else if (v.is_bool())
            v_ = v.get_strong_bool();
        else if (v.is_value_list())
            v_ = v.get_value_list();
    }

    Value& operator=(const Value& v) noexcept
    {
        if (v.is_string())
            v_ = v.get_string();
        else if (v.is_stash_ptr())
            v_ = std::make_unique<Stash>(*v.get_stash_ptr());
        else if (v.is_bool())
            v_ = v.get_strong_bool();
        else if (v.is_value_list())
            v_ = v.get_value_list();
        
        return *this;
    }
    
    Value(std::string string) noexcept: v_{std::move(string)}
    {
    }
    
    Value(std::string_view sv) noexcept: v_{std::string{sv}}
    {
    }
    
    Value(const char* s) noexcept: v_{std::string{s}}
    {
    }
    
    Value(Stash_ptr v) noexcept: v_{std::move(v)}
    {
    }
    
    Value(const Stash& v) noexcept: v_{std::make_unique<Stash>(v)}
    {
    }
    
    Value(Stash&& v) noexcept: v_{std::make_unique<Stash>(std::move(v))}
    {
    }
    
    Value(Strong_boolean v) noexcept: v_{v}
    {
    }
    
    Value(bool v) noexcept: v_{v ? Strong_boolean::True : Strong_boolean::False}
    {
    }
    
    Value(Value_list v) noexcept: v_{std::move(v)}
    {
    }
    
                                 
    Value& operator=(std::string string) noexcept
    {
        v_ = std::move(string);
        return *this;
    }
            
    Value& operator=(std::string_view sv) noexcept
    {
        v_ = std::string{sv};
        return *this;
    }
           
    Value& operator=(const char* s) noexcept
    {
        v_ = std::string{s};
        return *this;
    }

//     Value& operator=(String_list v) noexcept
//     {
//         v_ = std::move(v);
//         return *this;
//     }

    Value& operator=(Stash_ptr v) noexcept
    {
        v_ = std::move(v);
        return *this;
    }

    Value& operator=(const Stash& v) noexcept
    {
        v_ = std::make_unique<Stash>(v);
        return *this;
    }

    Value& operator=(Stash&& v) noexcept
    {
        v_ = std::make_unique<Stash>(std::move(v));
        return *this;
    }

    Value& operator=(Strong_boolean v) noexcept
    {
        v_ = v;
        return *this;
    }

    Value& operator=(bool v) noexcept
    {
        v_ = v ? Strong_boolean::True : Strong_boolean::False;
        return *this;
    }

    Value& operator=(Value_list v) noexcept
    {
        v_ = std::move(v);
        return *this;
    }
    
    
    
    bool is_string() const noexcept
    {
        return std::holds_alternative<std::string>(v_);
    }
    
    const std::string& get_string() const
    {
        return std::get<std::string>(v_);
    }
    
    std::string& get_string_ref()
    {
        return std::get<std::string>(v_);
    }
    
    
    
//     bool is_string_list() const noexcept
//     {
//         return std::holds_alternative<String_list>(v_);
//     }
//     
//     const String_list& get_string_list() const
//     {
//         return std::get<String_list>(v_);
//     }
//     
//     String_list& get_string_list_ref()
//     {
//         return std::get<String_list>(v_);
//     }
    
    
    
    bool is_stash_ptr() const noexcept
    {
        return std::holds_alternative<Stash_ptr>(v_);
    }
    
    const Stash_ptr& get_stash_ptr() const
    {
        return std::get<Stash_ptr>(v_);
    }
    
    Stash_ptr& get_stash_ptr_ref()
    {
        return std::get<Stash_ptr>(v_);
    }
    
    
    
    bool is_bool() const noexcept
    {
        return std::holds_alternative<Strong_boolean>(v_);
    }
    
    bool get_bool() const
    {
        return std::get<Strong_boolean>(v_) == Strong_boolean::True;
    }
    
    Strong_boolean get_strong_bool() const
    {
        return std::get<Strong_boolean>(v_);
    }
    
    Strong_boolean& get_strong_bool_ref()
    {
        return std::get<Strong_boolean>(v_);
    }
    
    
    
    bool is_value_list() const noexcept
    {
        return std::holds_alternative<Value_list>(v_);
    }
    
    const Value_list& get_value_list() const
    {
        return std::get<Value_list>(v_);
    }
    
    Value_list& get_value_list_ref()
    {
        return std::get<Value_list>(v_);
    }
    
    
    
    explicit operator bool() const noexcept
    {
        if (std::holds_alternative<std::string>(v_))
            return !std::get<std::string>(v_).empty();
        
//         if (std::holds_alternative<String_list>(v_))
//             return !std::get<String_list>(v_).empty();
        
        if (std::holds_alternative<Stash_ptr>(v_))
            return static_cast<bool>(std::get<Stash_ptr>(v_));
        
        if (std::holds_alternative<Strong_boolean>(v_))
            return std::get<Strong_boolean>(v_) == Strong_boolean::True;
        
        if (std::holds_alternative<Value_list>(v_))
            return !std::get<Value_list>(v_).empty();
        
        return false;
    }
    
    
    
    Variant v_;
};

    

/**
 * A Mustache node, mainly used in Stash, equals a map node.
 */
struct Node {
    using Key = std::string;
    
    Key key_;
    Value value_;
};



using Node_list = std::vector<Node>;
    


/**
 * A Stash of information for Mustache, equals a map or JSON object.
 */
struct Stash {
    
    
    Stash() noexcept
    {
    }
    
    
    Stash(Node_list nodes) noexcept: nodes_{std::move(nodes)}
    {
    }
    
    
    Stash(std::initializer_list<Node> nodes) noexcept: nodes_{nodes.begin(), nodes.end()}
    {
    }
    
    
//     Stash(std::initializer_list<T> l) noexcept: nodes_{std::move(nodes)}
//     {
//     }
    
    
    /**
     * Find Node by key.
     */
    Node* find(std::string_view key) noexcept
    {
        auto it = std::find_if(nodes_.begin(), nodes_.end(), [key](const Node& n){ return n.key_ == key; });
        if (it != nodes_.end())
            return it.base();
        
        return nullptr;
    }
    
    
    
    /**
     * Find Node by key.
     */
    const Node* find(std::string_view key) const noexcept
    {
        auto it = std::find_if(nodes_.begin(), nodes_.end(), [key](const Node& n){ return n.key_ == key; });
        if (it != nodes_.end())
            return it.base();
        
        return nullptr;
    }
    
    
    /**
     * Find node iterator by key.
     */
    Node_list::iterator find_it(std::string_view key) noexcept
    {
        return std::find_if(nodes_.begin(), nodes_.end(), [key](const Node& n){ return n.key_ == key; });
    }
    
    
    
    /**
     * Access value by name, creates node if not yet existing.
     */
    Value& operator[](std::string_view name) noexcept
    {
        if (auto it = find_it(name); it != nodes_.end())
            return it->value_;
        
        
        return nodes_.emplace_back(Node{std::string{name}, {}}).value_;
    }
    


    /**
     * A makeshift flat map of nodes.
     * Can/should be replaced when C++20 is stable.
     */
    Node_list nodes_;
};



/**
 * Mustache text, equals a mustache template.
 */
struct Mustache {
    std::string string_;
};



/**
 * Mustache literal for template literals.
 */
inline Mustache operator"" _mustache(const char* s, std::size_t size) noexcept
{
    return {std::string{s, size}};
}



/**
 * A partial, including one name and a Mustache template.
 */
struct Partial {
    using Name = std::string;
    
    Name name_;
    Mustache mustache_;
};



using Partial_list = std::vector<Partial>;



/**
 * A makeshift flat partial map.
 * 
 * Can/should be replaced when C++20 is stable.
 */
struct Partials {
    
    
    /**
     * Find Partial by name.
     */
    Partial* find(std::string_view name) noexcept
    {
        auto it = std::find_if(partials_.begin(), partials_.end(), [name](const Partial& n){ return n.name_ == name; });
        if (it != partials_.end())
            return it.base();
        
        return nullptr;
    }
    
    
    
    /**
     * Find Partial by name.
     */
    const Partial* find(std::string_view name) const noexcept
    {
        auto it = std::find_if(partials_.begin(), partials_.end(), [name](const Partial& n){ return n.name_ == name; });
        if (it != partials_.end())
            return it.base();
        
        return nullptr;
    }
    
    
    /**
     * Find partial iterator by name.
     */
    Partial_list::iterator find_it(std::string_view name) noexcept
    {
        return std::find_if(partials_.begin(), partials_.end(), [name](const Partial& p){ return p.name_ == name; });
    }
    
    
    
    /**
     * Access mustache for partial by name, creates if not yet existing.
     */
    Mustache& operator[](std::string_view name) noexcept
    {
        if (auto it = find_it(name); it != partials_.end())
            return it->mustache_;
        
        
        return partials_.emplace_back(Partial{std::string{name}, {}}).mustache_;
    }
    
    
    
    /**
     * Insert new partial or update mustache for existing partial.
     */
    Partials& operator<<(const Partial& partial) noexcept
    {
        if (auto it = find_it(partial.name_); it != partials_.end()) {
            it->mustache_ = partial.mustache_;
        }
        else {
            partials_.push_back(partial);
        }
        return *this;
    }
    
    
    
    Partial_list partials_;
};



using Closure_values = std::vector<const Value*>;



void shave(std::string& output, const Mustache& mustache, const Stash& stash, const Partials& partials, Closure_values closure_values = {});


std::string shave(const Mustache& mustache, const Stash& stash, const Partials& partials, Closure_values = {});





    
    
}
}
