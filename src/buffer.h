#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>


typedef struct {
	size_t len;
	void *data;
} buffer_t;

#endif /* BUFFER_H */
