#ifndef FTL_PLATFORM_HPP
#define FTL_PLATFORM_HPP
/*
  Schizo programming language
  Copyright (C) 2014-2015  Wael El Oraiby

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Also add information on how to contact you by electronic and paper mail.
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* platform related */
#ifdef __GNUC__
#	define INLINE __inline__ __attribute__ ((__unused__))
#	define UNUSED __attribute__ ((__unused__))
#else
#	define INLINE __inline
#	define UNUSED
#endif

template<typename T>	void*  UNUSED operator new(size_t UNUSED c, T* placement) { return placement; }
void*	operator new(size_t count);
void	operator delete(void* p);

namespace schizo {

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

template<typename T>	T min(T a, T b)	{ return a < b ? a : b; }
template<typename T>	T max(T a, T b)	{ return a > b ? a : b; }


}	// namespace schizo

#endif // FTL_PLATFORM_HPP
