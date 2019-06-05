#include "glo/json_x_mustache.h"


namespace glo {



stache::Array json_array_to_mustache(const nlohmann::json& json) noexcept
{
    stache::Array a;
    
    if (!json.is_array())
        return a;
    
    for (const auto& item: json.items()) {
    
        const nlohmann::json& val = item.value();
        if (val.is_array())
            a.push_back(json_array_to_mustache(val));
        else if (val.is_object())
            a.push_back(json_object_to_mustache(val));
        else if (val.is_boolean())
            a.push_back(val.get<bool>());
        else if (val.is_string())
            a.push_back(val.get<std::string>());
        else if (val.is_number_integer())
            a.push_back(val.get<int64_t>());
        else if (val.is_number_unsigned())
            a.push_back(val.get<uint64_t>());
        else if (val.is_number_float())
            a.push_back(val.get<double>());
        else if (val.is_null())
            a.push_back(nullptr);
    }
    
    return a;
}



stache::Object json_object_to_mustache(const nlohmann::json& json) noexcept
{
    stache::Object o;
    
    if (!json.is_object())
        return o;
    
    for (const auto& item: json.items()) {
        const nlohmann::json& val = item.value();
        const std::string& key = item.key();
        if (val.is_array())
            o[key] = json_array_to_mustache(val);
        else if (val.is_object())
            o[key] = json_object_to_mustache(val);
        else if (val.is_boolean())
            o[key] = val.get<bool>();
        else if (val.is_string())
            o[key] = val.get<std::string>();
        else if (val.is_number_integer())
            o[key] = val.get<int64_t>();
        else if (val.is_number_unsigned())
            o[key] = val.get<uint64_t>();
        else if (val.is_number_float())
            o[key] = val.get<double>();
        else if (val.is_null())
            o[key] = nullptr;
    }
    
    return o;
}


    
}

