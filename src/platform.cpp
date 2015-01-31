#include <malloc.h>

void*
operator new(size_t count) noexcept {
	return malloc(count);
}

void operator delete(void* ptr) noexcept {
	free(ptr);
}
