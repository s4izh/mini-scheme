#ifndef __TYPES_H__
#define __TYPES_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float    f32;
typedef double   f64;

#include "sv.h"
#include "ds.h"

#define is_between(i, start, end) ((token) >= (start) && (token) <= (end))

DA_DEFINE(char, da_char);
DA_DEFINE(u32, da_u32);

#endif // __TYPES_H__
