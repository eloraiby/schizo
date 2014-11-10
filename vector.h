#ifndef VECTOR_H
#define VECTOR_H
/*
  Schizo programming language
  Copyright (C) 2014  Wael El Oraiby

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

#include <stdlib.h>
#include <string.h>

#include "platform.h"

#define DECLARE_VEC_TYPE(name, type)	\
			typedef struct { \
				size_t	count; \
				size_t  max; \
				type*	array; \
			} name; \
			static INLINE name		name ## _new()					{ name ## _vec_t v; v.count = 0; v.max = 0; v.array = NULL; return v; } \
			static INLINE void		name ## _release(name* v)			{ v->count = 0; v->max = 0; free(v->array); v->array = NULL; } \
			static INLINE type		name ## _get(name* v, size_t idx)		{ return v->array[idx]; } \
			static INLINE void		name ## _set(name* v, size_t idx, type t)	{ v->array[idx] = t; } \
			static INLINE type		name ## _pop(name* v)				{ type t = v->array[--(v->count)]; return t; } \
			static INLINE void		name ## _push(name* v, type e)			{ if( v->count == v->max ) { \
													       v->max = v->max ? (v->max) << 1 : 2; \
													       v->array = (type*)realloc(v->array, sizeof(type) * v->max);	\
													  } \
													  v->array[(v->count)++] = e; \
													} \
			static INLINE name		name ## _copy(name* orig)			{ name dest; \
													  dest.count = orig->count; \
													  dest.max = orig->max; \
													  dest.array = malloc(sizeof(type) * orig->max); \
													  memcpy(dest.array, orig->array, sizeof(type) * orig->max); \
													  return dest; \
													} \
			static INLINE void		name ## _resize(name* v, size_t s)		{ v->max = s; v->count = MIN(v->count, s); v->array = (type*)realloc(v->array, sizeof(type) * s); }



#endif	/* VECTOR_H */
