
#pragma once

/** Generated header
 * with: c_variant_generator
 * version: 1.1.4
 * commit:d7c7c31ae4c2
 * date: 2026-05-16T18:50:31.393Z
 * repo: https://github.com/Totto16/c_variant_generator
 * lang: JS / TS
 * ABI: v1.0.1
 **/

// NOTE: manually stripped an changed in some parts

/* NOLINTBEGIN(readability-identifier-naming,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp,bugprone-macro-parentheses)
 */

#ifdef __cplusplus

	#if defined(__clang__)
		#define __INTERNALS_IGNORE_C_COMPOUND_LITERALS_ERROR_IN_CPP() \
			_Pragma("GCC diagnostic ignored \"-Wc99-extensions\"")
	#elif defined(__GNUC__)
	    /* I don't care, that this disables many errors, as this header should only be used from C,
	     * if it is used fom C++, the errors don't matter */
		#define __INTERNALS_IGNORE_C_COMPOUND_LITERALS_ERROR_IN_CPP() \
			_Pragma("GCC diagnostic ignored \"-Wpedantic\"")
	#else
		#define __INTERNALS_IGNORE_C_COMPOUND_LITERALS_ERROR_IN_CPP()
	#endif

	#define __INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
		_Pragma("GCC diagnostic push") __INTERNALS_IGNORE_C_COMPOUND_LITERALS_ERROR_IN_CPP()
	#define __INTERNAL_VARIANT_DECLARATION_END_IMPL_() _Pragma("GCC diagnostic pop")
#else
	#define __INTERNAL_VARIANT_DECLARATION_START_IMPL_()
	#define __INTERNAL_VARIANT_DECLARATION_END_IMPL_()
#endif

#include <tstr.h>

#ifdef __cplusplus
extern "C" {
__INTERNAL_VARIANT_DECLARATION_START_IMPL_()
#endif

// utils macros

#if !defined(VARIANT_IMPL_JSON_VARIANTS_COMPILED_WITH_NARROWED_ENUMS)
	#error \
	    "define 'VARIANT_IMPL_JSON_VARIANTS_COMPILED_WITH_NARROWED_ENUMS' to make enums work in all cases"
#elif VARIANT_IMPL_JSON_VARIANTS_COMPILED_WITH_NARROWED_ENUMS == 1
	#define _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(x) : x
#elif VARIANT_IMPL_JSON_VARIANTS_COMPILED_WITH_NARROWED_ENUMS == 0
	#define _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(x)
#else
	#error \
	    "value of 'VARIANT_IMPL_JSON_VARIANTS_COMPILED_WITH_NARROWED_ENUMS' can only be 0 or 1 (like a boolean)"
#endif

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202000) || __cplusplus
	#define JSON_VARIANTS_NODISCARD [[nodiscard]]
	#define JSON_VARIANTS_MAYBE_UNUSED [[maybe_unused]]
#else
    // see e.g. https://www.gnu.org/software/gnulib/manual/html_node/Attributes.html
	#define JSON_VARIANTS_NODISCARD __attribute__((__warn_unused_result__))
	#define JSON_VARIANTS_MAYBE_UNUSED __attribute__((__unused__))
#endif

#define JSON_VARIANTS_UNUSED(v) ((void)(v))

// cool trick from here:
// https://stackoverflow.com/questions/777261/avoiding-unused-variables-warnings-when-using-assert-in-a-release-build
#ifdef NDEBUG
	#define JSON_VARIANTS_UNREACHABLE() \
		do { \
			fprintf(stderr, "[%s %s:%d]: UNREACHABLE\n", __func__, __FILE__, __LINE__); \
			exit(EXIT_FAILURE); \
		} while(false)
#else
	#include <assert.h>

	#define JSON_VARIANTS_UNREACHABLE() \
		do { \
			assert(false && "UNREACHABLE"); /* NOLINT(cert-dcl03-c,misc-static-assert) */ \
		} while(false)
#endif

// end utils macros

#ifndef VARIANT_CASE_END
	#define VARIANT_CASE_END() \
		JSON_VARIANTS_UNREACHABLE_WITH_MESSAGE_SINGLE( \
		    "macro trick with for loops for getting the value was implemented wrong")
#endif

#ifndef VARIANT_STATE_ASSERT
	#define VARIANT_STATE_ASSERT(state, expected_state, variant_name, VariantName) \
		do { \
			if((state) != (expected_state)) { \
				tstr_static const state_str = \
				    _impl_get_state_string_for_variant_##variant_name(state); \
				tstr_static const expected_state_str = \
				    _impl_get_state_string_for_variant_##variant_name(expected_state); \
				JSON_VARIANTS_UNREACHABLE_WITH_MESSAGE( \
				    "Invalid variant access for variant '%s': state was " TSTR_FMT \
				    " but expected " TSTR_FMT, \
				    VariantName, TSTR_STATIC_FMT_ARGS(state_str), \
				    TSTR_STATIC_FMT_ARGS(expected_state_str)); \
			} \
		} while(false)
#endif

#define JSON_VARIANTS_UNREACHABLE_WITH_MESSAGE(msg, ...) \
	do { \
		fprintf(stderr, "[%s %s:%d]: UNREACHABLE: " msg "\n", __func__, __FILE__, __LINE__, \
		        __VA_ARGS__); \
		JSON_VARIANTS_UNREACHABLE(); \
	} while(false)

#define JSON_VARIANTS_UNREACHABLE_WITH_MESSAGE_SINGLE(msg) \
	JSON_VARIANTS_UNREACHABLE_WITH_MESSAGE(msg "%s", "")

#define VARIANT_JSON_VALUE_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, json_value, "JsonValue")

#define GENERATE_VARIANT_ENUM_JSON_VALUE() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(uint8_t) { \
		JsonValueTypeObject = 0, \
		JsonValueTypeArray, \
		JsonValueTypeNumber, \
		JsonValueTypeString, \
		JsonValueTypeBoolean, \
		JsonValueTypeNull \
	} JsonValueType;

#define GENERATE_VARIANT_CORE_JSON_VALUE() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	typedef struct { \
		JsonObject* obj; \
	} _variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_; \
\
	typedef struct { \
		JsonArray* arr; \
	} _variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_; \
\
	/* raw union for variant */ \
	typedef union { \
		_variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_ object; \
		_variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_ array; \
		JsonNumber number; \
		JsonString* string; \
		JsonBoolean boolean; \
	} _AnonymousUnionForVariantJsonValueImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		JsonValueType _variant_tag_for_json_value_tag_member; \
		_AnonymousUnionForVariantJsonValueImpl_DONT_USE _variant_data_for_json_value_data_member; \
	} JsonValue; \
	typedef struct { \
		JsonValueType _variant_tag_for_json_value_tag_member; \
		_AnonymousUnionForVariantJsonValueImpl_DONT_USE _variant_data_for_json_value_data_member; \
	} AssertTypeImplForJsonValueImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantJsonValueImpl_DONT_USE _variant_data_for_json_value_data_member; \
		JsonValueType _variant_tag_for_json_value_tag_member; \
	} AssertTypeImplForJsonValueImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForJsonValueImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForJsonValueImpl_2_DONT_USE_), \
	              "Size for variant JsonValue not smaller as the inverted order, current order: " \
	              "tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_json_value(JsonValueType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case JsonValueTypeObject: { \
			return TSTR_STATIC_LIT("object"); \
		} \
		case JsonValueTypeArray: { \
			return TSTR_STATIC_LIT("array"); \
		} \
		case JsonValueTypeNumber: { \
			return TSTR_STATIC_LIT("number"); \
		} \
		case JsonValueTypeString: { \
			return TSTR_STATIC_LIT("string"); \
		} \
		case JsonValueTypeBoolean: { \
			return TSTR_STATIC_LIT("boolean"); \
		} \
		case JsonValueTypeNull: { \
			return TSTR_STATIC_LIT("null"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValueType \
	get_current_tag_type_for_json_value(JsonValue const variant_entry) { \
		return variant_entry._variant_tag_for_json_value_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValue \
	new_json_value_object(JsonObject* const obj) { \
		return (JsonValue){ \
			._variant_tag_for_json_value_tag_member = JsonValueTypeObject, \
			._variant_data_for_json_value_data_member = \
			    (_AnonymousUnionForVariantJsonValueImpl_DONT_USE){ \
			        .object = \
			            (_variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_){ \
			                .obj = obj } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_ \
	    json_value_get_as_object(JsonValue const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry._variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeObject); \
		return variant_entry._variant_data_for_json_value_data_member.object; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_* \
	    json_value_get_as_object_mut_ref(JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeObject); \
		return &(variant_entry->_variant_data_for_json_value_data_member.object); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_* \
	    json_value_get_as_object_const_ref(const JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeObject); \
		return &(variant_entry->_variant_data_for_json_value_data_member.object); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValue \
	new_json_value_array(JsonArray* const arr) { \
		return (JsonValue){ \
			._variant_tag_for_json_value_tag_member = JsonValueTypeArray, \
			._variant_data_for_json_value_data_member = \
			    (_AnonymousUnionForVariantJsonValueImpl_DONT_USE){ \
			        .array = \
			            (_variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_){ \
			                .arr = arr } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_ \
	    json_value_get_as_array(JsonValue const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry._variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeArray); \
		return variant_entry._variant_data_for_json_value_data_member.array; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_* \
	    json_value_get_as_array_mut_ref(JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeArray); \
		return &(variant_entry->_variant_data_for_json_value_data_member.array); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_* \
	    json_value_get_as_array_const_ref(const JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeArray); \
		return &(variant_entry->_variant_data_for_json_value_data_member.array); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValue \
	new_json_value_number(JsonNumber const value) { \
		return (JsonValue){ ._variant_tag_for_json_value_tag_member = JsonValueTypeNumber, \
			                ._variant_data_for_json_value_data_member = \
			                    (_AnonymousUnionForVariantJsonValueImpl_DONT_USE){ .number = \
			                                                                           value } }; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonNumber \
	json_value_get_as_number(JsonValue const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry._variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeNumber); \
		return variant_entry._variant_data_for_json_value_data_member.number; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonNumber* \
	json_value_get_as_number_mut_ref(JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeNumber); \
		return &(variant_entry->_variant_data_for_json_value_data_member.number); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonNumber const* \
	json_value_get_as_number_const_ref(const JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeNumber); \
		return &(variant_entry->_variant_data_for_json_value_data_member.number); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValue \
	new_json_value_string(JsonString* const value) { \
		return (JsonValue){ ._variant_tag_for_json_value_tag_member = JsonValueTypeString, \
			                ._variant_data_for_json_value_data_member = \
			                    (_AnonymousUnionForVariantJsonValueImpl_DONT_USE){ .string = \
			                                                                           value } }; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonString* \
	json_value_get_as_string(JsonValue const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry._variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeString); \
		return variant_entry._variant_data_for_json_value_data_member.string; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonString** \
	json_value_get_as_string_mut_ref(JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeString); \
		return &(variant_entry->_variant_data_for_json_value_data_member.string); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonString* const* \
	json_value_get_as_string_const_ref(const JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeString); \
		return &(variant_entry->_variant_data_for_json_value_data_member.string); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValue \
	new_json_value_boolean(JsonBoolean const value) { \
		return (JsonValue){ ._variant_tag_for_json_value_tag_member = JsonValueTypeBoolean, \
			                ._variant_data_for_json_value_data_member = \
			                    (_AnonymousUnionForVariantJsonValueImpl_DONT_USE){ .boolean = \
			                                                                           value } }; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonBoolean \
	json_value_get_as_boolean(JsonValue const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry._variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeBoolean); \
		return variant_entry._variant_data_for_json_value_data_member.boolean; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonBoolean* \
	json_value_get_as_boolean_mut_ref(JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeBoolean); \
		return &(variant_entry->_variant_data_for_json_value_data_member.boolean); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonBoolean const* \
	json_value_get_as_boolean_const_ref(const JsonValue* const variant_entry) { \
		VARIANT_JSON_VALUE_STATE_ASSERT(variant_entry->_variant_tag_for_json_value_tag_member, \
		                                JsonValueTypeBoolean); \
		return &(variant_entry->_variant_data_for_json_value_data_member.boolean); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValue \
	new_json_value_null(void) { \
		return (JsonValue){ ._variant_tag_for_json_value_tag_member = JsonValueTypeNull, \
			                ._variant_data_for_json_value_data_member = \
			                    (_AnonymousUnionForVariantJsonValueImpl_DONT_USE){} }; \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_json_value") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_JSON_VALUE() \
	GENERATE_VARIANT_ENUM_JSON_VALUE() \
	GENERATE_VARIANT_CORE_JSON_VALUE()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_IMPL_2(variant_entry, object)

#define IF_JSON_VALUE_IS_OBJECT_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_IMPL_2(variant_entry, \
                                                                              var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeObject) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_value_data_member.object); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_MACRO_HELPER_IMPL_(_1, _2, \
                                                                                        NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_IMPL_2(variant_entry, object)

#define IF_JSON_VALUE_IS_OBJECT_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_IMPL_2(variant_entry, \
                                                                            var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeObject) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_value_data_member.object); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define IF_JSON_VALUE_IS_OBJECT_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeObject)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_2(variant_entry, object)

#define IF_JSON_VALUE_IS_OBJECT_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeObject) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_value_data_member.object); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_2(variant_entry, object)

#define IF_JSON_VALUE_IS_OBJECT_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_2(variant_entry, \
                                                                                var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeObject) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_value_data_member.object); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_IMPL_2(variant_entry, array)

#define IF_JSON_VALUE_IS_ARRAY_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_IMPL_2(variant_entry, \
                                                                             var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeArray) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_value_data_member.array); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_MACRO_HELPER_IMPL_(_1, _2, \
                                                                                       NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_IMPL_2(variant_entry, array)

#define IF_JSON_VALUE_IS_ARRAY_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_IMPL_2(variant_entry, \
                                                                           var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeArray) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_value_data_member.array); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define IF_JSON_VALUE_IS_ARRAY_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeArray)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_2(variant_entry, array)

#define IF_JSON_VALUE_IS_ARRAY_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeArray) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_* const \
			        var_name = &((variant_entry)->_variant_data_for_json_value_data_member.array); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_2(variant_entry, array)

#define IF_JSON_VALUE_IS_ARRAY_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_2(variant_entry, \
                                                                               var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeArray) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_* const \
			        var_name = &((variant_entry)->_variant_data_for_json_value_data_member.array); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_IMPL_2(variant_entry, number)

#define IF_JSON_VALUE_IS_NUMBER_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_IMPL_2(variant_entry, \
                                                                              var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeNumber) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonNumber const var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.number); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_MACRO_HELPER_IMPL_(_1, _2, \
                                                                                        NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_IMPL_2(variant_entry, number)

#define IF_JSON_VALUE_IS_NUMBER_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_IMPL_2(variant_entry, \
                                                                            var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeNumber) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonNumber var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.number); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define IF_JSON_VALUE_IS_NUMBER_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeNumber)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_2(variant_entry, number)

#define IF_JSON_VALUE_IS_NUMBER_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeNumber) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const JsonNumber* const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.number); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_2(variant_entry, number)

#define IF_JSON_VALUE_IS_NUMBER_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_2(variant_entry, \
                                                                                var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeNumber) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonNumber* const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.number); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_IMPL_2(variant_entry, string)

#define IF_JSON_VALUE_IS_STRING_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_IMPL_2(variant_entry, \
                                                                              var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonString* const var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_MACRO_HELPER_IMPL_(_1, _2, \
                                                                                        NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_IMPL_2(variant_entry, string)

#define IF_JSON_VALUE_IS_STRING_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_IMPL_2(variant_entry, \
                                                                            var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonString* var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define IF_JSON_VALUE_IS_STRING_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeString)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_REF_IMPL_2(variant_entry, string)

#define IF_JSON_VALUE_IS_STRING_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_CONST_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const JsonString** const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_REF_IMPL_2(variant_entry, string)

#define IF_JSON_VALUE_IS_STRING_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_STRING_MUT_REF_IMPL_2(variant_entry, \
                                                                                var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonString** const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_2(variant_entry, boolean)

#define IF_JSON_VALUE_IS_BOOLEAN_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_2(variant_entry, \
                                                                               var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeBoolean) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonBoolean const var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.boolean); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_2(variant_entry, boolean)

#define IF_JSON_VALUE_IS_BOOLEAN_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_2(variant_entry, \
                                                                             var_name) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeBoolean) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonBoolean var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.boolean); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define IF_JSON_VALUE_IS_BOOLEAN_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeBoolean)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2(variant_entry, \
	                                                                           boolean)

#define IF_JSON_VALUE_IS_BOOLEAN_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeBoolean) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const JsonBoolean* const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.boolean); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2(variant_entry, boolean)

#define IF_JSON_VALUE_IS_BOOLEAN_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)->_variant_tag_for_json_value_tag_member == JsonValueTypeBoolean) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonBoolean* const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.boolean); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl = false)

#define IF_JSON_VALUE_IS_NULL(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member == JsonValueTypeNull)

#define IF_JSON_VALUE_IS_NOT_OBJECT(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member != JsonValueTypeObject)

#define IF_JSON_VALUE_IS_NOT_ARRAY(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member != JsonValueTypeArray)

#define IF_JSON_VALUE_IS_NOT_NUMBER(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member != JsonValueTypeNumber)

#define IF_JSON_VALUE_IS_NOT_STRING(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member != JsonValueTypeString)

#define IF_JSON_VALUE_IS_NOT_BOOLEAN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member != JsonValueTypeBoolean)

#define IF_JSON_VALUE_IS_NOT_NULL(variant_entry) \
	if((variant_entry)._variant_tag_for_json_value_tag_member != JsonValueTypeNull)

#define SWITCH_JSON_VALUE(variant_entry) \
	switch((variant_entry)._variant_tag_for_json_value_tag_member)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_IMPL_2(variant_entry, object)

#define CASE_JSON_VALUE_IS_OBJECT_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_IMPL_2(variant_entry, \
                                                                                var_name) \
	case JsonValueTypeObject: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_value_data_member.object); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_IMPL_2(variant_entry, object)

#define CASE_JSON_VALUE_IS_OBJECT_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_IMPL_2(variant_entry, \
                                                                              var_name) \
	case JsonValueTypeObject: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_value_data_member.object); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define CASE_JSON_VALUE_IS_OBJECT_IGN() case JsonValueTypeObject:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_2(variant_entry, \
	                                                                            object)

#define CASE_JSON_VALUE_IS_OBJECT_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_CONST_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	case JsonValueTypeObject: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_value_data_member.object); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_2(variant_entry, object)

#define CASE_JSON_VALUE_IS_OBJECT_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_OBJECT_MUT_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	case JsonValueTypeObject: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_value_data_member.object); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_IMPL_2(variant_entry, array)

#define CASE_JSON_VALUE_IS_ARRAY_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_IMPL_2(variant_entry, \
                                                                               var_name) \
	case JsonValueTypeArray: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_value_data_member.array); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_IMPL_2(variant_entry, array)

#define CASE_JSON_VALUE_IS_ARRAY_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_IMPL_2(variant_entry, \
                                                                             var_name) \
	case JsonValueTypeArray: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_value_data_member.array); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define CASE_JSON_VALUE_IS_ARRAY_IGN() case JsonValueTypeArray:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_2(variant_entry, array)

#define CASE_JSON_VALUE_IS_ARRAY_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_CONST_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case JsonValueTypeArray: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_* const \
			        var_name = &((variant_entry)->_variant_data_for_json_value_data_member.array); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_2(variant_entry, array)

#define CASE_JSON_VALUE_IS_ARRAY_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_ARRAY_MUT_REF_IMPL_2(variant_entry, \
                                                                                 var_name) \
	case JsonValueTypeArray: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_* const \
			        var_name = &((variant_entry)->_variant_data_for_json_value_data_member.array); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_IMPL_2(variant_entry, number)

#define CASE_JSON_VALUE_IS_NUMBER_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_IMPL_2(variant_entry, \
                                                                                var_name) \
	case JsonValueTypeNumber: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonNumber const var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.number); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_IMPL_2(variant_entry, number)

#define CASE_JSON_VALUE_IS_NUMBER_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_IMPL_2(variant_entry, \
                                                                              var_name) \
	case JsonValueTypeNumber: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonNumber var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.number); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define CASE_JSON_VALUE_IS_NUMBER_IGN() case JsonValueTypeNumber:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_2(variant_entry, \
	                                                                            number)

#define CASE_JSON_VALUE_IS_NUMBER_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_CONST_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	case JsonValueTypeNumber: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const JsonNumber* const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.number); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_2(variant_entry, number)

#define CASE_JSON_VALUE_IS_NUMBER_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_NUMBER_MUT_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	case JsonValueTypeNumber: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonNumber* const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.number); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_IMPL_2(variant_entry, string)

#define CASE_JSON_VALUE_IS_STRING_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_IMPL_2(variant_entry, \
                                                                                var_name) \
	case JsonValueTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonString* const var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_IMPL_2(variant_entry, string)

#define CASE_JSON_VALUE_IS_STRING_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_IMPL_2(variant_entry, \
                                                                              var_name) \
	case JsonValueTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonString* var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define CASE_JSON_VALUE_IS_STRING_IGN() case JsonValueTypeString:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_REF_IMPL_2(variant_entry, \
	                                                                            string)

#define CASE_JSON_VALUE_IS_STRING_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_CONST_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	case JsonValueTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const JsonString** const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_REF_IMPL_2(variant_entry, string)

#define CASE_JSON_VALUE_IS_STRING_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_STRING_MUT_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	case JsonValueTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonString** const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_2(variant_entry, boolean)

#define CASE_JSON_VALUE_IS_BOOLEAN_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_IMPL_2(variant_entry, \
                                                                                 var_name) \
	case JsonValueTypeBoolean: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonBoolean const var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.boolean); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_2(variant_entry, boolean)

#define CASE_JSON_VALUE_IS_BOOLEAN_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_IMPL_2(variant_entry, \
                                                                               var_name) \
	case JsonValueTypeBoolean: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonBoolean var_name = \
			        ((variant_entry)._variant_data_for_json_value_data_member.boolean); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define CASE_JSON_VALUE_IS_BOOLEAN_IGN() case JsonValueTypeBoolean:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2(variant_entry, \
	                                                                             boolean)

#define CASE_JSON_VALUE_IS_BOOLEAN_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonValueTypeBoolean: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(const JsonBoolean* const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.boolean); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2(variant_entry, \
	                                                                           boolean)

#define CASE_JSON_VALUE_IS_BOOLEAN_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case JsonValueTypeBoolean: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false) \
			for(JsonBoolean* const var_name = \
			        &((variant_entry)->_variant_data_for_json_value_data_member.boolean); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl = false)

#define CASE_JSON_VALUE_IS_NULL() case JsonValueTypeNull:

#pragma GCC poison _variant_tag_for_json_value_tag_member _variant_data_for_json_value_data_member _for_macro_trick_for_if_expr_impl_once_variant_json_value_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_json_value_variant_impl _variant_impl_unnamed_struct_for_variant_json_value_id_0_name_object_impl_ _variant_impl_unnamed_struct_for_variant_json_value_id_1_name_array_impl_ _AnonymousUnionForVariantJsonValueImpl_DONT_USE AssertTypeImplForJsonValueImpl_1_DONT_USE_ AssertTypeImplForJsonValueImpl_2_DONT_USE_

#define VARIANT_JSON_PARSE_RESULT_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, json_parse_result, "JsonParseResult")

#define GENERATE_VARIANT_ENUM_JSON_PARSE_RESULT() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(bool) { \
		JsonParseResultTypeOk = false, \
		JsonParseResultTypeError = true \
	} JsonParseResultType;

#define GENERATE_VARIANT_CORE_JSON_PARSE_RESULT() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	/* raw union for variant */ \
	typedef union { \
		JsonValue ok; \
		JsonError error; \
	} _AnonymousUnionForVariantJsonParseResultImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		JsonParseResultType _variant_tag_for_json_parse_result_tag_member; \
		_AnonymousUnionForVariantJsonParseResultImpl_DONT_USE \
		    _variant_data_for_json_parse_result_data_member; \
	} JsonParseResult; \
	typedef struct { \
		JsonParseResultType _variant_tag_for_json_parse_result_tag_member; \
		_AnonymousUnionForVariantJsonParseResultImpl_DONT_USE \
		    _variant_data_for_json_parse_result_data_member; \
	} AssertTypeImplForJsonParseResultImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantJsonParseResultImpl_DONT_USE \
		    _variant_data_for_json_parse_result_data_member; \
		JsonParseResultType _variant_tag_for_json_parse_result_tag_member; \
	} AssertTypeImplForJsonParseResultImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForJsonParseResultImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForJsonParseResultImpl_2_DONT_USE_), \
	              "Size for variant JsonParseResult not smaller as the inverted order, current " \
	              "order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_json_parse_result(JsonParseResultType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case JsonParseResultTypeOk: { \
			return TSTR_STATIC_LIT("ok"); \
		} \
		case JsonParseResultTypeError: { \
			return TSTR_STATIC_LIT("error"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonParseResultType \
	get_current_tag_type_for_json_parse_result(JsonParseResult const variant_entry) { \
		return variant_entry._variant_tag_for_json_parse_result_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonParseResult \
	new_json_parse_result_ok(JsonValue const value) { \
		return (JsonParseResult){ \
			._variant_tag_for_json_parse_result_tag_member = JsonParseResultTypeOk, \
			._variant_data_for_json_parse_result_data_member = \
			    (_AnonymousUnionForVariantJsonParseResultImpl_DONT_USE){ .ok = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValue \
	json_parse_result_get_as_ok(JsonParseResult const variant_entry) { \
		VARIANT_JSON_PARSE_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_parse_result_tag_member, JsonParseResultTypeOk); \
		return variant_entry._variant_data_for_json_parse_result_data_member.ok; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValue* \
	json_parse_result_get_as_ok_mut_ref(JsonParseResult* const variant_entry) { \
		VARIANT_JSON_PARSE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_parse_result_tag_member, JsonParseResultTypeOk); \
		return &(variant_entry->_variant_data_for_json_parse_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonValue const* \
	json_parse_result_get_as_ok_const_ref(const JsonParseResult* const variant_entry) { \
		VARIANT_JSON_PARSE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_parse_result_tag_member, JsonParseResultTypeOk); \
		return &(variant_entry->_variant_data_for_json_parse_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonParseResult \
	new_json_parse_result_error(JsonError const value) { \
		return (JsonParseResult){ \
			._variant_tag_for_json_parse_result_tag_member = JsonParseResultTypeError, \
			._variant_data_for_json_parse_result_data_member = \
			    (_AnonymousUnionForVariantJsonParseResultImpl_DONT_USE){ .error = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonError \
	json_parse_result_get_as_error(JsonParseResult const variant_entry) { \
		VARIANT_JSON_PARSE_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_parse_result_tag_member, \
		    JsonParseResultTypeError); \
		return variant_entry._variant_data_for_json_parse_result_data_member.error; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonError* \
	json_parse_result_get_as_error_mut_ref(JsonParseResult* const variant_entry) { \
		VARIANT_JSON_PARSE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_parse_result_tag_member, \
		    JsonParseResultTypeError); \
		return &(variant_entry->_variant_data_for_json_parse_result_data_member.error); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonError const* \
	json_parse_result_get_as_error_const_ref(const JsonParseResult* const variant_entry) { \
		VARIANT_JSON_PARSE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_parse_result_tag_member, \
		    JsonParseResultTypeError); \
		return &(variant_entry->_variant_data_for_json_parse_result_data_member.error); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_json_parse_result") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_JSON_PARSE_RESULT() \
	GENERATE_VARIANT_ENUM_JSON_PARSE_RESULT() \
	GENERATE_VARIANT_CORE_JSON_PARSE_RESULT()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define IF_JSON_PARSE_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)._variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = false) \
			for(JsonValue const var_name = \
			        ((variant_entry)._variant_data_for_json_parse_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define IF_JSON_PARSE_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                               var_name) \
	if((variant_entry)._variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = false) \
			for(JsonValue var_name = \
			        ((variant_entry)._variant_data_for_json_parse_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define IF_JSON_PARSE_RESULT_IS_OK_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeOk)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, ok)

#define IF_JSON_PARSE_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = false) \
			for(const JsonValue* const var_name = \
			        &((variant_entry)->_variant_data_for_json_parse_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, ok)

#define IF_JSON_PARSE_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)->_variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = false) \
			for(JsonValue* const var_name = \
			        &((variant_entry)->_variant_data_for_json_parse_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
	                                                                            error)

#define IF_JSON_PARSE_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
                                                                                    var_name) \
	if((variant_entry)._variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = false) \
			for(JsonError const var_name = \
			        ((variant_entry)._variant_data_for_json_parse_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, error)

#define IF_JSON_PARSE_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
                                                                                  var_name) \
	if((variant_entry)._variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = false) \
			for(JsonError var_name = \
			        ((variant_entry)._variant_data_for_json_parse_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define IF_JSON_PARSE_RESULT_IS_ERROR_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeError)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_2(variant_entry, \
	                                                                                error)

#define IF_JSON_PARSE_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = false) \
			for(const JsonError* const var_name = \
			        &((variant_entry)->_variant_data_for_json_parse_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_2(variant_entry, \
	                                                                              error)

#define IF_JSON_PARSE_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_parse_result_tag_member == JsonParseResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = false) \
			for(JsonError* const var_name = \
			        &((variant_entry)->_variant_data_for_json_parse_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define IF_JSON_PARSE_RESULT_IS_NOT_OK(variant_entry) \
	if((variant_entry)._variant_tag_for_json_parse_result_tag_member != JsonParseResultTypeOk)

#define IF_JSON_PARSE_RESULT_IS_NOT_ERROR(variant_entry) \
	if((variant_entry)._variant_tag_for_json_parse_result_tag_member != JsonParseResultTypeError)

#define SWITCH_JSON_PARSE_RESULT(variant_entry) \
	_Pragma("GCC diagnostic push") \
	    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ switch( \
	        (variant_entry)._variant_tag_for_json_parse_result_tag_member) \
	        _Pragma("GCC diagnostic pop")

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define CASE_JSON_PARSE_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case JsonParseResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        false) \
			for(JsonValue const var_name = \
			        ((variant_entry)._variant_data_for_json_parse_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define CASE_JSON_PARSE_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                                 var_name) \
	case JsonParseResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        false) \
			for(JsonValue var_name = \
			        ((variant_entry)._variant_data_for_json_parse_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define CASE_JSON_PARSE_RESULT_IS_OK_IGN() case JsonParseResultTypeOk:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, \
	                                                                               ok)

#define CASE_JSON_PARSE_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonParseResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        false) \
			for(const JsonValue* const var_name = \
			        &((variant_entry)->_variant_data_for_json_parse_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, ok)

#define CASE_JSON_PARSE_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_OK_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonParseResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        false) \
			for(JsonValue* const var_name = \
			        &((variant_entry)->_variant_data_for_json_parse_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
	                                                                              error)

#define CASE_JSON_PARSE_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonParseResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        false) \
			for(JsonError const var_name = \
			        ((variant_entry)._variant_data_for_json_parse_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
	                                                                            error)

#define CASE_JSON_PARSE_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
                                                                                    var_name) \
	case JsonParseResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        false) \
			for(JsonError var_name = \
			        ((variant_entry)._variant_data_for_json_parse_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define CASE_JSON_PARSE_RESULT_IS_ERROR_IGN() case JsonParseResultTypeError:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonParseResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        false) \
			for(const JsonError* const var_name = \
			        &((variant_entry)->_variant_data_for_json_parse_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_2(variant_entry, \
	                                                                                error)

#define CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PARSE_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonParseResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
		        false) \
			for(JsonError* const var_name = \
			        &((variant_entry)->_variant_data_for_json_parse_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl = \
			        false)

#pragma GCC poison _variant_tag_for_json_parse_result_tag_member _variant_data_for_json_parse_result_data_member _for_macro_trick_for_if_expr_impl_once_variant_json_parse_result_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_json_parse_result_variant_impl _AnonymousUnionForVariantJsonParseResultImpl_DONT_USE AssertTypeImplForJsonParseResultImpl_1_DONT_USE_ AssertTypeImplForJsonParseResultImpl_2_DONT_USE_

#define VARIANT_JSON_SOURCE_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, json_source, "JsonSource")

#define GENERATE_VARIANT_ENUM_JSON_SOURCE() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(bool) { \
		JsonSourceTypeFile = false, \
		JsonSourceTypeString = true \
	} JsonSourceType;

#define GENERATE_VARIANT_CORE_JSON_SOURCE() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	/* raw union for variant */ \
	typedef union { \
		JsonFileSource file; \
		JsonStringSource string; \
	} _AnonymousUnionForVariantJsonSourceImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		JsonSourceType _variant_tag_for_json_source_tag_member; \
		_AnonymousUnionForVariantJsonSourceImpl_DONT_USE \
		    _variant_data_for_json_source_data_member; \
	} JsonSource; \
	typedef struct { \
		JsonSourceType _variant_tag_for_json_source_tag_member; \
		_AnonymousUnionForVariantJsonSourceImpl_DONT_USE \
		    _variant_data_for_json_source_data_member; \
	} AssertTypeImplForJsonSourceImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantJsonSourceImpl_DONT_USE \
		    _variant_data_for_json_source_data_member; \
		JsonSourceType _variant_tag_for_json_source_tag_member; \
	} AssertTypeImplForJsonSourceImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForJsonSourceImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForJsonSourceImpl_2_DONT_USE_), \
	              "Size for variant JsonSource not smaller as the inverted order, current order: " \
	              "tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_json_source(JsonSourceType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case JsonSourceTypeFile: { return TSTR_STATIC_LIT("file"); } \
		case JsonSourceTypeString: { \
			return TSTR_STATIC_LIT("string"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSourceType \
	get_current_tag_type_for_json_source(JsonSource const variant_entry) { \
		return variant_entry._variant_tag_for_json_source_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSource \
	new_json_source_file(JsonFileSource const value) { \
		return (JsonSource){ ._variant_tag_for_json_source_tag_member = JsonSourceTypeFile, \
			                 ._variant_data_for_json_source_data_member = \
			                     (_AnonymousUnionForVariantJsonSourceImpl_DONT_USE){ \
			                         .file = value } }; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonFileSource \
	json_source_get_as_file(JsonSource const variant_entry) { \
		VARIANT_JSON_SOURCE_STATE_ASSERT(variant_entry._variant_tag_for_json_source_tag_member, \
		                                 JsonSourceTypeFile); \
		return variant_entry._variant_data_for_json_source_data_member.file; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonFileSource* \
	json_source_get_as_file_mut_ref(JsonSource* const variant_entry) { \
		VARIANT_JSON_SOURCE_STATE_ASSERT(variant_entry->_variant_tag_for_json_source_tag_member, \
		                                 JsonSourceTypeFile); \
		return &(variant_entry->_variant_data_for_json_source_data_member.file); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonFileSource const* \
	json_source_get_as_file_const_ref(const JsonSource* const variant_entry) { \
		VARIANT_JSON_SOURCE_STATE_ASSERT(variant_entry->_variant_tag_for_json_source_tag_member, \
		                                 JsonSourceTypeFile); \
		return &(variant_entry->_variant_data_for_json_source_data_member.file); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSource \
	new_json_source_string(JsonStringSource const value) { \
		return (JsonSource){ ._variant_tag_for_json_source_tag_member = JsonSourceTypeString, \
			                 ._variant_data_for_json_source_data_member = \
			                     (_AnonymousUnionForVariantJsonSourceImpl_DONT_USE){ \
			                         .string = value } }; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonStringSource \
	json_source_get_as_string(JsonSource const variant_entry) { \
		VARIANT_JSON_SOURCE_STATE_ASSERT(variant_entry._variant_tag_for_json_source_tag_member, \
		                                 JsonSourceTypeString); \
		return variant_entry._variant_data_for_json_source_data_member.string; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonStringSource* \
	json_source_get_as_string_mut_ref(JsonSource* const variant_entry) { \
		VARIANT_JSON_SOURCE_STATE_ASSERT(variant_entry->_variant_tag_for_json_source_tag_member, \
		                                 JsonSourceTypeString); \
		return &(variant_entry->_variant_data_for_json_source_data_member.string); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonStringSource const* \
	json_source_get_as_string_const_ref(const JsonSource* const variant_entry) { \
		VARIANT_JSON_SOURCE_STATE_ASSERT(variant_entry->_variant_tag_for_json_source_tag_member, \
		                                 JsonSourceTypeString); \
		return &(variant_entry->_variant_data_for_json_source_data_member.string); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_json_source") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_JSON_SOURCE() \
	GENERATE_VARIANT_ENUM_JSON_SOURCE() \
	GENERATE_VARIANT_CORE_JSON_SOURCE()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_IMPL_2(variant_entry, file)

#define IF_JSON_SOURCE_IS_FILE_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_IMPL_2(variant_entry, \
                                                                             var_name) \
	if((variant_entry)._variant_tag_for_json_source_tag_member == JsonSourceTypeFile) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonFileSource const var_name = \
			        ((variant_entry)._variant_data_for_json_source_data_member.file); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_MACRO_HELPER_IMPL_(_1, _2, \
                                                                                       NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_IMPL_2(variant_entry, file)

#define IF_JSON_SOURCE_IS_FILE_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_IMPL_2(variant_entry, \
                                                                           var_name) \
	if((variant_entry)._variant_tag_for_json_source_tag_member == JsonSourceTypeFile) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonFileSource var_name = \
			        ((variant_entry)._variant_data_for_json_source_data_member.file); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false)

#define IF_JSON_SOURCE_IS_FILE_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_source_tag_member == JsonSourceTypeFile)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_2(variant_entry, file)

#define IF_JSON_SOURCE_IS_FILE_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)->_variant_tag_for_json_source_tag_member == JsonSourceTypeFile) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false) \
			for(const JsonFileSource* const var_name = \
			        &((variant_entry)->_variant_data_for_json_source_data_member.file); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_2(variant_entry, file)

#define IF_JSON_SOURCE_IS_FILE_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_2(variant_entry, \
                                                                               var_name) \
	if((variant_entry)->_variant_tag_for_json_source_tag_member == JsonSourceTypeFile) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonFileSource* const var_name = \
			        &((variant_entry)->_variant_data_for_json_source_data_member.file); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_IMPL_2(variant_entry, string)

#define IF_JSON_SOURCE_IS_STRING_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_IMPL_2(variant_entry, \
                                                                               var_name) \
	if((variant_entry)._variant_tag_for_json_source_tag_member == JsonSourceTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonStringSource const var_name = \
			        ((variant_entry)._variant_data_for_json_source_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_IMPL_2(variant_entry, string)

#define IF_JSON_SOURCE_IS_STRING_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_IMPL_2(variant_entry, \
                                                                             var_name) \
	if((variant_entry)._variant_tag_for_json_source_tag_member == JsonSourceTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonStringSource var_name = \
			        ((variant_entry)._variant_data_for_json_source_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false)

#define IF_JSON_SOURCE_IS_STRING_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_source_tag_member == JsonSourceTypeString)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_2(variant_entry, \
	                                                                           string)

#define IF_JSON_SOURCE_IS_STRING_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)->_variant_tag_for_json_source_tag_member == JsonSourceTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false) \
			for(const JsonStringSource* const var_name = \
			        &((variant_entry)->_variant_data_for_json_source_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_2(variant_entry, string)

#define IF_JSON_SOURCE_IS_STRING_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)->_variant_tag_for_json_source_tag_member == JsonSourceTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonStringSource* const var_name = \
			        &((variant_entry)->_variant_data_for_json_source_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl = false)

#define IF_JSON_SOURCE_IS_NOT_FILE(variant_entry) \
	if((variant_entry)._variant_tag_for_json_source_tag_member != JsonSourceTypeFile)

#define IF_JSON_SOURCE_IS_NOT_STRING(variant_entry) \
	if((variant_entry)._variant_tag_for_json_source_tag_member != JsonSourceTypeString)

#define SWITCH_JSON_SOURCE(variant_entry) \
	_Pragma("GCC diagnostic push") \
	    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ switch( \
	        (variant_entry)._variant_tag_for_json_source_tag_member) _Pragma("GCC diagnostic pop")

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_IMPL_2(variant_entry, file)

#define CASE_JSON_SOURCE_IS_FILE_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_IMPL_2(variant_entry, \
                                                                               var_name) \
	case JsonSourceTypeFile: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonFileSource const var_name = \
			        ((variant_entry)._variant_data_for_json_source_data_member.file); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_IMPL_2(variant_entry, file)

#define CASE_JSON_SOURCE_IS_FILE_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_IMPL_2(variant_entry, \
                                                                             var_name) \
	case JsonSourceTypeFile: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonFileSource var_name = \
			        ((variant_entry)._variant_data_for_json_source_data_member.file); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false)

#define CASE_JSON_SOURCE_IS_FILE_IGN() case JsonSourceTypeFile:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_2(variant_entry, file)

#define CASE_JSON_SOURCE_IS_FILE_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_CONST_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case JsonSourceTypeFile: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false) \
			for(const JsonFileSource* const var_name = \
			        &((variant_entry)->_variant_data_for_json_source_data_member.file); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_2(variant_entry, file)

#define CASE_JSON_SOURCE_IS_FILE_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_FILE_MUT_REF_IMPL_2(variant_entry, \
                                                                                 var_name) \
	case JsonSourceTypeFile: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonFileSource* const var_name = \
			        &((variant_entry)->_variant_data_for_json_source_data_member.file); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_IMPL_2(variant_entry, string)

#define CASE_JSON_SOURCE_IS_STRING_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_IMPL_2(variant_entry, \
                                                                                 var_name) \
	case JsonSourceTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonStringSource const var_name = \
			        ((variant_entry)._variant_data_for_json_source_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_IMPL_2(variant_entry, string)

#define CASE_JSON_SOURCE_IS_STRING_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_IMPL_2(variant_entry, \
                                                                               var_name) \
	case JsonSourceTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonStringSource var_name = \
			        ((variant_entry)._variant_data_for_json_source_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false)

#define CASE_JSON_SOURCE_IS_STRING_IGN() case JsonSourceTypeString:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_2(variant_entry, \
	                                                                             string)

#define CASE_JSON_SOURCE_IS_STRING_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonSourceTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false) \
			for(const JsonStringSource* const var_name = \
			        &((variant_entry)->_variant_data_for_json_source_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_2(variant_entry, \
	                                                                           string)

#define CASE_JSON_SOURCE_IS_STRING_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SOURCE_IS_STRING_MUT_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case JsonSourceTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false) \
			for(JsonStringSource* const var_name = \
			        &((variant_entry)->_variant_data_for_json_source_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl = false)

#pragma GCC poison _variant_tag_for_json_source_tag_member _variant_data_for_json_source_data_member _for_macro_trick_for_if_expr_impl_once_variant_json_source_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_json_source_variant_impl _AnonymousUnionForVariantJsonSourceImpl_DONT_USE AssertTypeImplForJsonSourceImpl_1_DONT_USE_ AssertTypeImplForJsonSourceImpl_2_DONT_USE_

#define VARIANT_JSON_SCHEMA_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, json_schema, "JsonSchema")

#define GENERATE_VARIANT_ENUM_JSON_SCHEMA() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(uint8_t) { \
		JsonSchemaTypeObject = 0, \
		JsonSchemaTypeArray, \
		JsonSchemaTypeNumber, \
		JsonSchemaTypeString, \
		JsonSchemaTypeBoolean, \
		JsonSchemaTypeNull, \
		JsonSchemaTypeOneOf, \
		JsonSchemaTypeLiteral \
	} JsonSchemaType;

#define GENERATE_VARIANT_CORE_JSON_SCHEMA() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	typedef struct { \
		JsonSchemaObject* obj; \
	} _variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_; \
\
	typedef struct { \
		JsonSchemaArray* arr; \
	} _variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_; \
\
	typedef struct { \
		JsonSchemaString* str; \
	} _variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_; \
\
	typedef struct { \
		JsonSchemaOneOf* one_of; \
	} _variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_; \
\
	typedef struct { \
		JsonSchemaLiteral* lit; \
	} _variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_; \
\
	/* raw union for variant */ \
	typedef union { \
		_variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_ object; \
		_variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_ array; \
		_variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_ string; \
		_variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_ one_of; \
		_variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_ literal; \
	} _AnonymousUnionForVariantJsonSchemaImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		JsonSchemaType _variant_tag_for_json_schema_tag_member; \
		_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE \
		    _variant_data_for_json_schema_data_member; \
	} JsonSchema; \
	typedef struct { \
		JsonSchemaType _variant_tag_for_json_schema_tag_member; \
		_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE \
		    _variant_data_for_json_schema_data_member; \
	} AssertTypeImplForJsonSchemaImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE \
		    _variant_data_for_json_schema_data_member; \
		JsonSchemaType _variant_tag_for_json_schema_tag_member; \
	} AssertTypeImplForJsonSchemaImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForJsonSchemaImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForJsonSchemaImpl_2_DONT_USE_), \
	              "Size for variant JsonSchema not smaller as the inverted order, current order: " \
	              "tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_json_schema(JsonSchemaType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case JsonSchemaTypeObject: { \
			return TSTR_STATIC_LIT("object"); \
		} \
		case JsonSchemaTypeArray: { \
			return TSTR_STATIC_LIT("array"); \
		} \
		case JsonSchemaTypeNumber: { \
			return TSTR_STATIC_LIT("number"); \
		} \
		case JsonSchemaTypeString: { \
			return TSTR_STATIC_LIT("string"); \
		} \
		case JsonSchemaTypeBoolean: { \
			return TSTR_STATIC_LIT("boolean"); \
		} \
		case JsonSchemaTypeNull: { \
			return TSTR_STATIC_LIT("null"); \
		} \
		case JsonSchemaTypeOneOf: { \
			return TSTR_STATIC_LIT("one_of"); \
		} \
		case JsonSchemaTypeLiteral: { \
			return TSTR_STATIC_LIT("literal"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchemaType \
	get_current_tag_type_for_json_schema(JsonSchema const variant_entry) { \
		return variant_entry._variant_tag_for_json_schema_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchema \
	new_json_schema_object(JsonSchemaObject* const obj) { \
		return (JsonSchema){ \
			._variant_tag_for_json_schema_tag_member = JsonSchemaTypeObject, \
			._variant_data_for_json_schema_data_member = \
			    (_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE){ \
			        .object = \
			            (_variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_){ \
			                .obj = obj } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_ \
	    json_schema_get_as_object(JsonSchema const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry._variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeObject); \
		return variant_entry._variant_data_for_json_schema_data_member.object; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_* \
	    json_schema_get_as_object_mut_ref(JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeObject); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.object); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_* \
	    json_schema_get_as_object_const_ref(const JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeObject); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.object); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchema \
	new_json_schema_array(JsonSchemaArray* const arr) { \
		return (JsonSchema){ \
			._variant_tag_for_json_schema_tag_member = JsonSchemaTypeArray, \
			._variant_data_for_json_schema_data_member = \
			    (_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE){ \
			        .array = \
			            (_variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_){ \
			                .arr = arr } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_ \
	    json_schema_get_as_array(JsonSchema const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry._variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeArray); \
		return variant_entry._variant_data_for_json_schema_data_member.array; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_* \
	    json_schema_get_as_array_mut_ref(JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeArray); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.array); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_* \
	    json_schema_get_as_array_const_ref(const JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeArray); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.array); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchema \
	new_json_schema_number(void) { \
		return (JsonSchema){ ._variant_tag_for_json_schema_tag_member = JsonSchemaTypeNumber, \
			                 ._variant_data_for_json_schema_data_member = \
			                     (_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE){} }; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchema \
	new_json_schema_string(JsonSchemaString* const str) { \
		return (JsonSchema){ \
			._variant_tag_for_json_schema_tag_member = JsonSchemaTypeString, \
			._variant_data_for_json_schema_data_member = \
			    (_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE){ \
			        .string = \
			            (_variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_){ \
			                .str = str } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_ \
	    json_schema_get_as_string(JsonSchema const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry._variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeString); \
		return variant_entry._variant_data_for_json_schema_data_member.string; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_* \
	    json_schema_get_as_string_mut_ref(JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeString); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.string); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_* \
	    json_schema_get_as_string_const_ref(const JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeString); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.string); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchema \
	new_json_schema_boolean(void) { \
		return (JsonSchema){ ._variant_tag_for_json_schema_tag_member = JsonSchemaTypeBoolean, \
			                 ._variant_data_for_json_schema_data_member = \
			                     (_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE){} }; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchema \
	new_json_schema_null(void) { \
		return (JsonSchema){ ._variant_tag_for_json_schema_tag_member = JsonSchemaTypeNull, \
			                 ._variant_data_for_json_schema_data_member = \
			                     (_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE){} }; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchema \
	new_json_schema_one_of(JsonSchemaOneOf* const one_of) { \
		return (JsonSchema){ \
			._variant_tag_for_json_schema_tag_member = JsonSchemaTypeOneOf, \
			._variant_data_for_json_schema_data_member = \
			    (_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE){ \
			        .one_of = \
			            (_variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_){ \
			                .one_of = one_of } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_ \
	    json_schema_get_as_one_of(JsonSchema const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry._variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeOneOf); \
		return variant_entry._variant_data_for_json_schema_data_member.one_of; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_* \
	    json_schema_get_as_one_of_mut_ref(JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeOneOf); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.one_of); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_* \
	    json_schema_get_as_one_of_const_ref(const JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeOneOf); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.one_of); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchema \
	new_json_schema_literal(JsonSchemaLiteral* const lit) { \
		return (JsonSchema){ \
			._variant_tag_for_json_schema_tag_member = JsonSchemaTypeLiteral, \
			._variant_data_for_json_schema_data_member = \
			    (_AnonymousUnionForVariantJsonSchemaImpl_DONT_USE){ \
			        .literal = \
			            (_variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_){ \
			                .lit = lit } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_ \
	    json_schema_get_as_literal(JsonSchema const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry._variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeLiteral); \
		return variant_entry._variant_data_for_json_schema_data_member.literal; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_* \
	    json_schema_get_as_literal_mut_ref(JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeLiteral); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.literal); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_* \
	    json_schema_get_as_literal_const_ref(const JsonSchema* const variant_entry) { \
		VARIANT_JSON_SCHEMA_STATE_ASSERT(variant_entry->_variant_tag_for_json_schema_tag_member, \
		                                 JsonSchemaTypeLiteral); \
		return &(variant_entry->_variant_data_for_json_schema_data_member.literal); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_json_schema") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_JSON_SCHEMA() \
	GENERATE_VARIANT_ENUM_JSON_SCHEMA() \
	GENERATE_VARIANT_CORE_JSON_SCHEMA()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_2(variant_entry, object)

#define IF_JSON_SCHEMA_IS_OBJECT_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_2(variant_entry, \
                                                                               var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeObject) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.object); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_2(variant_entry, object)

#define IF_JSON_SCHEMA_IS_OBJECT_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_2(variant_entry, \
                                                                             var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeObject) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.object); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define IF_JSON_SCHEMA_IS_OBJECT_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeObject)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_2(variant_entry, \
	                                                                           object)

#define IF_JSON_SCHEMA_IS_OBJECT_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeObject) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.object); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_2(variant_entry, object)

#define IF_JSON_SCHEMA_IS_OBJECT_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeObject) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.object); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_2(variant_entry, array)

#define IF_JSON_SCHEMA_IS_ARRAY_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_2(variant_entry, \
                                                                              var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeArray) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.array); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_MACRO_HELPER_IMPL_(_1, _2, \
                                                                                        NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_2(variant_entry, array)

#define IF_JSON_SCHEMA_IS_ARRAY_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_2(variant_entry, \
                                                                            var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeArray) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.array); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define IF_JSON_SCHEMA_IS_ARRAY_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeArray)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_2(variant_entry, array)

#define IF_JSON_SCHEMA_IS_ARRAY_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeArray) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.array); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_2(variant_entry, array)

#define IF_JSON_SCHEMA_IS_ARRAY_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_2(variant_entry, \
                                                                                var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeArray) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.array); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define IF_JSON_SCHEMA_IS_NUMBER(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeNumber)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_IMPL_2(variant_entry, string)

#define IF_JSON_SCHEMA_IS_STRING_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_IMPL_2(variant_entry, \
                                                                               var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_IMPL_2(variant_entry, string)

#define IF_JSON_SCHEMA_IS_STRING_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_IMPL_2(variant_entry, \
                                                                             var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define IF_JSON_SCHEMA_IS_STRING_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeString)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_2(variant_entry, \
	                                                                           string)

#define IF_JSON_SCHEMA_IS_STRING_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_2(variant_entry, string)

#define IF_JSON_SCHEMA_IS_STRING_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define IF_JSON_SCHEMA_IS_BOOLEAN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeBoolean)

#define IF_JSON_SCHEMA_IS_NULL(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeNull)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_2(variant_entry, one_of)

#define IF_JSON_SCHEMA_IS_ONE_OF_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_2(variant_entry, \
                                                                               var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeOneOf) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.one_of); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_2(variant_entry, one_of)

#define IF_JSON_SCHEMA_IS_ONE_OF_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_2(variant_entry, \
                                                                             var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeOneOf) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.one_of); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define IF_JSON_SCHEMA_IS_ONE_OF_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeOneOf)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_2(variant_entry, \
	                                                                           one_of)

#define IF_JSON_SCHEMA_IS_ONE_OF_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeOneOf) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.one_of); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_2(variant_entry, one_of)

#define IF_JSON_SCHEMA_IS_ONE_OF_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeOneOf) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.one_of); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_2(variant_entry, literal)

#define IF_JSON_SCHEMA_IS_LITERAL_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_2(variant_entry, \
                                                                                var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeLiteral) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_ const \
			        var_name = \
			            ((variant_entry)._variant_data_for_json_schema_data_member.literal); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_2(variant_entry, literal)

#define IF_JSON_SCHEMA_IS_LITERAL_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_2(variant_entry, \
                                                                              var_name) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeLiteral) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_ \
			        var_name = \
			            ((variant_entry)._variant_data_for_json_schema_data_member.literal); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define IF_JSON_SCHEMA_IS_LITERAL_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member == JsonSchemaTypeLiteral)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_2(variant_entry, \
	                                                                            literal)

#define IF_JSON_SCHEMA_IS_LITERAL_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeLiteral) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.literal); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_2(variant_entry, \
	                                                                          literal)

#define IF_JSON_SCHEMA_IS_LITERAL_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_tag_member == JsonSchemaTypeLiteral) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.literal); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl = false)

#define IF_JSON_SCHEMA_IS_NOT_OBJECT(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member != JsonSchemaTypeObject)

#define IF_JSON_SCHEMA_IS_NOT_ARRAY(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member != JsonSchemaTypeArray)

#define IF_JSON_SCHEMA_IS_NOT_NUMBER(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member != JsonSchemaTypeNumber)

#define IF_JSON_SCHEMA_IS_NOT_STRING(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member != JsonSchemaTypeString)

#define IF_JSON_SCHEMA_IS_NOT_BOOLEAN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member != JsonSchemaTypeBoolean)

#define IF_JSON_SCHEMA_IS_NOT_NULL(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member != JsonSchemaTypeNull)

#define IF_JSON_SCHEMA_IS_NOT_ONE_OF(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member != JsonSchemaTypeOneOf)

#define IF_JSON_SCHEMA_IS_NOT_LITERAL(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_tag_member != JsonSchemaTypeLiteral)

#define SWITCH_JSON_SCHEMA(variant_entry) \
	switch((variant_entry)._variant_tag_for_json_schema_tag_member)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_2(variant_entry, object)

#define CASE_JSON_SCHEMA_IS_OBJECT_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_IMPL_2(variant_entry, \
                                                                                 var_name) \
	case JsonSchemaTypeObject: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.object); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_2(variant_entry, object)

#define CASE_JSON_SCHEMA_IS_OBJECT_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_IMPL_2(variant_entry, \
                                                                               var_name) \
	case JsonSchemaTypeObject: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.object); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define CASE_JSON_SCHEMA_IS_OBJECT_IGN() case JsonSchemaTypeObject:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_2(variant_entry, \
	                                                                             object)

#define CASE_JSON_SCHEMA_IS_OBJECT_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaTypeObject: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.object); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_2(variant_entry, \
	                                                                           object)

#define CASE_JSON_SCHEMA_IS_OBJECT_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_OBJECT_MUT_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case JsonSchemaTypeObject: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.object); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_2(variant_entry, array)

#define CASE_JSON_SCHEMA_IS_ARRAY_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_IMPL_2(variant_entry, \
                                                                                var_name) \
	case JsonSchemaTypeArray: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.array); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_2(variant_entry, array)

#define CASE_JSON_SCHEMA_IS_ARRAY_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_IMPL_2(variant_entry, \
                                                                              var_name) \
	case JsonSchemaTypeArray: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.array); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define CASE_JSON_SCHEMA_IS_ARRAY_IGN() case JsonSchemaTypeArray:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_2(variant_entry, \
	                                                                            array)

#define CASE_JSON_SCHEMA_IS_ARRAY_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_CONST_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	case JsonSchemaTypeArray: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.array); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_2(variant_entry, array)

#define CASE_JSON_SCHEMA_IS_ARRAY_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ARRAY_MUT_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	case JsonSchemaTypeArray: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.array); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define CASE_JSON_SCHEMA_IS_NUMBER() case JsonSchemaTypeNumber:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_IMPL_2(variant_entry, string)

#define CASE_JSON_SCHEMA_IS_STRING_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_IMPL_2(variant_entry, \
                                                                                 var_name) \
	case JsonSchemaTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_IMPL_2(variant_entry, string)

#define CASE_JSON_SCHEMA_IS_STRING_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_IMPL_2(variant_entry, \
                                                                               var_name) \
	case JsonSchemaTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define CASE_JSON_SCHEMA_IS_STRING_IGN() case JsonSchemaTypeString:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_2(variant_entry, \
	                                                                             string)

#define CASE_JSON_SCHEMA_IS_STRING_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_2(variant_entry, \
	                                                                           string)

#define CASE_JSON_SCHEMA_IS_STRING_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_STRING_MUT_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case JsonSchemaTypeString: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define CASE_JSON_SCHEMA_IS_BOOLEAN() case JsonSchemaTypeBoolean:

#define CASE_JSON_SCHEMA_IS_NULL() case JsonSchemaTypeNull:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_2(variant_entry, one_of)

#define CASE_JSON_SCHEMA_IS_ONE_OF_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_IMPL_2(variant_entry, \
                                                                                 var_name) \
	case JsonSchemaTypeOneOf: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_ const \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.one_of); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_2(variant_entry, one_of)

#define CASE_JSON_SCHEMA_IS_ONE_OF_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_IMPL_2(variant_entry, \
                                                                               var_name) \
	case JsonSchemaTypeOneOf: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_ \
			        var_name = ((variant_entry)._variant_data_for_json_schema_data_member.one_of); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define CASE_JSON_SCHEMA_IS_ONE_OF_IGN() case JsonSchemaTypeOneOf:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_2(variant_entry, \
	                                                                             one_of)

#define CASE_JSON_SCHEMA_IS_ONE_OF_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaTypeOneOf: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.one_of); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_2(variant_entry, \
	                                                                           one_of)

#define CASE_JSON_SCHEMA_IS_ONE_OF_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_ONE_OF_MUT_REF_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case JsonSchemaTypeOneOf: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.one_of); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_2(variant_entry, \
	                                                                          literal)

#define CASE_JSON_SCHEMA_IS_LITERAL_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_IMPL_2(variant_entry, \
                                                                                  var_name) \
	case JsonSchemaTypeLiteral: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_ const \
			        var_name = \
			            ((variant_entry)._variant_data_for_json_schema_data_member.literal); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_2(variant_entry, literal)

#define CASE_JSON_SCHEMA_IS_LITERAL_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_IMPL_2(variant_entry, \
                                                                                var_name) \
	case JsonSchemaTypeLiteral: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_ \
			        var_name = \
			            ((variant_entry)._variant_data_for_json_schema_data_member.literal); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define CASE_JSON_SCHEMA_IS_LITERAL_IGN() case JsonSchemaTypeLiteral:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_2(variant_entry, \
	                                                                              literal)

#define CASE_JSON_SCHEMA_IS_LITERAL_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaTypeLiteral: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.literal); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_2(variant_entry, \
	                                                                            literal)

#define CASE_JSON_SCHEMA_IS_LITERAL_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_IS_LITERAL_MUT_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	case JsonSchemaTypeLiteral: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_json_schema_data_member.literal); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl = false)

#pragma GCC poison _variant_tag_for_json_schema_tag_member _variant_data_for_json_schema_data_member _for_macro_trick_for_if_expr_impl_once_variant_json_schema_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_json_schema_variant_impl _variant_impl_unnamed_struct_for_variant_json_schema_id_2_name_object_impl_ _variant_impl_unnamed_struct_for_variant_json_schema_id_3_name_array_impl_ _variant_impl_unnamed_struct_for_variant_json_schema_id_4_name_string_impl_ _variant_impl_unnamed_struct_for_variant_json_schema_id_5_name_one_of_impl_ _variant_impl_unnamed_struct_for_variant_json_schema_id_6_name_literal_impl_ _AnonymousUnionForVariantJsonSchemaImpl_DONT_USE AssertTypeImplForJsonSchemaImpl_1_DONT_USE_ AssertTypeImplForJsonSchemaImpl_2_DONT_USE_

#define VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, json_iterate_value, "JsonIterateValue")

#define GENERATE_VARIANT_ENUM_JSON_ITERATE_VALUE() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(uint8_t) { \
		JsonIterateValueTypeObjectStart = 0, \
		JsonIterateValueTypeObjectEnd, \
		JsonIterateValueTypeObjectEntry, \
		JsonIterateValueTypeArrayStart, \
		JsonIterateValueTypeArrayEnd, \
		JsonIterateValueTypeArrayPush, \
		JsonIterateValueTypeNumber, \
		JsonIterateValueTypeString, \
		JsonIterateValueTypeBoolean, \
		JsonIterateValueTypeNull \
	} JsonIterateValueType;

#define GENERATE_VARIANT_CORE_JSON_ITERATE_VALUE() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	typedef struct { \
		JsonIterateObjectEntry entry; \
	} _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_; \
\
	typedef struct { \
		JsonIterateArrayEntry entry; \
	} _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_; \
\
	/* raw union for variant */ \
	typedef union { \
		_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_ \
		    object_entry; \
		_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_ \
		    array_push; \
		JsonNumber number; \
		JsonString* string; \
		JsonBoolean boolean; \
	} _AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		JsonIterateValueType _variant_tag_for_json_iterate_value_tag_member; \
		_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE \
		    _variant_data_for_json_iterate_value_data_member; \
	} JsonIterateValue; \
	typedef struct { \
		JsonIterateValueType _variant_tag_for_json_iterate_value_tag_member; \
		_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE \
		    _variant_data_for_json_iterate_value_data_member; \
	} AssertTypeImplForJsonIterateValueImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE \
		    _variant_data_for_json_iterate_value_data_member; \
		JsonIterateValueType _variant_tag_for_json_iterate_value_tag_member; \
	} AssertTypeImplForJsonIterateValueImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForJsonIterateValueImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForJsonIterateValueImpl_2_DONT_USE_), \
	              "Size for variant JsonIterateValue not smaller as the inverted order, current " \
	              "order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_json_iterate_value(JsonIterateValueType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case JsonIterateValueTypeObjectStart: { \
			return TSTR_STATIC_LIT("object_start"); \
		} \
		case JsonIterateValueTypeObjectEnd: { \
			return TSTR_STATIC_LIT("object_end"); \
		} \
		case JsonIterateValueTypeObjectEntry: { \
			return TSTR_STATIC_LIT("object_entry"); \
		} \
		case JsonIterateValueTypeArrayStart: { \
			return TSTR_STATIC_LIT("array_start"); \
		} \
		case JsonIterateValueTypeArrayEnd: { \
			return TSTR_STATIC_LIT("array_end"); \
		} \
		case JsonIterateValueTypeArrayPush: { \
			return TSTR_STATIC_LIT("array_push"); \
		} \
		case JsonIterateValueTypeNumber: { \
			return TSTR_STATIC_LIT("number"); \
		} \
		case JsonIterateValueTypeString: { \
			return TSTR_STATIC_LIT("string"); \
		} \
		case JsonIterateValueTypeBoolean: { \
			return TSTR_STATIC_LIT("boolean"); \
		} \
		case JsonIterateValueTypeNull: { \
			return TSTR_STATIC_LIT("null"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValueType \
	get_current_tag_type_for_json_iterate_value(JsonIterateValue const variant_entry) { \
		return variant_entry._variant_tag_for_json_iterate_value_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_object_start(void) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeObjectStart, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){} \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_object_end(void) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeObjectEnd, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){} \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_object_entry(JsonIterateObjectEntry const entry) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeObjectEntry, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){ \
			        .object_entry = \
			            (_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_){ \
			                .entry = entry } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_ \
	    json_iterate_value_get_as_object_entry(JsonIterateValue const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeObjectEntry); \
		return variant_entry._variant_data_for_json_iterate_value_data_member.object_entry; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_* \
	    json_iterate_value_get_as_object_entry_mut_ref(JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeObjectEntry); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.object_entry); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_* \
	    json_iterate_value_get_as_object_entry_const_ref( \
	        const JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeObjectEntry); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.object_entry); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_array_start(void) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeArrayStart, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){} \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_array_end(void) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeArrayEnd, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){} \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_array_push(JsonIterateArrayEntry const entry) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeArrayPush, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){ \
			        .array_push = \
			            (_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_){ \
			                .entry = entry } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_ \
	    json_iterate_value_get_as_array_push(JsonIterateValue const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeArrayPush); \
		return variant_entry._variant_data_for_json_iterate_value_data_member.array_push; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_* \
	    json_iterate_value_get_as_array_push_mut_ref(JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeArrayPush); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.array_push); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_* \
	    json_iterate_value_get_as_array_push_const_ref( \
	        const JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeArrayPush); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.array_push); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_number(JsonNumber const value) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeNumber, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){ .number = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonNumber \
	json_iterate_value_get_as_number(JsonIterateValue const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeNumber); \
		return variant_entry._variant_data_for_json_iterate_value_data_member.number; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonNumber* \
	json_iterate_value_get_as_number_mut_ref(JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeNumber); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.number); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonNumber const* \
	json_iterate_value_get_as_number_const_ref(const JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeNumber); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.number); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_string(JsonString* const value) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeString, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){ .string = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonString* \
	json_iterate_value_get_as_string(JsonIterateValue const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeString); \
		return variant_entry._variant_data_for_json_iterate_value_data_member.string; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonString** \
	json_iterate_value_get_as_string_mut_ref(JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeString); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.string); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonString* const* \
	json_iterate_value_get_as_string_const_ref(const JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeString); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.string); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_boolean(JsonBoolean const value) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeBoolean, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){ .boolean = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonBoolean \
	json_iterate_value_get_as_boolean(JsonIterateValue const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeBoolean); \
		return variant_entry._variant_data_for_json_iterate_value_data_member.boolean; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonBoolean* \
	json_iterate_value_get_as_boolean_mut_ref(JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeBoolean); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.boolean); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonBoolean const* \
	json_iterate_value_get_as_boolean_const_ref(const JsonIterateValue* const variant_entry) { \
		VARIANT_JSON_ITERATE_VALUE_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_value_tag_member, \
		    JsonIterateValueTypeBoolean); \
		return &(variant_entry->_variant_data_for_json_iterate_value_data_member.boolean); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateValue \
	new_json_iterate_value_null(void) { \
		return (JsonIterateValue){ \
			._variant_tag_for_json_iterate_value_tag_member = JsonIterateValueTypeNull, \
			._variant_data_for_json_iterate_value_data_member = \
			    (_AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE){} \
		}; \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_json_iterate_value") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_JSON_ITERATE_VALUE() \
	GENERATE_VARIANT_ENUM_JSON_ITERATE_VALUE() \
	GENERATE_VARIANT_CORE_JSON_ITERATE_VALUE()

#define IF_JSON_ITERATE_VALUE_IS_OBJECT_START(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeObjectStart)

#define IF_JSON_ITERATE_VALUE_IS_OBJECT_END(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeObjectEnd)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_2( \
	    variant_entry, object_entry)

#define IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeObjectEntry) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_ const \
			        var_name = \
			            ((variant_entry) \
			                 ._variant_data_for_json_iterate_value_data_member.object_entry); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_2( \
	    variant_entry, object_entry)

#define IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeObjectEntry) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_ \
			        var_name = \
			            ((variant_entry) \
			                 ._variant_data_for_json_iterate_value_data_member.object_entry); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeObjectEntry)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_2( \
	    variant_entry, object_entry)

#define IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeObjectEntry) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_iterate_value_data_member.object_entry); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_2( \
	    variant_entry, object_entry)

#define IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeObjectEntry) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_iterate_value_data_member.object_entry); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define IF_JSON_ITERATE_VALUE_IS_ARRAY_START(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeArrayStart)

#define IF_JSON_ITERATE_VALUE_IS_ARRAY_END(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeArrayEnd)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_2( \
	    variant_entry, array_push)

#define IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeArrayPush) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_ const \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_iterate_value_data_member.array_push); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_2(variant_entry, \
	                                                                                array_push)

#define IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeArrayPush) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_ \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_iterate_value_data_member.array_push); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeArrayPush)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_2( \
	    variant_entry, array_push)

#define IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeArrayPush) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_iterate_value_data_member.array_push); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_2( \
	    variant_entry, array_push)

#define IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeArrayPush) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_iterate_value_data_member.array_push); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_2(variant_entry, \
	                                                                              number)

#define IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeNumber) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonNumber const var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.number); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_2(variant_entry, \
	                                                                            number)

#define IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_2(variant_entry, \
                                                                                    var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeNumber) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonNumber var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.number); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define IF_JSON_ITERATE_VALUE_IS_NUMBER_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == JsonIterateValueTypeNumber)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_2( \
	    variant_entry, number)

#define IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeNumber) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const JsonNumber* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.number); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_2(variant_entry, \
	                                                                                number)

#define IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeNumber) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonNumber* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.number); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_2(variant_entry, \
	                                                                              string)

#define IF_JSON_ITERATE_VALUE_IS_STRING_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonString* const var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_2(variant_entry, \
	                                                                            string)

#define IF_JSON_ITERATE_VALUE_IS_STRING_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_2(variant_entry, \
                                                                                    var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonString* var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define IF_JSON_ITERATE_VALUE_IS_STRING_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == JsonIterateValueTypeString)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_2( \
	    variant_entry, string)

#define IF_JSON_ITERATE_VALUE_IS_STRING_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const JsonString** const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_2(variant_entry, \
	                                                                                string)

#define IF_JSON_ITERATE_VALUE_IS_STRING_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeString) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonString** const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.string); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_2(variant_entry, \
	                                                                               boolean)

#define IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeBoolean) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonBoolean const var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.boolean); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_2(variant_entry, \
	                                                                             boolean)

#define IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeBoolean) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonBoolean var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.boolean); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define IF_JSON_ITERATE_VALUE_IS_BOOLEAN_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeBoolean)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2( \
	    variant_entry, boolean)

#define IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeBoolean) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const JsonBoolean* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.boolean); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2( \
	    variant_entry, boolean)

#define IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_value_tag_member == \
	   JsonIterateValueTypeBoolean) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonBoolean* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.boolean); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define IF_JSON_ITERATE_VALUE_IS_NULL(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member == JsonIterateValueTypeNull)

#define IF_JSON_ITERATE_VALUE_IS_NOT_OBJECT_START(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != \
	   JsonIterateValueTypeObjectStart)

#define IF_JSON_ITERATE_VALUE_IS_NOT_OBJECT_END(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != \
	   JsonIterateValueTypeObjectEnd)

#define IF_JSON_ITERATE_VALUE_IS_NOT_OBJECT_ENTRY(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != \
	   JsonIterateValueTypeObjectEntry)

#define IF_JSON_ITERATE_VALUE_IS_NOT_ARRAY_START(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != \
	   JsonIterateValueTypeArrayStart)

#define IF_JSON_ITERATE_VALUE_IS_NOT_ARRAY_END(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != \
	   JsonIterateValueTypeArrayEnd)

#define IF_JSON_ITERATE_VALUE_IS_NOT_ARRAY_PUSH(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != \
	   JsonIterateValueTypeArrayPush)

#define IF_JSON_ITERATE_VALUE_IS_NOT_NUMBER(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != JsonIterateValueTypeNumber)

#define IF_JSON_ITERATE_VALUE_IS_NOT_STRING(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != JsonIterateValueTypeString)

#define IF_JSON_ITERATE_VALUE_IS_NOT_BOOLEAN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != \
	   JsonIterateValueTypeBoolean)

#define IF_JSON_ITERATE_VALUE_IS_NOT_NULL(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_value_tag_member != JsonIterateValueTypeNull)

#define SWITCH_JSON_ITERATE_VALUE(variant_entry) \
	switch((variant_entry)._variant_tag_for_json_iterate_value_tag_member)

#define CASE_JSON_ITERATE_VALUE_IS_OBJECT_START() case JsonIterateValueTypeObjectStart:

#define CASE_JSON_ITERATE_VALUE_IS_OBJECT_END() case JsonIterateValueTypeObjectEnd:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_2( \
	    variant_entry, object_entry)

#define CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeObjectEntry: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_ const \
			        var_name = \
			            ((variant_entry) \
			                 ._variant_data_for_json_iterate_value_data_member.object_entry); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_2( \
	    variant_entry, object_entry)

#define CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeObjectEntry: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_ \
			        var_name = \
			            ((variant_entry) \
			                 ._variant_data_for_json_iterate_value_data_member.object_entry); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_IGN() case JsonIterateValueTypeObjectEntry:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_2( \
	    variant_entry, object_entry)

#define CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeObjectEntry: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_iterate_value_data_member.object_entry); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_2( \
	    variant_entry, object_entry)

#define CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_OBJECT_ENTRY_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeObjectEntry: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_iterate_value_data_member.object_entry); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define CASE_JSON_ITERATE_VALUE_IS_ARRAY_START() case JsonIterateValueTypeArrayStart:

#define CASE_JSON_ITERATE_VALUE_IS_ARRAY_END() case JsonIterateValueTypeArrayEnd:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_2( \
	    variant_entry, array_push)

#define CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeArrayPush: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_ const \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_iterate_value_data_member.array_push); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_2( \
	    variant_entry, array_push)

#define CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeArrayPush: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_ \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_iterate_value_data_member.array_push); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_IGN() case JsonIterateValueTypeArrayPush:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_2( \
	    variant_entry, array_push)

#define CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeArrayPush: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_iterate_value_data_member.array_push); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_2( \
	    variant_entry, array_push)

#define CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_ARRAY_PUSH_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeArrayPush: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_iterate_value_data_member.array_push); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_2(variant_entry, \
	                                                                                number)

#define CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeNumber: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonNumber const var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.number); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_2(variant_entry, \
	                                                                              number)

#define CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeNumber: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonNumber var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.number); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define CASE_JSON_ITERATE_VALUE_IS_NUMBER_IGN() case JsonIterateValueTypeNumber:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_2( \
	    variant_entry, number)

#define CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeNumber: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const JsonNumber* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.number); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_2( \
	    variant_entry, number)

#define CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_NUMBER_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeNumber: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonNumber* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.number); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_2(variant_entry, \
	                                                                                string)

#define CASE_JSON_ITERATE_VALUE_IS_STRING_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeString: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonString* const var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_2(variant_entry, \
	                                                                              string)

#define CASE_JSON_ITERATE_VALUE_IS_STRING_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeString: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonString* var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define CASE_JSON_ITERATE_VALUE_IS_STRING_IGN() case JsonIterateValueTypeString:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_2( \
	    variant_entry, string)

#define CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeString: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const JsonString** const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_2( \
	    variant_entry, string)

#define CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_STRING_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeString: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonString** const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.string); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_2( \
	    variant_entry, boolean)

#define CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeBoolean: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonBoolean const var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.boolean); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_2(variant_entry, \
	                                                                               boolean)

#define CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeBoolean: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonBoolean var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_value_data_member.boolean); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_IGN() case JsonIterateValueTypeBoolean:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2( \
	    variant_entry, boolean)

#define CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeBoolean: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(const JsonBoolean* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.boolean); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2( \
	    variant_entry, boolean)

#define CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_VALUE_IS_BOOLEAN_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateValueTypeBoolean: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
		        false) \
			for(JsonBoolean* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_value_data_member.boolean); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl = \
			        false)

#define CASE_JSON_ITERATE_VALUE_IS_NULL() case JsonIterateValueTypeNull:

#pragma GCC poison _variant_tag_for_json_iterate_value_tag_member _variant_data_for_json_iterate_value_data_member _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_value_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_value_variant_impl _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_7_name_object_entry_impl_ _variant_impl_unnamed_struct_for_variant_json_iterate_value_id_8_name_array_push_impl_ _AnonymousUnionForVariantJsonIterateValueImpl_DONT_USE AssertTypeImplForJsonIterateValueImpl_1_DONT_USE_ AssertTypeImplForJsonIterateValueImpl_2_DONT_USE_

#define VARIANT_JSON_ITERATE_RESULT_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, json_iterate_result, "JsonIterateResult")

#define GENERATE_VARIANT_ENUM_JSON_ITERATE_RESULT() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(bool) { \
		JsonIterateResultTypeOk = false, \
		JsonIterateResultTypeError = true \
	} JsonIterateResultType;

#define GENERATE_VARIANT_CORE_JSON_ITERATE_RESULT() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	typedef struct { \
		JsonIterateError error; \
	} _variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_; \
\
	/* raw union for variant */ \
	typedef union { \
		RTTIAnnotatedValue ok; \
		_variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_ error; \
	} _AnonymousUnionForVariantJsonIterateResultImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		JsonIterateResultType _variant_tag_for_json_iterate_result_tag_member; \
		_AnonymousUnionForVariantJsonIterateResultImpl_DONT_USE \
		    _variant_data_for_json_iterate_result_data_member; \
	} JsonIterateResult; \
	typedef struct { \
		JsonIterateResultType _variant_tag_for_json_iterate_result_tag_member; \
		_AnonymousUnionForVariantJsonIterateResultImpl_DONT_USE \
		    _variant_data_for_json_iterate_result_data_member; \
	} AssertTypeImplForJsonIterateResultImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantJsonIterateResultImpl_DONT_USE \
		    _variant_data_for_json_iterate_result_data_member; \
		JsonIterateResultType _variant_tag_for_json_iterate_result_tag_member; \
	} AssertTypeImplForJsonIterateResultImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForJsonIterateResultImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForJsonIterateResultImpl_2_DONT_USE_), \
	              "Size for variant JsonIterateResult not smaller as the inverted order, current " \
	              "order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_json_iterate_result( \
	    JsonIterateResultType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case JsonIterateResultTypeOk: { \
			return TSTR_STATIC_LIT("ok"); \
		} \
		case JsonIterateResultTypeError: { \
			return TSTR_STATIC_LIT("error"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateResultType \
	get_current_tag_type_for_json_iterate_result(JsonIterateResult const variant_entry) { \
		return variant_entry._variant_tag_for_json_iterate_result_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateResult \
	new_json_iterate_result_ok(RTTIAnnotatedValue const value) { \
		return (JsonIterateResult){ \
			._variant_tag_for_json_iterate_result_tag_member = JsonIterateResultTypeOk, \
			._variant_data_for_json_iterate_result_data_member = \
			    (_AnonymousUnionForVariantJsonIterateResultImpl_DONT_USE){ .ok = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline RTTIAnnotatedValue \
	json_iterate_result_get_as_ok(JsonIterateResult const variant_entry) { \
		VARIANT_JSON_ITERATE_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_iterate_result_tag_member, \
		    JsonIterateResultTypeOk); \
		return variant_entry._variant_data_for_json_iterate_result_data_member.ok; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline RTTIAnnotatedValue* \
	json_iterate_result_get_as_ok_mut_ref(JsonIterateResult* const variant_entry) { \
		VARIANT_JSON_ITERATE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_result_tag_member, \
		    JsonIterateResultTypeOk); \
		return &(variant_entry->_variant_data_for_json_iterate_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline RTTIAnnotatedValue const* \
	json_iterate_result_get_as_ok_const_ref(const JsonIterateResult* const variant_entry) { \
		VARIANT_JSON_ITERATE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_result_tag_member, \
		    JsonIterateResultTypeOk); \
		return &(variant_entry->_variant_data_for_json_iterate_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonIterateResult \
	new_json_iterate_result_error(JsonIterateError const error) { \
		return (JsonIterateResult){ \
			._variant_tag_for_json_iterate_result_tag_member = JsonIterateResultTypeError, \
			._variant_data_for_json_iterate_result_data_member = \
			    (_AnonymousUnionForVariantJsonIterateResultImpl_DONT_USE){ \
			        .error = \
			            (_variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_){ \
			                .error = error } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_ \
	    json_iterate_result_get_as_error(JsonIterateResult const variant_entry) { \
		VARIANT_JSON_ITERATE_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_iterate_result_tag_member, \
		    JsonIterateResultTypeError); \
		return variant_entry._variant_data_for_json_iterate_result_data_member.error; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_* \
	    json_iterate_result_get_as_error_mut_ref(JsonIterateResult* const variant_entry) { \
		VARIANT_JSON_ITERATE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_result_tag_member, \
		    JsonIterateResultTypeError); \
		return &(variant_entry->_variant_data_for_json_iterate_result_data_member.error); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_* \
	    json_iterate_result_get_as_error_const_ref(const JsonIterateResult* const variant_entry) { \
		VARIANT_JSON_ITERATE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_iterate_result_tag_member, \
		    JsonIterateResultTypeError); \
		return &(variant_entry->_variant_data_for_json_iterate_result_data_member.error); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_json_iterate_result") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_JSON_ITERATE_RESULT() \
	GENERATE_VARIANT_ENUM_JSON_ITERATE_RESULT() \
	GENERATE_VARIANT_CORE_JSON_ITERATE_RESULT()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define IF_JSON_ITERATE_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_result_tag_member == JsonIterateResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(RTTIAnnotatedValue const var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define IF_JSON_ITERATE_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_result_tag_member == JsonIterateResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(RTTIAnnotatedValue var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define IF_JSON_ITERATE_RESULT_IS_OK_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_result_tag_member == JsonIterateResultTypeOk)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, \
	                                                                               ok)

#define IF_JSON_ITERATE_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_result_tag_member == \
	   JsonIterateResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(const RTTIAnnotatedValue* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, ok)

#define IF_JSON_ITERATE_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_result_tag_member == \
	   JsonIterateResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(RTTIAnnotatedValue* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
	                                                                              error)

#define IF_JSON_ITERATE_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_result_tag_member == \
	   JsonIterateResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_ const \
			        var_name = \
			            ((variant_entry)._variant_data_for_json_iterate_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
	                                                                            error)

#define IF_JSON_ITERATE_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
                                                                                    var_name) \
	if((variant_entry)._variant_tag_for_json_iterate_result_tag_member == \
	   JsonIterateResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_ \
			        var_name = \
			            ((variant_entry)._variant_data_for_json_iterate_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define IF_JSON_ITERATE_RESULT_IS_ERROR_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_result_tag_member == \
	   JsonIterateResultTypeError)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_result_tag_member == \
	   JsonIterateResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_* const \
			        var_name = &( \
			            (variant_entry)->_variant_data_for_json_iterate_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_2(variant_entry, \
	                                                                                error)

#define IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_iterate_result_tag_member == \
	   JsonIterateResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_* const \
			        var_name = &( \
			            (variant_entry)->_variant_data_for_json_iterate_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define IF_JSON_ITERATE_RESULT_IS_NOT_OK(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_result_tag_member != JsonIterateResultTypeOk)

#define IF_JSON_ITERATE_RESULT_IS_NOT_ERROR(variant_entry) \
	if((variant_entry)._variant_tag_for_json_iterate_result_tag_member != \
	   JsonIterateResultTypeError)

#define SWITCH_JSON_ITERATE_RESULT(variant_entry) \
	_Pragma("GCC diagnostic push") \
	    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ switch( \
	        (variant_entry)._variant_tag_for_json_iterate_result_tag_member) \
	        _Pragma("GCC diagnostic pop")

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define CASE_JSON_ITERATE_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(RTTIAnnotatedValue const var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define CASE_JSON_ITERATE_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case JsonIterateResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(RTTIAnnotatedValue var_name = \
			        ((variant_entry)._variant_data_for_json_iterate_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define CASE_JSON_ITERATE_RESULT_IS_OK_IGN() case JsonIterateResultTypeOk:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_2( \
	    variant_entry, ok)

#define CASE_JSON_ITERATE_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(const RTTIAnnotatedValue* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, \
	                                                                               ok)

#define CASE_JSON_ITERATE_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_OK_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(RTTIAnnotatedValue* const var_name = \
			        &((variant_entry)->_variant_data_for_json_iterate_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
	                                                                                error)

#define CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_ const \
			        var_name = \
			            ((variant_entry)._variant_data_for_json_iterate_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
	                                                                              error)

#define CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_ \
			        var_name = \
			            ((variant_entry)._variant_data_for_json_iterate_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define CASE_JSON_ITERATE_RESULT_IS_ERROR_IGN() case JsonIterateResultTypeError:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_* const \
			        var_name = &( \
			            (variant_entry)->_variant_data_for_json_iterate_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_ITERATE_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonIterateResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_* const \
			        var_name = &( \
			            (variant_entry)->_variant_data_for_json_iterate_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl = \
			        false)

#pragma GCC poison _variant_tag_for_json_iterate_result_tag_member _variant_data_for_json_iterate_result_data_member _for_macro_trick_for_if_expr_impl_once_variant_json_iterate_result_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_json_iterate_result_variant_impl _variant_impl_unnamed_struct_for_variant_json_iterate_result_id_9_name_error_impl_ _AnonymousUnionForVariantJsonIterateResultImpl_DONT_USE AssertTypeImplForJsonIterateResultImpl_1_DONT_USE_ AssertTypeImplForJsonIterateResultImpl_2_DONT_USE_

#define VARIANT_JSON_PATH_SEGMENT_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, json_path_segment, "JsonPathSegment")

#define GENERATE_VARIANT_ENUM_JSON_PATH_SEGMENT() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(uint8_t) { \
		JsonPathSegmentTypeObjectKey = 0, \
		JsonPathSegmentTypeArrayIndex \
	} JsonPathSegmentType;

#define GENERATE_VARIANT_CORE_JSON_PATH_SEGMENT() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	typedef struct { \
		tstr name; \
	} _variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_; \
\
	typedef struct { \
		size_t index; \
	} _variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_; \
\
	/* raw union for variant */ \
	typedef union { \
		_variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_ \
		    object_key; \
		_variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_ \
		    array_index; \
	} _AnonymousUnionForVariantJsonPathSegmentImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		JsonPathSegmentType _variant_tag_for_json_path_segment_tag_member; \
		_AnonymousUnionForVariantJsonPathSegmentImpl_DONT_USE \
		    _variant_data_for_json_path_segment_data_member; \
	} JsonPathSegment; \
	typedef struct { \
		JsonPathSegmentType _variant_tag_for_json_path_segment_tag_member; \
		_AnonymousUnionForVariantJsonPathSegmentImpl_DONT_USE \
		    _variant_data_for_json_path_segment_data_member; \
	} AssertTypeImplForJsonPathSegmentImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantJsonPathSegmentImpl_DONT_USE \
		    _variant_data_for_json_path_segment_data_member; \
		JsonPathSegmentType _variant_tag_for_json_path_segment_tag_member; \
	} AssertTypeImplForJsonPathSegmentImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForJsonPathSegmentImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForJsonPathSegmentImpl_2_DONT_USE_), \
	              "Size for variant JsonPathSegment not smaller as the inverted order, current " \
	              "order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_json_path_segment(JsonPathSegmentType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case JsonPathSegmentTypeObjectKey: { \
			return TSTR_STATIC_LIT("object_key"); \
		} \
		case JsonPathSegmentTypeArrayIndex: { \
			return TSTR_STATIC_LIT("array_index"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonPathSegmentType \
	get_current_tag_type_for_json_path_segment(JsonPathSegment const variant_entry) { \
		return variant_entry._variant_tag_for_json_path_segment_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonPathSegment \
	new_json_path_segment_object_key(tstr const name) { \
		return (JsonPathSegment){ \
			._variant_tag_for_json_path_segment_tag_member = JsonPathSegmentTypeObjectKey, \
			._variant_data_for_json_path_segment_data_member = \
			    (_AnonymousUnionForVariantJsonPathSegmentImpl_DONT_USE){ \
			        .object_key = \
			            (_variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_){ \
			                .name = name } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_ \
	    json_path_segment_get_as_object_key(JsonPathSegment const variant_entry) { \
		VARIANT_JSON_PATH_SEGMENT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_path_segment_tag_member, \
		    JsonPathSegmentTypeObjectKey); \
		return variant_entry._variant_data_for_json_path_segment_data_member.object_key; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_* \
	    json_path_segment_get_as_object_key_mut_ref(JsonPathSegment* const variant_entry) { \
		VARIANT_JSON_PATH_SEGMENT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_path_segment_tag_member, \
		    JsonPathSegmentTypeObjectKey); \
		return &(variant_entry->_variant_data_for_json_path_segment_data_member.object_key); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_* \
	    json_path_segment_get_as_object_key_const_ref( \
	        const JsonPathSegment* const variant_entry) { \
		VARIANT_JSON_PATH_SEGMENT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_path_segment_tag_member, \
		    JsonPathSegmentTypeObjectKey); \
		return &(variant_entry->_variant_data_for_json_path_segment_data_member.object_key); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonPathSegment \
	new_json_path_segment_array_index(size_t const index) { \
		return (JsonPathSegment){ \
			._variant_tag_for_json_path_segment_tag_member = JsonPathSegmentTypeArrayIndex, \
			._variant_data_for_json_path_segment_data_member = \
			    (_AnonymousUnionForVariantJsonPathSegmentImpl_DONT_USE){ \
			        .array_index = \
			            (_variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_){ \
			                .index = index } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_ \
	    json_path_segment_get_as_array_index(JsonPathSegment const variant_entry) { \
		VARIANT_JSON_PATH_SEGMENT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_path_segment_tag_member, \
		    JsonPathSegmentTypeArrayIndex); \
		return variant_entry._variant_data_for_json_path_segment_data_member.array_index; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_* \
	    json_path_segment_get_as_array_index_mut_ref(JsonPathSegment* const variant_entry) { \
		VARIANT_JSON_PATH_SEGMENT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_path_segment_tag_member, \
		    JsonPathSegmentTypeArrayIndex); \
		return &(variant_entry->_variant_data_for_json_path_segment_data_member.array_index); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_* \
	    json_path_segment_get_as_array_index_const_ref( \
	        const JsonPathSegment* const variant_entry) { \
		VARIANT_JSON_PATH_SEGMENT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_path_segment_tag_member, \
		    JsonPathSegmentTypeArrayIndex); \
		return &(variant_entry->_variant_data_for_json_path_segment_data_member.array_index); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_json_path_segment") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_JSON_PATH_SEGMENT() \
	GENERATE_VARIANT_ENUM_JSON_PATH_SEGMENT() \
	GENERATE_VARIANT_CORE_JSON_PATH_SEGMENT()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_2( \
	    variant_entry, object_key)

#define IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeObjectKey) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_ const \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_path_segment_data_member.object_key); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_2(variant_entry, \
	                                                                               object_key)

#define IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeObjectKey) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_ \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_path_segment_data_member.object_key); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeObjectKey)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_2( \
	    variant_entry, object_key)

#define IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeObjectKey) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_path_segment_data_member.object_key); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_2( \
	    variant_entry, object_key)

#define IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeObjectKey) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_path_segment_data_member.object_key); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_2( \
	    variant_entry, array_index)

#define IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeArrayIndex) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_ const \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_path_segment_data_member.array_index); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_2(variant_entry, \
	                                                                                array_index)

#define IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeArrayIndex) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_ \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_path_segment_data_member.array_index); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeArrayIndex)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_2( \
	    variant_entry, array_index)

#define IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeArrayIndex) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_path_segment_data_member.array_index); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_2( \
	    variant_entry, array_index)

#define IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_path_segment_tag_member == \
	   JsonPathSegmentTypeArrayIndex) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_path_segment_data_member.array_index); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define IF_JSON_PATH_SEGMENT_IS_NOT_OBJECT_KEY(variant_entry) \
	if((variant_entry)._variant_tag_for_json_path_segment_tag_member != \
	   JsonPathSegmentTypeObjectKey)

#define IF_JSON_PATH_SEGMENT_IS_NOT_ARRAY_INDEX(variant_entry) \
	if((variant_entry)._variant_tag_for_json_path_segment_tag_member != \
	   JsonPathSegmentTypeArrayIndex)

#define SWITCH_JSON_PATH_SEGMENT(variant_entry) \
	switch((variant_entry)._variant_tag_for_json_path_segment_tag_member)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_2( \
	    variant_entry, object_key)

#define CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonPathSegmentTypeObjectKey: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_ const \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_path_segment_data_member.object_key); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_2( \
	    variant_entry, object_key)

#define CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonPathSegmentTypeObjectKey: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_ \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_path_segment_data_member.object_key); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_IGN() case JsonPathSegmentTypeObjectKey:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_2( \
	    variant_entry, object_key)

#define CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonPathSegmentTypeObjectKey: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_path_segment_data_member.object_key); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_2( \
	    variant_entry, object_key)

#define CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_OBJECT_KEY_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonPathSegmentTypeObjectKey: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_path_segment_data_member.object_key); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_2( \
	    variant_entry, array_index)

#define CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonPathSegmentTypeArrayIndex: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_ const \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_path_segment_data_member.array_index); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_2( \
	    variant_entry, array_index)

#define CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonPathSegmentTypeArrayIndex: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_ \
			        var_name = ((variant_entry) \
			                        ._variant_data_for_json_path_segment_data_member.array_index); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_IGN() case JsonPathSegmentTypeArrayIndex:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_2( \
	    variant_entry, array_index)

#define CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonPathSegmentTypeArrayIndex: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_path_segment_data_member.array_index); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_2( \
	    variant_entry, array_index)

#define CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_PATH_SEGMENT_IS_ARRAY_INDEX_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonPathSegmentTypeArrayIndex: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_json_path_segment_data_member.array_index); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl = \
			        false)

#pragma GCC poison _variant_tag_for_json_path_segment_tag_member _variant_data_for_json_path_segment_data_member _for_macro_trick_for_if_expr_impl_once_variant_json_path_segment_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_json_path_segment_variant_impl _variant_impl_unnamed_struct_for_variant_json_path_segment_id_10_name_object_key_impl_ _variant_impl_unnamed_struct_for_variant_json_path_segment_id_11_name_array_index_impl_ _AnonymousUnionForVariantJsonPathSegmentImpl_DONT_USE AssertTypeImplForJsonPathSegmentImpl_1_DONT_USE_ AssertTypeImplForJsonPathSegmentImpl_2_DONT_USE_

#define VARIANT_JSON_SCHEMA_ADD_RESULT_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, json_schema_add_result, "JsonSchemaAddResult")

#define GENERATE_VARIANT_ENUM_JSON_SCHEMA_ADD_RESULT() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(bool) { \
		JsonSchemaAddResultTypeOk = false, \
		JsonSchemaAddResultTypeError = true \
	} JsonSchemaAddResultType;

#define GENERATE_VARIANT_CORE_JSON_SCHEMA_ADD_RESULT() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	/* raw union for variant */ \
	typedef union { \
		JsonDefId ok; \
		tstr_static error; \
	} _AnonymousUnionForVariantJsonSchemaAddResultImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		JsonSchemaAddResultType _variant_tag_for_json_schema_add_result_tag_member; \
		_AnonymousUnionForVariantJsonSchemaAddResultImpl_DONT_USE \
		    _variant_data_for_json_schema_add_result_data_member; \
	} JsonSchemaAddResult; \
	typedef struct { \
		JsonSchemaAddResultType _variant_tag_for_json_schema_add_result_tag_member; \
		_AnonymousUnionForVariantJsonSchemaAddResultImpl_DONT_USE \
		    _variant_data_for_json_schema_add_result_data_member; \
	} AssertTypeImplForJsonSchemaAddResultImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantJsonSchemaAddResultImpl_DONT_USE \
		    _variant_data_for_json_schema_add_result_data_member; \
		JsonSchemaAddResultType _variant_tag_for_json_schema_add_result_tag_member; \
	} AssertTypeImplForJsonSchemaAddResultImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForJsonSchemaAddResultImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForJsonSchemaAddResultImpl_2_DONT_USE_), \
	              "Size for variant JsonSchemaAddResult not smaller as the inverted order, " \
	              "current order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_json_schema_add_result( \
	    JsonSchemaAddResultType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case JsonSchemaAddResultTypeOk: { \
			return TSTR_STATIC_LIT("ok"); \
		} \
		case JsonSchemaAddResultTypeError: { \
			return TSTR_STATIC_LIT("error"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchemaAddResultType \
	get_current_tag_type_for_json_schema_add_result(JsonSchemaAddResult const variant_entry) { \
		return variant_entry._variant_tag_for_json_schema_add_result_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchemaAddResult \
	new_json_schema_add_result_ok(JsonDefId const value) { \
		return (JsonSchemaAddResult){ \
			._variant_tag_for_json_schema_add_result_tag_member = JsonSchemaAddResultTypeOk, \
			._variant_data_for_json_schema_add_result_data_member = \
			    (_AnonymousUnionForVariantJsonSchemaAddResultImpl_DONT_USE){ .ok = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonDefId \
	json_schema_add_result_get_as_ok(JsonSchemaAddResult const variant_entry) { \
		VARIANT_JSON_SCHEMA_ADD_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_schema_add_result_tag_member, \
		    JsonSchemaAddResultTypeOk); \
		return variant_entry._variant_data_for_json_schema_add_result_data_member.ok; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonDefId* \
	json_schema_add_result_get_as_ok_mut_ref(JsonSchemaAddResult* const variant_entry) { \
		VARIANT_JSON_SCHEMA_ADD_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_schema_add_result_tag_member, \
		    JsonSchemaAddResultTypeOk); \
		return &(variant_entry->_variant_data_for_json_schema_add_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonDefId const* \
	json_schema_add_result_get_as_ok_const_ref(const JsonSchemaAddResult* const variant_entry) { \
		VARIANT_JSON_SCHEMA_ADD_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_schema_add_result_tag_member, \
		    JsonSchemaAddResultTypeOk); \
		return &(variant_entry->_variant_data_for_json_schema_add_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonSchemaAddResult \
	new_json_schema_add_result_error(tstr_static const value) { \
		return (JsonSchemaAddResult){ \
			._variant_tag_for_json_schema_add_result_tag_member = JsonSchemaAddResultTypeError, \
			._variant_data_for_json_schema_add_result_data_member = \
			    (_AnonymousUnionForVariantJsonSchemaAddResultImpl_DONT_USE){ .error = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	json_schema_add_result_get_as_error(JsonSchemaAddResult const variant_entry) { \
		VARIANT_JSON_SCHEMA_ADD_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_json_schema_add_result_tag_member, \
		    JsonSchemaAddResultTypeError); \
		return variant_entry._variant_data_for_json_schema_add_result_data_member.error; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static* \
	json_schema_add_result_get_as_error_mut_ref(JsonSchemaAddResult* const variant_entry) { \
		VARIANT_JSON_SCHEMA_ADD_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_schema_add_result_tag_member, \
		    JsonSchemaAddResultTypeError); \
		return &(variant_entry->_variant_data_for_json_schema_add_result_data_member.error); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static const* \
	json_schema_add_result_get_as_error_const_ref( \
	    const JsonSchemaAddResult* const variant_entry) { \
		VARIANT_JSON_SCHEMA_ADD_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_json_schema_add_result_tag_member, \
		    JsonSchemaAddResultTypeError); \
		return &(variant_entry->_variant_data_for_json_schema_add_result_data_member.error); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_json_schema_add_result") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_JSON_SCHEMA_ADD_RESULT() \
	GENERATE_VARIANT_ENUM_JSON_SCHEMA_ADD_RESULT() \
	GENERATE_VARIANT_CORE_JSON_SCHEMA_ADD_RESULT()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeOk) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(JsonDefId const var_name = \
			        ((variant_entry)._variant_data_for_json_schema_add_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                                    var_name) \
	if((variant_entry)._variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeOk) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(JsonDefId var_name = \
			        ((variant_entry)._variant_data_for_json_schema_add_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_OK_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeOk)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_2( \
	    variant_entry, ok)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeOk) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(const JsonDefId* const var_name = \
			        &((variant_entry)->_variant_data_for_json_schema_add_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, \
	                                                                                ok)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeOk) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(JsonDefId* const var_name = \
			        &((variant_entry)->_variant_data_for_json_schema_add_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_2( \
	    variant_entry, error)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeError) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(tstr_static const var_name = \
			        ((variant_entry)._variant_data_for_json_schema_add_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
	                                                                               error)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeError) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(tstr_static var_name = \
			        ((variant_entry)._variant_data_for_json_schema_add_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeError)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeError) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(const tstr_static* const var_name = &( \
			        (variant_entry)->_variant_data_for_json_schema_add_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
	    variant_entry, error)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_json_schema_add_result_tag_member == \
	   JsonSchemaAddResultTypeError) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(tstr_static* const var_name = &( \
			        (variant_entry)->_variant_data_for_json_schema_add_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_NOT_OK(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_add_result_tag_member != \
	   JsonSchemaAddResultTypeOk)

#define IF_JSON_SCHEMA_ADD_RESULT_IS_NOT_ERROR(variant_entry) \
	if((variant_entry)._variant_tag_for_json_schema_add_result_tag_member != \
	   JsonSchemaAddResultTypeError)

#define SWITCH_JSON_SCHEMA_ADD_RESULT(variant_entry) \
	_Pragma("GCC diagnostic push") \
	    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ switch( \
	        (variant_entry)._variant_tag_for_json_schema_add_result_tag_member) \
	        _Pragma("GCC diagnostic pop")

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
	                                                                                ok)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaAddResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(JsonDefId const var_name = \
			        ((variant_entry)._variant_data_for_json_schema_add_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaAddResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(JsonDefId var_name = \
			        ((variant_entry)._variant_data_for_json_schema_add_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_IGN() case JsonSchemaAddResultTypeOk:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_2( \
	    variant_entry, ok)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaAddResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(const JsonDefId* const var_name = \
			        &((variant_entry)->_variant_data_for_json_schema_add_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_2( \
	    variant_entry, ok)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_OK_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaAddResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(JsonDefId* const var_name = \
			        &((variant_entry)->_variant_data_for_json_schema_add_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_2( \
	    variant_entry, error)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaAddResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(tstr_static const var_name = \
			        ((variant_entry)._variant_data_for_json_schema_add_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_2( \
	    variant_entry, error)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaAddResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(tstr_static var_name = \
			        ((variant_entry)._variant_data_for_json_schema_add_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_IGN() case JsonSchemaAddResultTypeError:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaAddResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(const tstr_static* const var_name = &( \
			        (variant_entry)->_variant_data_for_json_schema_add_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_JSON_SCHEMA_ADD_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case JsonSchemaAddResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
		        false) \
			for(tstr_static* const var_name = &( \
			        (variant_entry)->_variant_data_for_json_schema_add_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl = \
			        false)

#pragma GCC poison _variant_tag_for_json_schema_add_result_tag_member _variant_data_for_json_schema_add_result_data_member _for_macro_trick_for_if_expr_impl_once_variant_json_schema_add_result_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_json_schema_add_result_variant_impl _AnonymousUnionForVariantJsonSchemaAddResultImpl_DONT_USE AssertTypeImplForJsonSchemaAddResultImpl_1_DONT_USE_ AssertTypeImplForJsonSchemaAddResultImpl_2_DONT_USE_

#define VARIANT_UTF8_NEXT_CHAR_RESULT_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, utf8_next_char_result, "Utf8NextCharResult")

#define GENERATE_VARIANT_ENUM_UTF8_NEXT_CHAR_RESULT() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(bool) { \
		Utf8NextCharResultTypeOk = false, \
		Utf8NextCharResultTypeError = true \
	} Utf8NextCharResultType;

#define GENERATE_VARIANT_CORE_UTF8_NEXT_CHAR_RESULT() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	/* raw union for variant */ \
	typedef union { \
		Utf8Codepoint ok; \
		JsonError error; \
	} _AnonymousUnionForVariantUtf8NextCharResultImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		Utf8NextCharResultType _variant_tag_for_utf8_next_char_result_tag_member; \
		_AnonymousUnionForVariantUtf8NextCharResultImpl_DONT_USE \
		    _variant_data_for_utf8_next_char_result_data_member; \
	} Utf8NextCharResult; \
	typedef struct { \
		Utf8NextCharResultType _variant_tag_for_utf8_next_char_result_tag_member; \
		_AnonymousUnionForVariantUtf8NextCharResultImpl_DONT_USE \
		    _variant_data_for_utf8_next_char_result_data_member; \
	} AssertTypeImplForUtf8NextCharResultImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantUtf8NextCharResultImpl_DONT_USE \
		    _variant_data_for_utf8_next_char_result_data_member; \
		Utf8NextCharResultType _variant_tag_for_utf8_next_char_result_tag_member; \
	} AssertTypeImplForUtf8NextCharResultImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForUtf8NextCharResultImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForUtf8NextCharResultImpl_2_DONT_USE_), \
	              "Size for variant Utf8NextCharResult not smaller as the inverted order, " \
	              "current order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_utf8_next_char_result( \
	    Utf8NextCharResultType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case Utf8NextCharResultTypeOk: { \
			return TSTR_STATIC_LIT("ok"); \
		} \
		case Utf8NextCharResultTypeError: { \
			return TSTR_STATIC_LIT("error"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8NextCharResultType \
	get_current_tag_type_for_utf8_next_char_result(Utf8NextCharResult const variant_entry) { \
		return variant_entry._variant_tag_for_utf8_next_char_result_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8NextCharResult \
	new_utf8_next_char_result_ok(Utf8Codepoint const value) { \
		return (Utf8NextCharResult){ \
			._variant_tag_for_utf8_next_char_result_tag_member = Utf8NextCharResultTypeOk, \
			._variant_data_for_utf8_next_char_result_data_member = \
			    (_AnonymousUnionForVariantUtf8NextCharResultImpl_DONT_USE){ .ok = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8Codepoint \
	utf8_next_char_result_get_as_ok(Utf8NextCharResult const variant_entry) { \
		VARIANT_UTF8_NEXT_CHAR_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_utf8_next_char_result_tag_member, \
		    Utf8NextCharResultTypeOk); \
		return variant_entry._variant_data_for_utf8_next_char_result_data_member.ok; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8Codepoint* \
	utf8_next_char_result_get_as_ok_mut_ref(Utf8NextCharResult* const variant_entry) { \
		VARIANT_UTF8_NEXT_CHAR_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_utf8_next_char_result_tag_member, \
		    Utf8NextCharResultTypeOk); \
		return &(variant_entry->_variant_data_for_utf8_next_char_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8Codepoint const* \
	utf8_next_char_result_get_as_ok_const_ref(const Utf8NextCharResult* const variant_entry) { \
		VARIANT_UTF8_NEXT_CHAR_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_utf8_next_char_result_tag_member, \
		    Utf8NextCharResultTypeOk); \
		return &(variant_entry->_variant_data_for_utf8_next_char_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8NextCharResult \
	new_utf8_next_char_result_error(JsonError const value) { \
		return (Utf8NextCharResult){ \
			._variant_tag_for_utf8_next_char_result_tag_member = Utf8NextCharResultTypeError, \
			._variant_data_for_utf8_next_char_result_data_member = \
			    (_AnonymousUnionForVariantUtf8NextCharResultImpl_DONT_USE){ .error = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonError \
	utf8_next_char_result_get_as_error(Utf8NextCharResult const variant_entry) { \
		VARIANT_UTF8_NEXT_CHAR_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_utf8_next_char_result_tag_member, \
		    Utf8NextCharResultTypeError); \
		return variant_entry._variant_data_for_utf8_next_char_result_data_member.error; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonError* \
	utf8_next_char_result_get_as_error_mut_ref(Utf8NextCharResult* const variant_entry) { \
		VARIANT_UTF8_NEXT_CHAR_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_utf8_next_char_result_tag_member, \
		    Utf8NextCharResultTypeError); \
		return &(variant_entry->_variant_data_for_utf8_next_char_result_data_member.error); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline JsonError const* \
	utf8_next_char_result_get_as_error_const_ref(const Utf8NextCharResult* const variant_entry) { \
		VARIANT_UTF8_NEXT_CHAR_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_utf8_next_char_result_tag_member, \
		    Utf8NextCharResultTypeError); \
		return &(variant_entry->_variant_data_for_utf8_next_char_result_data_member.error); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_utf8_next_char_result") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_UTF8_NEXT_CHAR_RESULT() \
	GENERATE_VARIANT_ENUM_UTF8_NEXT_CHAR_RESULT() \
	GENERATE_VARIANT_CORE_UTF8_NEXT_CHAR_RESULT()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeOk) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(Utf8Codepoint const var_name = \
			        ((variant_entry)._variant_data_for_utf8_next_char_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)._variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeOk) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(Utf8Codepoint var_name = \
			        ((variant_entry)._variant_data_for_utf8_next_char_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_OK_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeOk)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_2( \
	    variant_entry, ok)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeOk) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(const Utf8Codepoint* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_next_char_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, \
	                                                                               ok)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeOk) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(Utf8Codepoint* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_next_char_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
	                                                                                error)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeError) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(JsonError const var_name = \
			        ((variant_entry)._variant_data_for_utf8_next_char_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
	                                                                              error)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeError) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(JsonError var_name = \
			        ((variant_entry)._variant_data_for_utf8_next_char_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeError)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeError) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(const JsonError* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_next_char_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
	    variant_entry, error)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_utf8_next_char_result_tag_member == \
	   Utf8NextCharResultTypeError) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(JsonError* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_next_char_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_NOT_OK(variant_entry) \
	if((variant_entry)._variant_tag_for_utf8_next_char_result_tag_member != \
	   Utf8NextCharResultTypeOk)

#define IF_UTF8_NEXT_CHAR_RESULT_IS_NOT_ERROR(variant_entry) \
	if((variant_entry)._variant_tag_for_utf8_next_char_result_tag_member != \
	   Utf8NextCharResultTypeError)

#define SWITCH_UTF8_NEXT_CHAR_RESULT(variant_entry) \
	_Pragma("GCC diagnostic push") \
	    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ switch( \
	        (variant_entry)._variant_tag_for_utf8_next_char_result_tag_member) \
	        _Pragma("GCC diagnostic pop")

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
	                                                                               ok)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_IMPL_2( \
    variant_entry, var_name) \
	case Utf8NextCharResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(Utf8Codepoint const var_name = \
			        ((variant_entry)._variant_data_for_utf8_next_char_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_IMPL_2( \
    variant_entry, var_name) \
	case Utf8NextCharResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(Utf8Codepoint var_name = \
			        ((variant_entry)._variant_data_for_utf8_next_char_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_IGN() case Utf8NextCharResultTypeOk:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_2( \
	    variant_entry, ok)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case Utf8NextCharResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(const Utf8Codepoint* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_next_char_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_2( \
	    variant_entry, ok)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_OK_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case Utf8NextCharResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(Utf8Codepoint* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_next_char_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_2( \
	    variant_entry, error)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	case Utf8NextCharResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(JsonError const var_name = \
			        ((variant_entry)._variant_data_for_utf8_next_char_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
	                                                                                error)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_IMPL_2( \
    variant_entry, var_name) \
	case Utf8NextCharResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(JsonError var_name = \
			        ((variant_entry)._variant_data_for_utf8_next_char_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_IGN() case Utf8NextCharResultTypeError:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case Utf8NextCharResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(const JsonError* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_next_char_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_NEXT_CHAR_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case Utf8NextCharResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
		        false) \
			for(JsonError* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_next_char_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl = \
			        false)

#pragma GCC poison _variant_tag_for_utf8_next_char_result_tag_member _variant_data_for_utf8_next_char_result_data_member _for_macro_trick_for_if_expr_impl_once_variant_utf8_next_char_result_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_utf8_next_char_result_variant_impl _AnonymousUnionForVariantUtf8NextCharResultImpl_DONT_USE AssertTypeImplForUtf8NextCharResultImpl_1_DONT_USE_ AssertTypeImplForUtf8NextCharResultImpl_2_DONT_USE_

#define VARIANT_READ_FILE_RESULT_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, read_file_result, "ReadFileResult")

#define GENERATE_VARIANT_ENUM_READ_FILE_RESULT() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(bool) { \
		ReadFileResultTypeOk = false, \
		ReadFileResultTypeError = true \
	} ReadFileResultType;

#define GENERATE_VARIANT_CORE_READ_FILE_RESULT() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	typedef struct { \
		tstr file; \
	} _variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_; \
\
	typedef struct { \
		tstr_static error; \
	} _variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_; \
\
	/* raw union for variant */ \
	typedef union { \
		_variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_ ok; \
		_variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_ error; \
	} _AnonymousUnionForVariantReadFileResultImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		ReadFileResultType _variant_tag_for_read_file_result_tag_member; \
		_AnonymousUnionForVariantReadFileResultImpl_DONT_USE \
		    _variant_data_for_read_file_result_data_member; \
	} ReadFileResult; \
	typedef struct { \
		ReadFileResultType _variant_tag_for_read_file_result_tag_member; \
		_AnonymousUnionForVariantReadFileResultImpl_DONT_USE \
		    _variant_data_for_read_file_result_data_member; \
	} AssertTypeImplForReadFileResultImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantReadFileResultImpl_DONT_USE \
		    _variant_data_for_read_file_result_data_member; \
		ReadFileResultType _variant_tag_for_read_file_result_tag_member; \
	} AssertTypeImplForReadFileResultImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForReadFileResultImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForReadFileResultImpl_2_DONT_USE_), \
	              "Size for variant ReadFileResult not smaller as the inverted order, current " \
	              "order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_read_file_result(ReadFileResultType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case ReadFileResultTypeOk: { return TSTR_STATIC_LIT("ok"); } \
		case ReadFileResultTypeError: { \
			return TSTR_STATIC_LIT("error"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline ReadFileResultType \
	get_current_tag_type_for_read_file_result(ReadFileResult const variant_entry) { \
		return variant_entry._variant_tag_for_read_file_result_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline ReadFileResult \
	new_read_file_result_ok(tstr const file) { \
		return (ReadFileResult){ \
			._variant_tag_for_read_file_result_tag_member = ReadFileResultTypeOk, \
			._variant_data_for_read_file_result_data_member = \
			    (_AnonymousUnionForVariantReadFileResultImpl_DONT_USE){ \
			        .ok = \
			            (_variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_){ \
			                .file = file } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_ \
	    read_file_result_get_as_ok(ReadFileResult const variant_entry) { \
		VARIANT_READ_FILE_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_read_file_result_tag_member, ReadFileResultTypeOk); \
		return variant_entry._variant_data_for_read_file_result_data_member.ok; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_* \
	    read_file_result_get_as_ok_mut_ref(ReadFileResult* const variant_entry) { \
		VARIANT_READ_FILE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_read_file_result_tag_member, ReadFileResultTypeOk); \
		return &(variant_entry->_variant_data_for_read_file_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_* \
	    read_file_result_get_as_ok_const_ref(const ReadFileResult* const variant_entry) { \
		VARIANT_READ_FILE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_read_file_result_tag_member, ReadFileResultTypeOk); \
		return &(variant_entry->_variant_data_for_read_file_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline ReadFileResult \
	new_read_file_result_error(tstr_static const error) { \
		return (ReadFileResult){ \
			._variant_tag_for_read_file_result_tag_member = ReadFileResultTypeError, \
			._variant_data_for_read_file_result_data_member = \
			    (_AnonymousUnionForVariantReadFileResultImpl_DONT_USE){ \
			        .error = \
			            (_variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_){ \
			                .error = error } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_ \
	    read_file_result_get_as_error(ReadFileResult const variant_entry) { \
		VARIANT_READ_FILE_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_read_file_result_tag_member, ReadFileResultTypeError); \
		return variant_entry._variant_data_for_read_file_result_data_member.error; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_* \
	    read_file_result_get_as_error_mut_ref(ReadFileResult* const variant_entry) { \
		VARIANT_READ_FILE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_read_file_result_tag_member, ReadFileResultTypeError); \
		return &(variant_entry->_variant_data_for_read_file_result_data_member.error); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_* \
	    read_file_result_get_as_error_const_ref(const ReadFileResult* const variant_entry) { \
		VARIANT_READ_FILE_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_read_file_result_tag_member, ReadFileResultTypeError); \
		return &(variant_entry->_variant_data_for_read_file_result_data_member.error); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_read_file_result") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_READ_FILE_RESULT() \
	GENERATE_VARIANT_ENUM_READ_FILE_RESULT() \
	GENERATE_VARIANT_CORE_READ_FILE_RESULT()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define IF_READ_FILE_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
                                                                                var_name) \
	if((variant_entry)._variant_tag_for_read_file_result_tag_member == ReadFileResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_ const \
			        var_name = \
			            ((variant_entry)._variant_data_for_read_file_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define IF_READ_FILE_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                              var_name) \
	if((variant_entry)._variant_tag_for_read_file_result_tag_member == ReadFileResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_ \
			        var_name = \
			            ((variant_entry)._variant_data_for_read_file_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define IF_READ_FILE_RESULT_IS_OK_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_read_file_result_tag_member == ReadFileResultTypeOk)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, ok)

#define IF_READ_FILE_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	if((variant_entry)->_variant_tag_for_read_file_result_tag_member == ReadFileResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_read_file_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, ok)

#define IF_READ_FILE_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	if((variant_entry)->_variant_tag_for_read_file_result_tag_member == ReadFileResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_read_file_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, error)

#define IF_READ_FILE_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)._variant_tag_for_read_file_result_tag_member == ReadFileResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_ const \
			        var_name = \
			            ((variant_entry)._variant_data_for_read_file_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, error)

#define IF_READ_FILE_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)._variant_tag_for_read_file_result_tag_member == ReadFileResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_ \
			        var_name = \
			            ((variant_entry)._variant_data_for_read_file_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define IF_READ_FILE_RESULT_IS_ERROR_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_read_file_result_tag_member == ReadFileResultTypeError)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_2(variant_entry, \
	                                                                               error)

#define IF_READ_FILE_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_read_file_result_tag_member == ReadFileResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = false) \
			for(const _variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_read_file_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_2(variant_entry, \
	                                                                             error)

#define IF_READ_FILE_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_read_file_result_tag_member == ReadFileResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_read_file_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define IF_READ_FILE_RESULT_IS_NOT_OK(variant_entry) \
	if((variant_entry)._variant_tag_for_read_file_result_tag_member != ReadFileResultTypeOk)

#define IF_READ_FILE_RESULT_IS_NOT_ERROR(variant_entry) \
	if((variant_entry)._variant_tag_for_read_file_result_tag_member != ReadFileResultTypeError)

#define SWITCH_READ_FILE_RESULT(variant_entry) \
	_Pragma("GCC diagnostic push") \
	    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ switch( \
	        (variant_entry)._variant_tag_for_read_file_result_tag_member) \
	        _Pragma("GCC diagnostic pop")

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define CASE_READ_FILE_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
                                                                                  var_name) \
	case ReadFileResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_ const \
			        var_name = \
			            ((variant_entry)._variant_data_for_read_file_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define CASE_READ_FILE_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                                var_name) \
	case ReadFileResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_ \
			        var_name = \
			            ((variant_entry)._variant_data_for_read_file_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define CASE_READ_FILE_RESULT_IS_OK_IGN() case ReadFileResultTypeOk:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, ok)

#define CASE_READ_FILE_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case ReadFileResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_read_file_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, ok)

#define CASE_READ_FILE_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	case ReadFileResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_read_file_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
	                                                                             error)

#define CASE_READ_FILE_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	case ReadFileResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_ const \
			        var_name = \
			            ((variant_entry)._variant_data_for_read_file_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, error)

#define CASE_READ_FILE_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case ReadFileResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_ \
			        var_name = \
			            ((variant_entry)._variant_data_for_read_file_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define CASE_READ_FILE_RESULT_IS_ERROR_IGN() case ReadFileResultTypeError:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_READ_FILE_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case ReadFileResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_read_file_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_2(variant_entry, \
	                                                                               error)

#define CASE_READ_FILE_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_READ_FILE_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case ReadFileResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_* const \
			        var_name = \
			            &((variant_entry)->_variant_data_for_read_file_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl = \
			        false)

#pragma GCC poison _variant_tag_for_read_file_result_tag_member _variant_data_for_read_file_result_data_member _for_macro_trick_for_if_expr_impl_once_variant_read_file_result_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_read_file_result_variant_impl _variant_impl_unnamed_struct_for_variant_read_file_result_id_12_name_ok_impl_ _variant_impl_unnamed_struct_for_variant_read_file_result_id_13_name_error_impl_ _AnonymousUnionForVariantReadFileResultImpl_DONT_USE AssertTypeImplForReadFileResultImpl_1_DONT_USE_ AssertTypeImplForReadFileResultImpl_2_DONT_USE_

#define VARIANT_SIMPLE_REGEX_RESULT_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, simple_regex_result, "SimpleRegexResult")

#define GENERATE_VARIANT_ENUM_SIMPLE_REGEX_RESULT() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(bool) { \
		SimpleRegexResultTypeOk = false, \
		SimpleRegexResultTypeError = true \
	} SimpleRegexResultType;

#define GENERATE_VARIANT_CORE_SIMPLE_REGEX_RESULT() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	/* raw union for variant */ \
	typedef union { \
		SimpleRegex ok; \
		tstr error; \
	} _AnonymousUnionForVariantSimpleRegexResultImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		SimpleRegexResultType _variant_tag_for_simple_regex_result_tag_member; \
		_AnonymousUnionForVariantSimpleRegexResultImpl_DONT_USE \
		    _variant_data_for_simple_regex_result_data_member; \
	} SimpleRegexResult; \
	typedef struct { \
		SimpleRegexResultType _variant_tag_for_simple_regex_result_tag_member; \
		_AnonymousUnionForVariantSimpleRegexResultImpl_DONT_USE \
		    _variant_data_for_simple_regex_result_data_member; \
	} AssertTypeImplForSimpleRegexResultImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantSimpleRegexResultImpl_DONT_USE \
		    _variant_data_for_simple_regex_result_data_member; \
		SimpleRegexResultType _variant_tag_for_simple_regex_result_tag_member; \
	} AssertTypeImplForSimpleRegexResultImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForSimpleRegexResultImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForSimpleRegexResultImpl_2_DONT_USE_), \
	              "Size for variant SimpleRegexResult not smaller as the inverted order, current " \
	              "order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_simple_regex_result( \
	    SimpleRegexResultType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case SimpleRegexResultTypeOk: { \
			return TSTR_STATIC_LIT("ok"); \
		} \
		case SimpleRegexResultTypeError: { \
			return TSTR_STATIC_LIT("error"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline SimpleRegexResultType \
	get_current_tag_type_for_simple_regex_result(SimpleRegexResult const variant_entry) { \
		return variant_entry._variant_tag_for_simple_regex_result_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline SimpleRegexResult \
	new_simple_regex_result_ok(SimpleRegex const value) { \
		return (SimpleRegexResult){ \
			._variant_tag_for_simple_regex_result_tag_member = SimpleRegexResultTypeOk, \
			._variant_data_for_simple_regex_result_data_member = \
			    (_AnonymousUnionForVariantSimpleRegexResultImpl_DONT_USE){ .ok = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline SimpleRegex \
	simple_regex_result_get_as_ok(SimpleRegexResult const variant_entry) { \
		VARIANT_SIMPLE_REGEX_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_simple_regex_result_tag_member, \
		    SimpleRegexResultTypeOk); \
		return variant_entry._variant_data_for_simple_regex_result_data_member.ok; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline SimpleRegex* \
	simple_regex_result_get_as_ok_mut_ref(SimpleRegexResult* const variant_entry) { \
		VARIANT_SIMPLE_REGEX_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_simple_regex_result_tag_member, \
		    SimpleRegexResultTypeOk); \
		return &(variant_entry->_variant_data_for_simple_regex_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline SimpleRegex const* \
	simple_regex_result_get_as_ok_const_ref(const SimpleRegexResult* const variant_entry) { \
		VARIANT_SIMPLE_REGEX_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_simple_regex_result_tag_member, \
		    SimpleRegexResultTypeOk); \
		return &(variant_entry->_variant_data_for_simple_regex_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline SimpleRegexResult \
	new_simple_regex_result_error(tstr const value) { \
		return (SimpleRegexResult){ \
			._variant_tag_for_simple_regex_result_tag_member = SimpleRegexResultTypeError, \
			._variant_data_for_simple_regex_result_data_member = \
			    (_AnonymousUnionForVariantSimpleRegexResultImpl_DONT_USE){ .error = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr \
	simple_regex_result_get_as_error(SimpleRegexResult const variant_entry) { \
		VARIANT_SIMPLE_REGEX_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_simple_regex_result_tag_member, \
		    SimpleRegexResultTypeError); \
		return variant_entry._variant_data_for_simple_regex_result_data_member.error; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr* \
	simple_regex_result_get_as_error_mut_ref(SimpleRegexResult* const variant_entry) { \
		VARIANT_SIMPLE_REGEX_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_simple_regex_result_tag_member, \
		    SimpleRegexResultTypeError); \
		return &(variant_entry->_variant_data_for_simple_regex_result_data_member.error); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr const* \
	simple_regex_result_get_as_error_const_ref(const SimpleRegexResult* const variant_entry) { \
		VARIANT_SIMPLE_REGEX_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_simple_regex_result_tag_member, \
		    SimpleRegexResultTypeError); \
		return &(variant_entry->_variant_data_for_simple_regex_result_data_member.error); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_simple_regex_result") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_SIMPLE_REGEX_RESULT() \
	GENERATE_VARIANT_ENUM_SIMPLE_REGEX_RESULT() \
	GENERATE_VARIANT_CORE_SIMPLE_REGEX_RESULT()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define IF_SIMPLE_REGEX_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)._variant_tag_for_simple_regex_result_tag_member == SimpleRegexResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(SimpleRegex const var_name = \
			        ((variant_entry)._variant_data_for_simple_regex_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define IF_SIMPLE_REGEX_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)._variant_tag_for_simple_regex_result_tag_member == SimpleRegexResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(SimpleRegex var_name = \
			        ((variant_entry)._variant_data_for_simple_regex_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define IF_SIMPLE_REGEX_RESULT_IS_OK_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_simple_regex_result_tag_member == SimpleRegexResultTypeOk)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, \
	                                                                               ok)

#define IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_simple_regex_result_tag_member == \
	   SimpleRegexResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(const SimpleRegex* const var_name = \
			        &((variant_entry)->_variant_data_for_simple_regex_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, ok)

#define IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_simple_regex_result_tag_member == \
	   SimpleRegexResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(SimpleRegex* const var_name = \
			        &((variant_entry)->_variant_data_for_simple_regex_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
	                                                                              error)

#define IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_simple_regex_result_tag_member == \
	   SimpleRegexResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(tstr const var_name = \
			        ((variant_entry)._variant_data_for_simple_regex_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
	                                                                            error)

#define IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
                                                                                    var_name) \
	if((variant_entry)._variant_tag_for_simple_regex_result_tag_member == \
	   SimpleRegexResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(tstr var_name = \
			        ((variant_entry)._variant_data_for_simple_regex_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define IF_SIMPLE_REGEX_RESULT_IS_ERROR_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_simple_regex_result_tag_member == \
	   SimpleRegexResultTypeError)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_simple_regex_result_tag_member == \
	   SimpleRegexResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(const tstr* const var_name = \
			        &((variant_entry)->_variant_data_for_simple_regex_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_2(variant_entry, \
	                                                                                error)

#define IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_simple_regex_result_tag_member == \
	   SimpleRegexResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(tstr* const var_name = \
			        &((variant_entry)->_variant_data_for_simple_regex_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define IF_SIMPLE_REGEX_RESULT_IS_NOT_OK(variant_entry) \
	if((variant_entry)._variant_tag_for_simple_regex_result_tag_member != SimpleRegexResultTypeOk)

#define IF_SIMPLE_REGEX_RESULT_IS_NOT_ERROR(variant_entry) \
	if((variant_entry)._variant_tag_for_simple_regex_result_tag_member != \
	   SimpleRegexResultTypeError)

#define SWITCH_SIMPLE_REGEX_RESULT(variant_entry) \
	_Pragma("GCC diagnostic push") \
	    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ switch( \
	        (variant_entry)._variant_tag_for_simple_regex_result_tag_member) \
	        _Pragma("GCC diagnostic pop")

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_IMPL_2( \
    variant_entry, var_name) \
	case SimpleRegexResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(SimpleRegex const var_name = \
			        ((variant_entry)._variant_data_for_simple_regex_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case SimpleRegexResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(SimpleRegex var_name = \
			        ((variant_entry)._variant_data_for_simple_regex_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define CASE_SIMPLE_REGEX_RESULT_IS_OK_IGN() case SimpleRegexResultTypeOk:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_2( \
	    variant_entry, ok)

#define CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case SimpleRegexResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(const SimpleRegex* const var_name = \
			        &((variant_entry)->_variant_data_for_simple_regex_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, \
	                                                                               ok)

#define CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_OK_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case SimpleRegexResultTypeOk: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(SimpleRegex* const var_name = \
			        &((variant_entry)->_variant_data_for_simple_regex_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
	                                                                                error)

#define CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	case SimpleRegexResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(tstr const var_name = \
			        ((variant_entry)._variant_data_for_simple_regex_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
	                                                                              error)

#define CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_IMPL_2( \
    variant_entry, var_name) \
	case SimpleRegexResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(tstr var_name = \
			        ((variant_entry)._variant_data_for_simple_regex_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define CASE_SIMPLE_REGEX_RESULT_IS_ERROR_IGN() case SimpleRegexResultTypeError:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case SimpleRegexResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(const tstr* const var_name = \
			        &((variant_entry)->_variant_data_for_simple_regex_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_SIMPLE_REGEX_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case SimpleRegexResultTypeError: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
		        false) \
			for(tstr* const var_name = \
			        &((variant_entry)->_variant_data_for_simple_regex_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl = \
			        false)

#pragma GCC poison _variant_tag_for_simple_regex_result_tag_member _variant_data_for_simple_regex_result_data_member _for_macro_trick_for_if_expr_impl_once_variant_simple_regex_result_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_simple_regex_result_variant_impl _AnonymousUnionForVariantSimpleRegexResultImpl_DONT_USE AssertTypeImplForSimpleRegexResultImpl_1_DONT_USE_ AssertTypeImplForSimpleRegexResultImpl_2_DONT_USE_

#define VARIANT_UTF8_DATA_RESULT_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, utf8_data_result, "Utf8DataResult")

#define GENERATE_VARIANT_ENUM_UTF8_DATA_RESULT() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(bool) { \
		Utf8DataResultTypeOk = false, \
		Utf8DataResultTypeError = true \
	} Utf8DataResultType;

#define GENERATE_VARIANT_CORE_UTF8_DATA_RESULT() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	/* raw union for variant */ \
	typedef union { \
		Utf8Data ok; \
		tstr_static error; \
	} _AnonymousUnionForVariantUtf8DataResultImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		Utf8DataResultType _variant_tag_for_utf8_data_result_tag_member; \
		_AnonymousUnionForVariantUtf8DataResultImpl_DONT_USE \
		    _variant_data_for_utf8_data_result_data_member; \
	} Utf8DataResult; \
	typedef struct { \
		Utf8DataResultType _variant_tag_for_utf8_data_result_tag_member; \
		_AnonymousUnionForVariantUtf8DataResultImpl_DONT_USE \
		    _variant_data_for_utf8_data_result_data_member; \
	} AssertTypeImplForUtf8DataResultImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantUtf8DataResultImpl_DONT_USE \
		    _variant_data_for_utf8_data_result_data_member; \
		Utf8DataResultType _variant_tag_for_utf8_data_result_tag_member; \
	} AssertTypeImplForUtf8DataResultImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForUtf8DataResultImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForUtf8DataResultImpl_2_DONT_USE_), \
	              "Size for variant Utf8DataResult not smaller as the inverted order, current " \
	              "order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_utf8_data_result(Utf8DataResultType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case Utf8DataResultTypeOk: { return TSTR_STATIC_LIT("ok"); } \
		case Utf8DataResultTypeError: { \
			return TSTR_STATIC_LIT("error"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8DataResultType \
	get_current_tag_type_for_utf8_data_result(Utf8DataResult const variant_entry) { \
		return variant_entry._variant_tag_for_utf8_data_result_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8DataResult \
	new_utf8_data_result_ok(Utf8Data const value) { \
		return (Utf8DataResult){ \
			._variant_tag_for_utf8_data_result_tag_member = Utf8DataResultTypeOk, \
			._variant_data_for_utf8_data_result_data_member = \
			    (_AnonymousUnionForVariantUtf8DataResultImpl_DONT_USE){ .ok = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8Data \
	utf8_data_result_get_as_ok(Utf8DataResult const variant_entry) { \
		VARIANT_UTF8_DATA_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_utf8_data_result_tag_member, Utf8DataResultTypeOk); \
		return variant_entry._variant_data_for_utf8_data_result_data_member.ok; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8Data* \
	utf8_data_result_get_as_ok_mut_ref(Utf8DataResult* const variant_entry) { \
		VARIANT_UTF8_DATA_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_utf8_data_result_tag_member, Utf8DataResultTypeOk); \
		return &(variant_entry->_variant_data_for_utf8_data_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8Data const* \
	utf8_data_result_get_as_ok_const_ref(const Utf8DataResult* const variant_entry) { \
		VARIANT_UTF8_DATA_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_utf8_data_result_tag_member, Utf8DataResultTypeOk); \
		return &(variant_entry->_variant_data_for_utf8_data_result_data_member.ok); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline Utf8DataResult \
	new_utf8_data_result_error(tstr_static const value) { \
		return (Utf8DataResult){ \
			._variant_tag_for_utf8_data_result_tag_member = Utf8DataResultTypeError, \
			._variant_data_for_utf8_data_result_data_member = \
			    (_AnonymousUnionForVariantUtf8DataResultImpl_DONT_USE){ .error = value } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	utf8_data_result_get_as_error(Utf8DataResult const variant_entry) { \
		VARIANT_UTF8_DATA_RESULT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_utf8_data_result_tag_member, Utf8DataResultTypeError); \
		return variant_entry._variant_data_for_utf8_data_result_data_member.error; \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static* \
	utf8_data_result_get_as_error_mut_ref(Utf8DataResult* const variant_entry) { \
		VARIANT_UTF8_DATA_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_utf8_data_result_tag_member, Utf8DataResultTypeError); \
		return &(variant_entry->_variant_data_for_utf8_data_result_data_member.error); \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static const* \
	utf8_data_result_get_as_error_const_ref(const Utf8DataResult* const variant_entry) { \
		VARIANT_UTF8_DATA_RESULT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_utf8_data_result_tag_member, Utf8DataResultTypeError); \
		return &(variant_entry->_variant_data_for_utf8_data_result_data_member.error); \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_utf8_data_result") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_UTF8_DATA_RESULT() \
	GENERATE_VARIANT_ENUM_UTF8_DATA_RESULT() \
	GENERATE_VARIANT_CORE_UTF8_DATA_RESULT()

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define IF_UTF8_DATA_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
                                                                                var_name) \
	if((variant_entry)._variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = false) \
			for(Utf8Data const var_name = \
			        ((variant_entry)._variant_data_for_utf8_data_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define IF_UTF8_DATA_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                              var_name) \
	if((variant_entry)._variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = false) \
			for(Utf8Data var_name = \
			        ((variant_entry)._variant_data_for_utf8_data_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define IF_UTF8_DATA_RESULT_IS_OK_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeOk)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, ok)

#define IF_UTF8_DATA_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	if((variant_entry)->_variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = false) \
			for(const Utf8Data* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_data_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, ok)

#define IF_UTF8_DATA_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, \
                                                                                  var_name) \
	if((variant_entry)->_variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeOk) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = false) \
			for(Utf8Data* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_data_result_data_member.ok); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, error)

#define IF_UTF8_DATA_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
                                                                                   var_name) \
	if((variant_entry)._variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = false) \
			for(tstr_static const var_name = \
			        ((variant_entry)._variant_data_for_utf8_data_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, error)

#define IF_UTF8_DATA_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
                                                                                 var_name) \
	if((variant_entry)._variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = false) \
			for(tstr_static var_name = \
			        ((variant_entry)._variant_data_for_utf8_data_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define IF_UTF8_DATA_RESULT_IS_ERROR_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeError)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_2(variant_entry, \
	                                                                               error)

#define IF_UTF8_DATA_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = false) \
			for(const tstr_static* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_data_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_2(variant_entry, \
	                                                                             error)

#define IF_UTF8_DATA_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_utf8_data_result_tag_member == Utf8DataResultTypeError) \
		for(bool _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = false) \
			for(tstr_static* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_data_result_data_member.error); \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define IF_UTF8_DATA_RESULT_IS_NOT_OK(variant_entry) \
	if((variant_entry)._variant_tag_for_utf8_data_result_tag_member != Utf8DataResultTypeOk)

#define IF_UTF8_DATA_RESULT_IS_NOT_ERROR(variant_entry) \
	if((variant_entry)._variant_tag_for_utf8_data_result_tag_member != Utf8DataResultTypeError)

#define SWITCH_UTF8_DATA_RESULT(variant_entry) \
	_Pragma("GCC diagnostic push") \
	    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ switch( \
	        (variant_entry)._variant_tag_for_utf8_data_result_tag_member) \
	        _Pragma("GCC diagnostic pop")

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_2(variant_entry, ok)

#define CASE_UTF8_DATA_RESULT_IS_OK_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_IMPL_2(variant_entry, \
                                                                                  var_name) \
	case Utf8DataResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        false) \
			for(Utf8Data const var_name = \
			        ((variant_entry)._variant_data_for_utf8_data_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_2(variant_entry, ok)

#define CASE_UTF8_DATA_RESULT_IS_OK_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_IMPL_2(variant_entry, \
                                                                                var_name) \
	case Utf8DataResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        false) \
			for(Utf8Data var_name = \
			        ((variant_entry)._variant_data_for_utf8_data_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define CASE_UTF8_DATA_RESULT_IS_OK_IGN() case Utf8DataResultTypeOk:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_2(variant_entry, ok)

#define CASE_UTF8_DATA_RESULT_IS_OK_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case Utf8DataResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        false) \
			for(const Utf8Data* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_data_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, ok)

#define CASE_UTF8_DATA_RESULT_IS_OK_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_OK_MUT_REF_IMPL_2(variant_entry, \
                                                                                    var_name) \
	case Utf8DataResultTypeOk: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        false) \
			for(Utf8Data* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_data_result_data_member.ok); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_2(variant_entry, \
	                                                                             error)

#define CASE_UTF8_DATA_RESULT_IS_ERROR_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_IMPL_2( \
    variant_entry, var_name) \
	case Utf8DataResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        false) \
			for(tstr_static const var_name = \
			        ((variant_entry)._variant_data_for_utf8_data_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_1(variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, error)

#define CASE_UTF8_DATA_RESULT_IS_ERROR_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_1)(__VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_IMPL_2(variant_entry, \
                                                                                   var_name) \
	case Utf8DataResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        false) \
			for(tstr_static var_name = \
			        ((variant_entry)._variant_data_for_utf8_data_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define CASE_UTF8_DATA_RESULT_IS_ERROR_IGN() case Utf8DataResultTypeError:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
	    variant_entry, error)

#define CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case Utf8DataResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        false) \
			for(const tstr_static* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_data_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_2(variant_entry, \
	                                                                               error)

#define CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_UTF8_DATA_RESULT_IS_ERROR_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case Utf8DataResultTypeError: \
		for(bool _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
		        false) \
			for(tstr_static* const var_name = \
			        &((variant_entry)->_variant_data_for_utf8_data_result_data_member.error); \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl = \
			        false)

#pragma GCC poison _variant_tag_for_utf8_data_result_tag_member _variant_data_for_utf8_data_result_data_member _for_macro_trick_for_if_expr_impl_once_variant_utf8_data_result_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_utf8_data_result_variant_impl _AnonymousUnionForVariantUtf8DataResultImpl_DONT_USE AssertTypeImplForUtf8DataResultImpl_1_DONT_USE_ AssertTypeImplForUtf8DataResultImpl_2_DONT_USE_

#define VARIANT_ALLOCATOR_FUNCTION_HANDLE_CONTENT_STATE_ASSERT(state, expected_state) \
	VARIANT_STATE_ASSERT(state, expected_state, allocator_function_handle_content, \
	                     "AllocatorFunctionHandleContent")

#define GENERATE_VARIANT_ENUM_ALLOCATOR_FUNCTION_HANDLE_CONTENT() \
	/* @enum value */ \
	typedef enum _IMPL_JSON_VARIANTS_C_23_NARROW_ENUM_TO(uint8_t) { \
		AllocatorFunctionHandleTypeFailAlways = 0, \
		AllocatorFunctionHandleTypeFailAfter, \
		AllocatorFunctionHandleTypeFailNever \
	} AllocatorFunctionHandleType;

#define GENERATE_VARIANT_CORE_ALLOCATOR_FUNCTION_HANDLE_CONTENT() \
	__INTERNAL_VARIANT_DECLARATION_START_IMPL_() \
	typedef struct { \
		size_t count; \
	} _variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_; \
\
	/* raw union for variant */ \
	typedef union { \
		_variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_ \
		    fail_after; \
	} _AnonymousUnionForVariantAllocatorFunctionHandleContentImpl_DONT_USE; \
\
	/* tagged union (variant) implementation */ \
	typedef struct { \
		AllocatorFunctionHandleType _variant_tag_for_allocator_function_handle_content_tag_member; \
		_AnonymousUnionForVariantAllocatorFunctionHandleContentImpl_DONT_USE \
		    _variant_data_for_allocator_function_handle_content_data_member; \
	} AllocatorFunctionHandleContent; \
	typedef struct { \
		AllocatorFunctionHandleType _variant_tag_for_allocator_function_handle_content_tag_member; \
		_AnonymousUnionForVariantAllocatorFunctionHandleContentImpl_DONT_USE \
		    _variant_data_for_allocator_function_handle_content_data_member; \
	} AssertTypeImplForAllocatorFunctionHandleContentImpl_1_DONT_USE_; \
	typedef struct { \
		_AnonymousUnionForVariantAllocatorFunctionHandleContentImpl_DONT_USE \
		    _variant_data_for_allocator_function_handle_content_data_member; \
		AllocatorFunctionHandleType _variant_tag_for_allocator_function_handle_content_tag_member; \
	} AssertTypeImplForAllocatorFunctionHandleContentImpl_2_DONT_USE_; \
	static_assert(sizeof(AssertTypeImplForAllocatorFunctionHandleContentImpl_1_DONT_USE_) <= \
	                  sizeof(AssertTypeImplForAllocatorFunctionHandleContentImpl_2_DONT_USE_), \
	              "Size for variant AllocatorFunctionHandleContent not smaller as the inverted " \
	              "order, current order: tag is first"); \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline tstr_static \
	_impl_get_state_string_for_variant_allocator_function_handle_content( \
	    AllocatorFunctionHandleType const enum_value) { \
		_Pragma("GCC diagnostic push") \
		    _Pragma("GCC diagnostic ignored \"-Wswitch-bool\"") /* FOR GCC */ \
		    switch(enum_value) { \
		_Pragma("GCC diagnostic pop") case AllocatorFunctionHandleTypeFailAlways: { \
			return TSTR_STATIC_LIT("fail_always"); \
		} \
		case AllocatorFunctionHandleTypeFailAfter: { \
			return TSTR_STATIC_LIT("fail_after"); \
		} \
		case AllocatorFunctionHandleTypeFailNever: { \
			return TSTR_STATIC_LIT("fail_never"); \
		} \
		default: { \
			return TSTR_STATIC_LIT("<unknown>"); \
		} \
		} \
	} \
\
	JSON_VARIANTS_NODISCARD JSON_VARIANTS_MAYBE_UNUSED static inline AllocatorFunctionHandleType \
	get_current_tag_type_for_allocator_function_handle_content( \
	    AllocatorFunctionHandleContent const variant_entry) { \
		return variant_entry._variant_tag_for_allocator_function_handle_content_tag_member; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline AllocatorFunctionHandleContent \
	    new_allocator_function_handle_content_fail_always(void) { \
		return (AllocatorFunctionHandleContent){ \
			._variant_tag_for_allocator_function_handle_content_tag_member = \
			    AllocatorFunctionHandleTypeFailAlways, \
			._variant_data_for_allocator_function_handle_content_data_member = \
			    (_AnonymousUnionForVariantAllocatorFunctionHandleContentImpl_DONT_USE){} \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline AllocatorFunctionHandleContent \
	    new_allocator_function_handle_content_fail_after(size_t const count) { \
		return (AllocatorFunctionHandleContent){ \
			._variant_tag_for_allocator_function_handle_content_tag_member = \
			    AllocatorFunctionHandleTypeFailAfter, \
			._variant_data_for_allocator_function_handle_content_data_member = \
			    (_AnonymousUnionForVariantAllocatorFunctionHandleContentImpl_DONT_USE){ \
			        .fail_after = \
			            (_variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_){ \
			                .count = count } } \
		}; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_ \
	    allocator_function_handle_content_get_as_fail_after( \
	        AllocatorFunctionHandleContent const variant_entry) { \
		VARIANT_ALLOCATOR_FUNCTION_HANDLE_CONTENT_STATE_ASSERT( \
		    variant_entry._variant_tag_for_allocator_function_handle_content_tag_member, \
		    AllocatorFunctionHandleTypeFailAfter); \
		return variant_entry._variant_data_for_allocator_function_handle_content_data_member \
		    .fail_after; \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline _variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_* \
	    allocator_function_handle_content_get_as_fail_after_mut_ref( \
	        AllocatorFunctionHandleContent* const variant_entry) { \
		VARIANT_ALLOCATOR_FUNCTION_HANDLE_CONTENT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_allocator_function_handle_content_tag_member, \
		    AllocatorFunctionHandleTypeFailAfter); \
		return &(variant_entry->_variant_data_for_allocator_function_handle_content_data_member \
		             .fail_after); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline const _variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_* \
	    allocator_function_handle_content_get_as_fail_after_const_ref( \
	        const AllocatorFunctionHandleContent* const variant_entry) { \
		VARIANT_ALLOCATOR_FUNCTION_HANDLE_CONTENT_STATE_ASSERT( \
		    variant_entry->_variant_tag_for_allocator_function_handle_content_tag_member, \
		    AllocatorFunctionHandleTypeFailAfter); \
		return &(variant_entry->_variant_data_for_allocator_function_handle_content_data_member \
		             .fail_after); \
	} \
\
	JSON_VARIANTS_NODISCARD \
	    JSON_VARIANTS_MAYBE_UNUSED static inline AllocatorFunctionHandleContent \
	    new_allocator_function_handle_content_fail_never(void) { \
		return (AllocatorFunctionHandleContent){ \
			._variant_tag_for_allocator_function_handle_content_tag_member = \
			    AllocatorFunctionHandleTypeFailNever, \
			._variant_data_for_allocator_function_handle_content_data_member = \
			    (_AnonymousUnionForVariantAllocatorFunctionHandleContentImpl_DONT_USE){} \
		}; \
	} \
\
	_Pragma("GCC poison _impl_get_state_string_for_variant_allocator_function_handle_content") \
	    __INTERNAL_VARIANT_DECLARATION_END_IMPL_()

#define GENERATE_VARIANT_ALL_ALLOCATOR_FUNCTION_HANDLE_CONTENT() \
	GENERATE_VARIANT_ENUM_ALLOCATOR_FUNCTION_HANDLE_CONTENT() \
	GENERATE_VARIANT_CORE_ALLOCATOR_FUNCTION_HANDLE_CONTENT()

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_ALWAYS(variant_entry) \
	if((variant_entry)._variant_tag_for_allocator_function_handle_content_tag_member == \
	   AllocatorFunctionHandleTypeFailAlways)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_2( \
	    variant_entry, fail_after)

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_allocator_function_handle_content_tag_member == \
	   AllocatorFunctionHandleTypeFailAfter) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_ const \
			        var_name = \
			            ((variant_entry) \
			                 ._variant_data_for_allocator_function_handle_content_data_member \
			                 .fail_after); \
			    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_2( \
	    variant_entry, fail_after)

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)._variant_tag_for_allocator_function_handle_content_tag_member == \
	   AllocatorFunctionHandleTypeFailAfter) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_ \
			        var_name = \
			            ((variant_entry) \
			                 ._variant_data_for_allocator_function_handle_content_data_member \
			                 .fail_after); \
			    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
			        false)

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_IGN(variant_entry) \
	if((variant_entry)._variant_tag_for_allocator_function_handle_content_tag_member == \
	   AllocatorFunctionHandleTypeFailAfter)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_2( \
	    variant_entry, fail_after)

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_allocator_function_handle_content_tag_member == \
	   AllocatorFunctionHandleTypeFailAfter) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_allocator_function_handle_content_data_member \
			                  .fail_after); \
			    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_2( \
	    variant_entry, fail_after)

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	if((variant_entry)->_variant_tag_for_allocator_function_handle_content_tag_member == \
	   AllocatorFunctionHandleTypeFailAfter) \
		for(bool \
		        _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		            true; \
		    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
		    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_allocator_function_handle_content_data_member \
			                  .fail_after); \
			    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
			    _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
			        false)

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_NEVER(variant_entry) \
	if((variant_entry)._variant_tag_for_allocator_function_handle_content_tag_member == \
	   AllocatorFunctionHandleTypeFailNever)

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_NOT_FAIL_ALWAYS(variant_entry) \
	if((variant_entry)._variant_tag_for_allocator_function_handle_content_tag_member != \
	   AllocatorFunctionHandleTypeFailAlways)

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_NOT_FAIL_AFTER(variant_entry) \
	if((variant_entry)._variant_tag_for_allocator_function_handle_content_tag_member != \
	   AllocatorFunctionHandleTypeFailAfter)

#define IF_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_NOT_FAIL_NEVER(variant_entry) \
	if((variant_entry)._variant_tag_for_allocator_function_handle_content_tag_member != \
	   AllocatorFunctionHandleTypeFailNever)

#define SWITCH_ALLOCATOR_FUNCTION_HANDLE_CONTENT(variant_entry) \
	switch((variant_entry)._variant_tag_for_allocator_function_handle_content_tag_member)

#define CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_ALWAYS() \
	case AllocatorFunctionHandleTypeFailAlways:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_2( \
	    variant_entry, fail_after)

#define CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_IMPL_2( \
    variant_entry, var_name) \
	case AllocatorFunctionHandleTypeFailAfter: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_ const \
			        var_name = \
			            ((variant_entry) \
			                 ._variant_data_for_allocator_function_handle_content_data_member \
			                 .fail_after); \
			    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_2( \
	    variant_entry, fail_after)

#define CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_IMPL_2( \
    variant_entry, var_name) \
	case AllocatorFunctionHandleTypeFailAfter: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_ \
			        var_name = \
			            ((variant_entry) \
			                 ._variant_data_for_allocator_function_handle_content_data_member \
			                 .fail_after); \
			    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
			        false)

#define CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_IGN() \
	case AllocatorFunctionHandleTypeFailAfter:

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_2( \
	    variant_entry, fail_after)

#define CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_CONST_REF_IMPL_2( \
    variant_entry, var_name) \
	case AllocatorFunctionHandleTypeFailAfter: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		        false) \
			for(const _variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_allocator_function_handle_content_data_member \
			                  .fail_after); \
			    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
			        false)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_MACRO_HELPER_IMPL_( \
    _1, _2, NAME, ...) \
	NAME

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_1( \
    variant_entry) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_2( \
	    variant_entry, fail_after)

#define CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF(...) \
	__INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_MACRO_HELPER_IMPL_( \
	    __VA_ARGS__, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_2, \
	    __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_1)( \
	    __VA_ARGS__)

#define __INTERNAL_HELPER_MACRO_DONT_USE_CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT_REF_IMPL_2( \
    variant_entry, var_name) \
	case AllocatorFunctionHandleTypeFailAfter: \
		for(bool \
		        _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		            true; \
		    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
		    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
		        false) \
			for(_variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_* const \
			        var_name = \
			            &((variant_entry) \
			                  ->_variant_data_for_allocator_function_handle_content_data_member \
			                  .fail_after); \
			    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl; \
			    _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl = \
			        false)

#define CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_NEVER() \
	case AllocatorFunctionHandleTypeFailNever:

#pragma GCC poison _variant_tag_for_allocator_function_handle_content_tag_member _variant_data_for_allocator_function_handle_content_data_member _for_macro_trick_for_if_expr_impl_once_variant_allocator_function_handle_content_variant_impl _for_macro_trick_for_case_expr_impl_once_variant_allocator_function_handle_content_variant_impl _variant_impl_unnamed_struct_for_variant_allocator_function_handle_content_id_14_name_fail_after_impl_ _AnonymousUnionForVariantAllocatorFunctionHandleContentImpl_DONT_USE AssertTypeImplForAllocatorFunctionHandleContentImpl_1_DONT_USE_ AssertTypeImplForAllocatorFunctionHandleContentImpl_2_DONT_USE_

#ifdef __cplusplus
__INTERNAL_VARIANT_DECLARATION_END_IMPL_()
}
#endif

#ifdef __cplusplus
	#define CPP_DEFINE_ERROR_VARIANTS_START_DECLS() \
		namespace cpp::error_variants { \
		template <typename T> struct IsErrorVariant : std::false_type {}; \
\
		template <typename T> struct ErrorVariantConversionImpl; \
		}
	#define CPP_DEFINE_ERROR_VARIANTS_JSON_VARIANTS() \
		namespace cpp::error_variants {}
	#if !defined(CPP_DEFINE_ERROR_VARIANTS) && !defined(CPP_DEFINE_ERROR_VARIANTS_HAVE_MULTIPLE)
		#define CPP_DEFINE_ERROR_VARIANTS() \
			CPP_DEFINE_ERROR_VARIANTS_START_DECLS() \
			CPP_DEFINE_ERROR_VARIANTS_JSON_VARIANTS
	#else
	    // NOTE: if we have multiple generated variant header available, this macros can't be used,
	    // so disable them!
		#ifdef CPP_DEFINE_ERROR_VARIANTS
			#undef CPP_DEFINE_ERROR_VARIANTS
		#endif

		#ifndef CPP_DEFINE_ERROR_VARIANTS_HAVE_MULTIPLE
			#define CPP_DEFINE_ERROR_VARIANTS_HAVE_MULTIPLE
		#endif
	#endif

#endif

/* NOLINTEND(readability-identifier-naming,bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp,bugprone-macro-parentheses)
 */
