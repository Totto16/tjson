#pragma once

#ifndef _TJSON_IMPL_INTERNAL__
	#error "can only be used internally"
#endif

#include "../allocator.h"

#include "../_impl/utils.h"

#include <tstr.h>

#include "../tjson/variants.h"

GENERATE_VARIANT_ALL_READ_FILE_RESULT()

NODISCARD ReadFileResult read_entire_file(const tstr* file_path);
