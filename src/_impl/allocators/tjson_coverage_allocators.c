#include "./tjson_coverage_allocators.h"

#include <stdlib.h>

#include "../utils.h"

#include "../../tjson/variants.h"

GENERATE_VARIANT_ALL_ALLOCATOR_FUNCTION_HANDLE_CONTENT()

struct AllocatorFunctionHandleImpl {
	AllocatorFunctionHandleContent content;
	AllocatorFunctionType type;
};

static bool handle_should_fail_impl(AllocatorFunctionHandle* const handle) {
	SWITCH_ALLOCATOR_FUNCTION_HANDLE_CONTENT(handle->content) { // GCOVR_EXCL_BR_WITHOUT_HIT: 1/4
		CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_ALWAYS() {
			return true;
		}
		VARIANT_CASE_END();
		CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_AFTER_MUT(handle->content) {
			size_t* const count = &(fail_after->count);

			bool result = false;

			if(*count == 0) {
				result = true;
				goto change_to_never_fail;
			} else {
				result = false;
			}

			(*count)--;

			return result;
		change_to_never_fail:
			handle->content = new_allocator_function_handle_content_fail_never();
			return result;
		}
		VARIANT_CASE_END();
		CASE_ALLOCATOR_FUNCTION_HANDLE_CONTENT_IS_FAIL_NEVER() {
			return false;
		}
		VARIANT_CASE_END();
		default: {       // GCOVR_EXCL_LINE
			return true; // GCOVR_EXCL_LINE
		}
	}
}

static AllocatorFunctionHandle*
    malloc_handle = // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    NULL;

void* tjson_coverage_allocator_malloc(size_t size) {
	if(malloc_handle != NULL) {
		if(handle_should_fail_impl(malloc_handle)) {
			return NULL;
		}
	}

	return malloc(size);
}

static AllocatorFunctionHandle*
    calloc_handle // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    = NULL;

void* tjson_coverage_allocator_calloc(size_t nmemb, size_t size) {
	if(calloc_handle != NULL) {
		if(handle_should_fail_impl(calloc_handle)) {
			return NULL;
		}
	}

	return calloc(nmemb, size);
}

static AllocatorFunctionHandle*
    realloc_handle // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    = NULL;

void* tjson_coverage_allocator_realloc(void* ptr, size_t size) {
	if(realloc_handle != NULL) {
		if(handle_should_fail_impl(realloc_handle)) {
			return NULL;
		}
	}

	return realloc(ptr, size);
}

void tjson_coverage_allocator_free(void* ptr) {
	// NOTE: free can't ever fail
	free(ptr);
}

AllocatorFunctionHandle* tjson_coverage_allocator_get_handle(const AllocatorFunctionType type) {

	AllocatorFunctionHandle** to_use = NULL;

	switch(type) { // GCOVR_EXCL_BR_WITHOUT_HIT: 1/4
		case AllocatorFunctionTypeMalloc: {
			to_use = &malloc_handle;
			break;
		}
		case AllocatorFunctionTypeCalloc: {
			to_use = &calloc_handle;
			break;
		}
		case AllocatorFunctionTypeRealloc: {
			to_use = &realloc_handle;
			break;
		}
		default: {       // GCOVR_EXCL_LINE
			return NULL; // GCOVR_EXCL_LINE
		}
	}

	if(to_use == NULL) { // GCOVR_EXCL_LINE
		return NULL;     // GCOVR_EXCL_LINE
	}

	if(*to_use != NULL) {
		// fprintf(stderr, "Handle already retrieved, this handle can only be used once!\n");
		return NULL;
	}

	AllocatorFunctionHandle* allocated = malloc(sizeof(AllocatorFunctionHandle));

	if(allocated == NULL) { // GCOVR_EXCL_LINE
		return NULL;        // GCOVR_EXCL_LINE
	}

	*allocated =
	    (AllocatorFunctionHandle){ .content = new_allocator_function_handle_content_fail_never(),
		                           .type = type };

	*to_use = allocated;

	return *to_use;
}

bool tjson_coverage_allocator_handle_fail_always(AllocatorFunctionHandle* const handle) {
	handle->content = new_allocator_function_handle_content_fail_always();
	return true;
}

bool tjson_coverage_allocator_handle_fail_after(AllocatorFunctionHandle* const handle,
                                                size_t count) {
	handle->content = new_allocator_function_handle_content_fail_after(count);
	return true;
}

bool tjson_coverage_allocator_handle_fail_never(AllocatorFunctionHandle* const handle) {
	handle->content = new_allocator_function_handle_content_fail_never();
	return true;
}

void tjson_coverage_allocator_free_handle(AllocatorFunctionHandle* const handle) {

	AllocatorFunctionHandle** used = NULL;

	switch(handle->type) { // GCOVR_EXCL_BR_WITHOUT_HIT: 1/4
		case AllocatorFunctionTypeMalloc: {
			used = &malloc_handle;
			break;
		}
		case AllocatorFunctionTypeCalloc: {
			used = &calloc_handle;
			break;
		}
		case AllocatorFunctionTypeRealloc: {
			used = &realloc_handle;
			break;
		}
		default: {  // GCOVR_EXCL_LINE
			return; // GCOVR_EXCL_LINE
		}
	}

	free(handle);
	*used = NULL;
}
