#pragma once

#ifndef _TJSON_IMPL_INTERNAL__
	#error "can only be used internally"
#endif

#include "../allocator.h"

#include "../_impl/utils.h"

#include <tstr.h>

typedef int32_t Utf8Codepoint;

typedef struct {
	Utf8Codepoint* data;
	uint64_t size;
} Utf8Data;

#include "../tjson.h"

GENERATE_VARIANT_ALL_UTF8_DATA_RESULT()

NODISCARD Utf8DataResult get_utf8_string(tstr_view str_view);

void free_utf8_data(Utf8Data data);
