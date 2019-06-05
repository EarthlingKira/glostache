#pragma once

#include <nlohmann/json.hpp>
#include "glo/stache.h"

namespace glo {



stache::Array json_array_to_mustache(const nlohmann::json& json) noexcept;

stache::Object json_object_to_mustache(const nlohmann::json& json) noexcept;



}

