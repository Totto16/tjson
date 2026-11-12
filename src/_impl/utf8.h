#pragma once

#ifndef _TJSON_IMPL_INTERNAL__
	#error "can only be used internally"
#endif

#include "../allocator.h"

#include "../_impl/utils.h"

#include <tstr.h>
#include <tvec.h>

#include "../tjson.h"

typedef int32_t Utf8Codepoint;

/* NOLINTBEGIN(misc-use-internal-linkage,totto-function-passing-type) */
// GCOVR_EXCL_START (external library)
TVEC_DEFINE_VEC_TYPE(Utf8Codepoint)
// GCOVR_EXCL_STOP
/* NOLINTEND(misc-use-internal-linkage,totto-function-passing-type) */

typedef TVEC_TYPENAME(Utf8Codepoint) Utf8CodepointArr;

typedef struct {
	Utf8Codepoint* data;
	uint64_t size;
} Utf8Data;

#include "../tjson/variants.h"

GENERATE_VARIANT_ALL_UTF8_DATA_RESULT() // GCOVR_EXCL_LINE

#ifdef __cplusplus
extern "C" {
#endif

NODISCARD Utf8DataResult get_utf8_string(tstr_view str_view);

void free_utf8_data(Utf8Data data);

NODISCARD bool utf8_codpoints_eq(const Utf8CodepointArr* arr1, const Utf8CodepointArr* arr2);

NODISCARD tstr get_normalized_string_from_codepoints(Utf8CodepointArr codepoints);

tstr get_normalized_string_from_codepoints_json_escaped(Utf8CodepointArr codepoints,
                                                        JsonSerializeEscapeOption escape);

#define UNICODE_MAX_VALUE_OVERALL 0x10FFFF

#ifdef __cplusplus
}
#endif
