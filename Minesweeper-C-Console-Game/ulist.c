// C Standard Libraries
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

// My Library
#include "ulist.h"


// Ulist Create Function
ulist_t* ulist_create(const size_t size){
	ulist_t* __ulist__ = (ulist_t*) malloc(sizeof(ulist_t));
	if (!__ulist__) return NULL;
	__ulist__->buffer = (matrix_t*) malloc(sizeof(matrix_t) * size);
	if (!__ulist__->buffer){
		free(__ulist__);
		return NULL;
	}
	__ulist__->capacity = size;
	__ulist__->iterator = 0;
	return __ulist__;
}

// Ulist Resize Function
bool ulist_resize(ulist_t* __ulist__, const size_t new_size){
	matrix_t* __ulist_buffer_resize__ = (matrix_t*) realloc(__ulist__->buffer, sizeof(matrix_t) * new_size);
	if (!__ulist_buffer_resize__) return false;
	__ulist__->buffer = __ulist_buffer_resize__;
	__ulist__->capacity = new_size;
	return true;
}

// Ulist Insert Function
bool ulist_insert(ulist_t* __ulist__, const matrix_t new_data){
	if (__ulist__->capacity == __ulist__->iterator){
		if (ulist_resize(__ulist__, __ulist__->capacity * 2) == false){
			return false;
		}
	}
	__ulist__->buffer[__ulist__->iterator++] = new_data;
	return true;		
}

void ulist_delete(ulist_t* __ulist__){
	free(__ulist__->buffer);
	free(__ulist__);
}
