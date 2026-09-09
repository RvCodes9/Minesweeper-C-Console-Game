#ifndef ULIST_H
#define ULIST_H

// C Standard Libraries
#include <stddef.h>
#include <stdbool.h>

// Matrix Type (Struct Implementation)
typedef struct {
	size_t col;
	size_t row;		
} matrix_t;

// Ulist (Struct Type Implementation)
typedef struct{
	matrix_t* buffer;
	size_t iterator;
	size_t capacity;
} ulist_t;

// Ulist Function Implementations
ulist_t* ulist_create(const size_t);
bool ulist_resize(ulist_t*, const size_t);
bool ulist_insert(ulist_t*, const matrix_t);
void ulist_delete(ulist_t*);

#endif
