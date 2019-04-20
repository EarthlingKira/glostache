#pragma once

#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <memory>
#include <optional>
#include <filesystem>



namespace glo {
namespace stache {



struct Value;
struct Object;

using Object_ptr = std::unique_ptr<Object>;



/**
 * A special string list, optimization for simple lists (usually mixing objects and strings doesn't make much sense).
 */
// using String_list = std::vector<std::string>;



/**
 * A variable value list, can contain different types.
 */
using Array = std::vector<Value>;



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
                                 Object_ptr,
                                 Strong_boolean,
                                 Array>;


    Value() noexcept: v_{Strong_boolean::False}
    {
    }

    Value(const Value& v) noexcept
    {
        if (v.is_string())
            v_ = v.get_string();
        else if (v.is_object())
            v_ = std::make_unique<Object>(v.get_object());
        else if (v.is_bool())
            v_ = v.get_strong_bool();
        else if (v.is_array())
            v_ = v.get_array();
    }

    Value(Value&& v) noexcept
    {
        if (v.is_string())
            v_ = std::move(v.get_string());
        else if (v.is_object())
            v_ = std::move(v.get_object_ptr_ref());
        else if (v.is_bool())
            v_ = std::move(v.get_strong_bool());
        else if (v.is_array())
            v_ = std::move(v.get_array());
    }

    Value& operator=(const Value& v) noexcept
    {
        if (v.is_string())
            v_ = v.get_string();
        else if (v.is_object())
            v_ = std::make_unique<Object>(v.get_object());
        else if (v.is_bool())
            v_ = v.get_strong_bool();
        else if (v.is_array())
            v_ = v.get_array();

        return *this;
    }

    Value& operator=(Value&& v) noexcept
    {
        if (v.is_string())
            v_ = std::move(v.get_string());
        else if (v.is_object())
            v_ = std::move(v.get_object_ptr_ref());
        else if (v.is_bool())
            v_ = std::move(v.get_strong_bool());
        else if (v.is_array())
            v_ = std::move(v.get_array());

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

    Value(uint8_t v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(int8_t v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(uint16_t v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(int16_t v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(uint32_t v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(int32_t v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(uint64_t v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(int64_t v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(float v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(double v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(long double v) noexcept: v_{std::to_string(v)}
    {
    }

    Value(Object_ptr v) noexcept: v_{std::move(v)}
    {
    }

    Value(const Object& v) noexcept: v_{std::make_unique<Object>(v)}
    {
    }

    Value(Object&& v) noexcept: v_{std::make_unique<Object>(std::move(v))}
    {
    }

    Value(Strong_boolean v) noexcept: v_{v}
    {
    }

    Value(bool v) noexcept: v_{v ? Strong_boolean::True : Strong_boolean::False}
    {
    }

    Value(Array v) noexcept: v_{std::move(v)}
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

    Value& operator=(uint8_t v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(int8_t v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(uint16_t v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(int16_t v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(uint32_t v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(int32_t v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(uint64_t v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(int64_t v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(float v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(double v) noexcept
    {
        return *this = std::to_string(v);
    }

    Value& operator=(long double v) noexcept
    {
        return *this = std::to_string(v);
    }

//     Value& operator=(String_list v) noexcept
//     {
//         v_ = std::move(v);
//         return *this;
//     }

    Value& operator=(Object_ptr v)
    {
        if (!v)
            throw std::runtime_error{"Please do not assign an empty object_ptr"};
        
        v_ = std::move(v);
        return *this;
    }

    Value& operator=(const Object& v) noexcept
    {
        v_ = std::make_unique<Object>(v);
        return *this;
    }

    Value& operator=(Object&& v) noexcept
    {
        v_ = std::make_unique<Object>(std::move(v));
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

    Value& operator=(Array v) noexcept
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



    bool is_object() const noexcept
    {
        return std::holds_alternative<Object_ptr>(v_);
    }

    const Object& get_object() const
    {
        return *std::get<Object_ptr>(v_);
    }

    Object_ptr& get_object_ptr_ref()
    {
        return std::get<Object_ptr>(v_);
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



    bool is_array() const noexcept
    {
        return std::holds_alternative<Array>(v_);
    }

    const Array& get_array() const
    {
        return std::get<Array>(v_);
    }

    Array& get_array_ref()
    {
        return std::get<Array>(v_);
    }



    explicit operator bool() const noexcept
    {
        if (std::holds_alternative<std::string>(v_))
            return !std::get<std::string>(v_).empty();

//         if (std::holds_alternative<String_list>(v_))
//             return !std::get<String_list>(v_).empty();

        if (std::holds_alternative<Object_ptr>(v_))
            return static_cast<bool>(std::get<Object_ptr>(v_));

        if (std::holds_alternative<Strong_boolean>(v_))
            return std::get<Strong_boolean>(v_) == Strong_boolean::True;

        if (std::holds_alternative<Array>(v_))
            return !std::get<Array>(v_).empty();

        return false;
    }



    Variant v_;
};



/**
 * A Mustache node, mainly used in Object, equals a map node.
 */
struct Node {
    using Key = std::string;

    Key key_;
    Value value_;
};



using Node_list = std::vector<Node>;



/**
 * A Object of information for Mustache, equals a map or JSON object.
 */
struct Object {


    Object() noexcept
    {
    }


    Object(Node_list nodes) noexcept: nodes_{std::move(nodes)}
    {
    }


    Object(std::initializer_list<Node> nodes) noexcept: nodes_{nodes.begin(), nodes.end()}
    {
    }


//     Object(std::initializer_list<T> l) noexcept: nodes_{std::move(nodes)}
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
     * Find Value by key.
     */
    Value& at(std::string_view key)
    {
        auto it = std::find_if(nodes_.begin(), nodes_.end(), [key](const Node& n){ return n.key_ == key; });
        if (it != nodes_.end())
            return it->value_;

        throw std::out_of_range{"Key not found"};
    }


    /**
     * Find Value by key.
     */
    const Value& at(std::string_view key) const
    {
        auto it = std::find_if(nodes_.begin(), nodes_.end(), [key](const Node& n){ return n.key_ == key; });
        if (it != nodes_.end())
            return it->value_;

        throw std::out_of_range{"Key not found"};
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


    Partials() noexcept
    {
    }


    Partials(Partial_list partials) noexcept: partials_{std::move(partials)}
    {
    }


    Partials(std::initializer_list<Partial> partials) noexcept: partials_{partials.begin(), partials.end()}
    {
    }


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




class Cache {
public:

    struct Relative_partial {
        std::filesystem::path relative_path_;
        Partial partial_;
    };

    Cache(std::filesystem::path root_path);

    void load_partials();

    Partial find(std::filesystem::path relative_path);


private:
    std::filesystem::path root_path_;
    std::vector<Relative_partial> partials_;
};


/**
 * Load a partial by name in folder, expecting %name%.mustache file name.
 */
Partial load_partial_by_name(std::filesystem::path folder, std::string name);



struct Section {
    std::string_view name_;
    const Value* value_;
    std::string::const_iterator begin_;

    const Array* array_{nullptr};
    Array::const_iterator array_it_{};
};



using Section_list = std::vector<Section>;



void shave(std::string& output, const Mustache& mustache, const Object& object, const Partials& partials, Section_list& sections);


inline void shave(std::string& output, const Mustache& mustache, const Object& object, const Partials& partials)
{
    Section_list sections;
    shave(output, mustache, object, partials, sections);
}


inline void shave(std::string& output, const Mustache& mustache, const Object& object)
{
    Partials partials;
    Section_list sections;
    shave(output, mustache, object, partials, sections);
}


inline void shave(std::string& output, const Mustache& mustache)
{
    Object object;
    Partials partials;
    Section_list sections;
    shave(output, mustache, object, partials, sections);
}



inline std::string shave(const Mustache& mustache, const Object& object, const Partials& partials, Section_list& sections)
{
    std::string output;
    shave(output, mustache, object, partials, sections);
    return output;
}


inline std::string shave(const Mustache& mustache, const Object& object, const Partials& partials)
{
    Section_list sections;
    return shave(mustache, object, partials, sections);
}


inline std::string shave(const Mustache& mustache, const Object& object)
{
    Partials partials;
    Section_list sections;
    return shave(mustache, object, partials, sections);
}


inline std::string shave(const Mustache& mustache)
{
    Object object;
    Partials partials;
    Section_list sections;
    return shave(mustache, object, partials, sections);
}







}
}
