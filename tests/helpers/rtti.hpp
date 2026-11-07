

#pragma once

#include <tjson_helper.h>

#include "./cpp_types.hpp"
#include <ostream>
#include <trtti.h>

[[nodiscard]] bool operator==(const RTTIAnnotatedValue& rtti_value1,
                              const RTTIAnnotatedValue& rtti_value2);

std::ostream& operator<<(std::ostream& os, const RTTIAnnotatedValue& rtti_value);

[[nodiscard]] bool operator==(const IsNotError& error, const JsonIterateResult& result);

std::ostream& operator<<(std::ostream& os, const JsonIterateResult& result);
