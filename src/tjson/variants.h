#pragma once

#if _TJSON_COMPILE_WITH_NARROWED_ENUMS
	#define TJSON_C_23_NARROW_ENUM_TO(x) : x
	#define TJSON_C_23_ENUM_TYPE(x) x

	#define TJSON_VARIANT_IMPL_COMPILED_WITH_NARROWED_ENUMS 1
#else
	#define TJSON_C_23_NARROW_ENUM_TO(x)
	#define TJSON_C_23_ENUM_TYPE(x) int

	#define TJSON_VARIANT_IMPL_COMPILED_WITH_NARROWED_ENUMS 0
#endif

#define VARIANT_IMPL_JSON_VARIANTS_COMPILED_WITH_NARROWED_ENUMS \
	TJSON_VARIANT_IMPL_COMPILED_WITH_NARROWED_ENUMS

#if !defined(_TJSON_USE_VARIANT_IMPL)
	#error "'_TJSON_USE_VARIANT_IMPL' needs to be defined"
#elif _TJSON_USE_VARIANT_IMPL == 1
	#include "json_variants.h"
#elif _TJSON_USE_VARIANT_IMPL == 0
	#ifndef TJSON_ALLOW_BOOTSTRAP
		#error "tjson bootstrap not allowed"
	#else
		#include "./bootstrap_variants.h"
	#endif
#else
	#error "_TJSON_USE_VARIANT_IMPL has to be 0 or 1"
#endif
