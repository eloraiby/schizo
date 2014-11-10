#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#	define INLINE inline __attribute__ ((__unused__))
#	define UNUSED __attribute__ ((__unused__))
#else
#	define INLINE __inline
#	define UNUSED
#endif

#ifdef __cplusplus

#else
typedef	unsigned char	bool;
#	define false	(0)
#	define true	(!false)
#endif

typedef int8_t		sint8;
typedef int16_t		sint16;
typedef int32_t		sint32;
typedef int64_t		sint64;

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef float		real32;
typedef double		real64;

#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) < (b) ? (a) : (b))

#endif // PLATFORM_H
