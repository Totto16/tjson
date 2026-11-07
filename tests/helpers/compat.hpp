
#pragma once

#include <tjson.h>

#include <nlohmann/json.hpp>

[[nodiscard]] bool operator==(const JsonValue& json_value, const nlohmann::json& compat);
