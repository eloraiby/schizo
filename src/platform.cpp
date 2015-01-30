#include <malloc.h>

void*
operator new(size_t count) {
	return malloc(count);
}

void operator delete(void* ptr) {
	free(ptr);
}
