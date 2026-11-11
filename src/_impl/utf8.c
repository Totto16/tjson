#include <utf8proc.h>

#include "../allocator.h"

#include "./utf8.h"

NODISCARD Utf8DataResult get_utf8_string(const tstr_view str_view) {

	utf8proc_int32_t* buffer = TJSON_MALLOC(sizeof(utf8proc_int32_t) * str_view.len);

	if(buffer == NULL) {
		return new_utf8_data_result_error(TSTR_STATIC_LIT("failed malloc"));
	}

	const utf8proc_ssize_t result = utf8proc_decompose(
	    (const uint8_t*)str_view.data, (LibCLong)str_view.len, buffer, (LibCLong)str_view.len,
	    (utf8proc_option_t)0); // NOLINT(cppcoreguidelines-narrowing-conversions,clang-analyzer-optin.core.EnumCastOutOfRange)

	if(result < 0) {
		TJSON_FREE(buffer);
		return new_utf8_data_result_error(tstr_static_from_static_cstr(utf8proc_errmsg(result)));
	}

	if(result != (utf8proc_ssize_t)str_view.len) {
		// truncate the buffer
		Utf8Codepoint* const new_buffer = // NOLINT(totto-const-correctness-c)
		    TJSON_REALLOC(buffer, sizeof(utf8proc_int32_t) * (size_t)result);

		if(new_buffer == NULL) {
			TJSON_FREE(buffer);
			return new_utf8_data_result_error(TSTR_STATIC_LIT("failed realloc"));
		}
		buffer = new_buffer;
	}

	const Utf8Data utf8_data = {
		.data = buffer,
		.size = (size_t)result,
	};

	return new_utf8_data_result_ok(utf8_data);
}

void free_utf8_data(Utf8Data data) {
	TJSON_FREE(data.data);
}

NODISCARD bool utf8_codpoints_eq(const Utf8CodepointArr* arr1, const Utf8CodepointArr* arr2) {
	const size_t len1 = TVEC_LENGTH(Utf8Codepoint, *arr1);
	const size_t len2 = TVEC_LENGTH(Utf8Codepoint, *arr2);

	if(len1 != len2) {
		return false;
	}

	const Utf8Codepoint* const data1 = TVEC_DATA_CONST(Utf8Codepoint, arr1);
	const Utf8Codepoint* const data2 = TVEC_DATA_CONST(Utf8Codepoint, arr2);

	return memcmp(data1, data2, sizeof(*data1) * len1) == 0;
}

// from my project ass_parser_c, modified slightly
#define NORMALIZED_STR_NORMAL_UTF8_CHUNK_SIZE_NORMALIZE 256

// 4 for unicode chars
#define NORMALIZED_STR_NORMAL_UTF8_MAX_AMOUNT_PER_CHUNK_ITERATION 4

NODISCARD tstr get_normalized_string_from_codepoints(const Utf8CodepointArr codepoints) {
	if(codepoints.data == NULL) {
		return tstr_null();
	}

	size_t buffer_size = NORMALIZED_STR_NORMAL_UTF8_CHUNK_SIZE_NORMALIZE;
	uint8_t* buffer = (uint8_t*)TJSON_MALLOC(buffer_size);

	size_t current_size = 0;

	if(buffer == NULL) {
		return tstr_null();
	}

	for(size_t i = 0; i < TVEC_LENGTH(Utf8Codepoint, codepoints); ++i) {

		if(buffer_size - current_size < NORMALIZED_STR_NORMAL_UTF8_MAX_AMOUNT_PER_CHUNK_ITERATION) {
			buffer_size = buffer_size + NORMALIZED_STR_NORMAL_UTF8_CHUNK_SIZE_NORMALIZE;
			uint8_t* new_buffer = (uint8_t*)TJSON_REALLOC(buffer, buffer_size);

			if(new_buffer == NULL) {
				TJSON_FREE(buffer);
				return tstr_null();
			}

			buffer = new_buffer;
		}

		const Utf8Codepoint codepoint = TVEC_AT(Utf8Codepoint, codepoints, i);

		// needs place for 4 / NORMALIZED_STR_NORMAL_UTF8_MAX_AMOUNT_PER_CHUNK_ITERATION chars
		const utf8proc_ssize_t result = utf8proc_encode_char(codepoint, buffer + current_size);

		if(result <= 0) {
			TJSON_FREE(buffer);
			return tstr_null();
		}

		current_size = current_size + (uint8_t)result;
	}

	if(buffer_size - current_size < 1) {
		buffer_size = buffer_size + 1;
		uint8_t* new_buffer = (uint8_t*)TJSON_REALLOC(buffer, buffer_size);

		if(new_buffer == NULL) {
			TJSON_FREE(buffer);
			return tstr_null();
		}

		buffer = new_buffer;
	}

	buffer[current_size] = '\0';

	return tstr_own((char*)buffer, current_size, current_size);
}

NODISCARD static bool json_impl_should_escape(
    const Utf8Codepoint codepoint, // NOLINT(bugprone-easily-swappable-parameters)
    const JsonSerializeEscapeOption escape) {

	// note: not all escapable chars are required to be escaped

	//     (    %x22 /          ; "    quotation mark  U+0022
	//          %x5C /          ; \    reverse solidus U+005C
	//          %x2F /          ; /    solidus         U+002F
	//          %x62 /          ; b    backspace       U+0008
	//          %x66 /          ; f    form feed       U+000C
	//          %x6E /          ; n    line feed       U+000A
	//          %x72 /          ; r    carriage return U+000D
	//          %x74 /          ; t    tab             U+0009
	//          %x75 4HEXDIG   ; uXXXX                U+XXXX
	//     )

	switch(codepoint) {
		case '"':
		case '\\': {
			return true;
		}
		case '/': {
			// can be, but not required
			return escape >= JsonSerializeEscapeOptionMore;
		}
		case '\b':
		case '\f':
		case '\n':
		case '\r':
		case '\t': {
			return true;
		}
		default: {

			const utf8proc_category_t cat = utf8proc_category(codepoint);

			switch(escape) {
				case JsonSerializeEscapeOptionMore: {
					// escape utf8 control characters or surrogates
					switch(cat) {
						case UTF8PROC_CATEGORY_CS:
						case UTF8PROC_CATEGORY_CC: {
							return true;
						}
						default: {
							return false;
						}
					}
				}
				case JsonSerializeEscapeOptionMost: {
					// escape most special chars
					switch(cat) {
						case UTF8PROC_CATEGORY_LU:
						case UTF8PROC_CATEGORY_LL:
						case UTF8PROC_CATEGORY_LT:
						case UTF8PROC_CATEGORY_LM:
						case UTF8PROC_CATEGORY_LO:
						case UTF8PROC_CATEGORY_ND:
						case UTF8PROC_CATEGORY_NL:
						case UTF8PROC_CATEGORY_NO:
							//
						case UTF8PROC_CATEGORY_PC:
						case UTF8PROC_CATEGORY_PD:
						case UTF8PROC_CATEGORY_PS:
						case UTF8PROC_CATEGORY_PE:
						case UTF8PROC_CATEGORY_PI:
						case UTF8PROC_CATEGORY_PF:
						case UTF8PROC_CATEGORY_PO:
						case UTF8PROC_CATEGORY_ZS:
						case UTF8PROC_CATEGORY_ZL:
						case UTF8PROC_CATEGORY_ZP: {
							return false;
						}
						case UTF8PROC_CATEGORY_CC: {
							return true;
						}
						default: {
							// never escape ascii, but all the rest
							return (codepoint > 0x7F); // NOLINT(readability-magic-numbers)
						}
					}
				}
				case JsonSerializeEscapeOptionAll: {
					// escape all but a few
					switch(cat) {
						case UTF8PROC_CATEGORY_CC: {
							return true;
						}
						default: {
							// never escape ascii, but all the rest
							return (codepoint > 0x7F); // NOLINT(readability-magic-numbers)
						}
					}
				}
				case JsonSerializeEscapeOptionDefault:
				default: {
					// utf8 control characters always need an escaping
					return (cat == UTF8PROC_CATEGORY_CC);
				}
			}
		}
	}
}

NODISCARD static int8_t json_impl_escape_char_into(const Utf8Codepoint codepoint,
                                                   uint8_t* const dst) {
	switch(codepoint) {
		case '"':
		case '\\':
		case '/': {

			dst[0] = '\\';
			dst[1] = (uint8_t)codepoint;

			return 2;
		}
		case '\b': {
			dst[0] = '\\';
			dst[1] = 'b';

			return 2;
		}
		case '\f': {
			dst[0] = '\\';
			dst[1] = 'f';

			return 2;
		}
		case '\n': {
			dst[0] = '\\';
			dst[1] = 'n';

			return 2;
		}
		case '\r': {
			dst[0] = '\\';
			dst[1] = 'r';

			return 2;
		}
		case '\t': {

			dst[0] = '\\';
			dst[1] = 't';

			return 2;
		}
		default: {

			if(codepoint > 0xFFFF) { // NOLINT(readability-magic-numbers)

				// Surrogate pair escaping, see
				// https://datatracker.ietf.org/doc/html/rfc2781#section-2

				if(codepoint > UNICODE_MAX_VALUE_OVERALL) { // GCOVR_EXCL_BR_WITHOUT_HIT: 1/2
					// NOTE: utf8proc never returns such a code, as it is invalid utf8 and is caught
					// earlier!

					ASSERT(false && "IMPLEMENTATION ERROR"); // GCOVR_EXCL_LINE
				}

				// see: https://datatracker.ietf.org/doc/html/rfc2781#section-2.1
				const uint32_t surrogate_rest = (uint32_t)codepoint - (uint32_t)0x10000ULL;

				const uint16_t low_surrogate_pair =
				    (uint16_t)0xDC00UL + (uint16_t)(surrogate_rest & 0x3FFU);
				const uint16_t high_surrogate_pair =
				    (uint16_t)0xD800UL + ((uint16_t)(surrogate_rest >> 10) & 0x3FFU);

				dst[0] = '\\';
				dst[1] = 'u';

				char hex_buf[5]; // NOLINT(readability-magic-numbers)

				LibCInt result = snprintf(hex_buf, sizeof(hex_buf), "%04X", high_surrogate_pair);
				if(result != 4) { // GCOVR_EXCL_BR_WITHOUT_HIT: 1/2
					              // NOTE: unreachable, as snprintf with 04X always produces 4 chars
					return -1;    // GCOVR_EXCL_LINE
				}

				memcpy(dst + 2, hex_buf, 4);

				dst[6] = '\\'; // NOLINT(readability-magic-numbers)
				dst[7] = 'u';  // NOLINT(readability-magic-numbers)

				result = snprintf(hex_buf, sizeof(hex_buf), "%04X", low_surrogate_pair);
				if(result != 4) { // GCOVR_EXCL_BR_WITHOUT_HIT: 1/2
					// NOTE: unreachable, as snprintf with 04X always produces 4 chars
					return -1; // GCOVR_EXCL_LINE
				}

				memcpy(dst + 8, hex_buf, 4); // NOLINT(readability-magic-numbers)

				return 12; // NOLINT(readability-magic-numbers)
			}

			const uint16_t small_codepoint = (uint16_t)codepoint;

			dst[0] = '\\';
			dst[1] = 'u';

			char hex_buf[5]; // NOLINT(readability-magic-numbers)

			const LibCInt result = snprintf(hex_buf, sizeof(hex_buf), "%04X", small_codepoint);
			if(result != 4) { // GCOVR_EXCL_BR_WITHOUT_HIT: 1/2
				// NOTE: unreachable, as snprintf with 04X always produces 4 chars
				return -1; // GCOVR_EXCL_LINE
			}

			memcpy(dst + 2, hex_buf, 4);

			return 6; // NOLINT(readability-magic-numbers)
		}
	}
}

// from my project ass_parser_c, modified slightly
#define NORMALIZED_STR_JSON_ESCAPED_UTF8_CHUNK_SIZE_NORMALIZE 256

// 4 for unicode chars, 6 for escaped chars, as the max there is \uXXXX and 12 for surrogates ->
// \uXXXX\uYYYYY
#define NORMALIZED_STR_JSON_ESCAPED_UTF8_MAX_AMOUNT_PER_CHUNK_ITERATION 12

tstr get_normalized_string_from_codepoints_json_escaped(const Utf8CodepointArr codepoints,
                                                        const JsonSerializeEscapeOption escape) {
	if(codepoints.data == NULL) {
		return tstr_from("");
	}

	size_t buffer_size = NORMALIZED_STR_JSON_ESCAPED_UTF8_CHUNK_SIZE_NORMALIZE;
	uint8_t* buffer = (uint8_t*)TJSON_MALLOC(buffer_size);

	size_t current_size = 0;

	if(buffer == NULL) {
		return tstr_null();
	}

	for(size_t i = 0; i < TVEC_LENGTH(Utf8Codepoint, codepoints); ++i) {

		if(buffer_size - current_size <
		   NORMALIZED_STR_JSON_ESCAPED_UTF8_MAX_AMOUNT_PER_CHUNK_ITERATION) {
			buffer_size = buffer_size + NORMALIZED_STR_JSON_ESCAPED_UTF8_CHUNK_SIZE_NORMALIZE;
			uint8_t* new_buffer = (uint8_t*)TJSON_REALLOC(buffer, buffer_size);

			if(new_buffer == NULL) {
				TJSON_FREE(buffer);
				return tstr_null();
			}

			buffer = new_buffer;
		}

		const Utf8Codepoint codepoint = TVEC_AT(Utf8Codepoint, codepoints, i);

		if(json_impl_should_escape(codepoint, escape)) {
			// needs place for 12 / NORMALIZED_STR_JSON_ESCAPED_UTF8_MAX_AMOUNT_PER_CHUNK_ITERATION
			// chars
			const int8_t result = json_impl_escape_char_into(codepoint, buffer + current_size);

			if(result <= 0) {
				TJSON_FREE(buffer);
				return tstr_null();
			}

			current_size = current_size + (uint8_t)result;
		} else {

			// needs place for 4  chars
			const utf8proc_ssize_t result = utf8proc_encode_char(codepoint, buffer + current_size);

			if(result <= 0) {
				TJSON_FREE(buffer);
				return tstr_null();
			}

			current_size = current_size + (uint8_t)result;
		}
	}

	if(buffer_size - current_size < 1) {
		buffer_size = buffer_size + 1;
		uint8_t* new_buffer = (uint8_t*)TJSON_REALLOC(buffer, buffer_size);

		if(new_buffer == NULL) {
			TJSON_FREE(buffer);
			return tstr_null();
		}

		buffer = new_buffer;
	}

	buffer[current_size] = '\0';

	return tstr_own((char*)buffer, current_size, current_size);
}
