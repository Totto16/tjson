#pragma once

#ifndef _TJSON_IMPL_INTERNAL__
	#error "can only be used internally"
#endif

#include "../allocator.h"

#include <regex.h>
#include <tstr.h>

#include "./utils.h"

typedef struct {
	regex_t regex;
} SimpleRegex;

#include "../tjson/variants.h"

GENERATE_VARIANT_ALL_SIMPLE_REGEX_RESULT() // GCOVR_EXCL_LINE

#ifdef __cplusplus
extern "C" {
#endif

NODISCARD SimpleRegexResult simple_regex_compile(const tstr* str);

NODISCARD bool simple_regex_match(const SimpleRegex* regex, const tstr* str);

void free_simple_regex(SimpleRegex* regex);

#ifdef __cplusplus
}
#endif
