#ifndef __LUP_H
# define __LUP_H

# include <stdint.h>

# define U8(c)  UINT8_C(c)
# define U16(c) UINT16_C(c)
# define U32(c) UINT32_C(c)
# define U64(c) UINT64_C(c)

typedef uint8_t  u8_t  ;
typedef uint16_t u16_t ;
typedef uint32_t u32_t ;
typedef uint64_t u64_t ;

# define I8(c)  INT8_C(c)
# define I16(c) INT16_C(c)
# define I32(c) INT32_C(c)
# define I64(c) INT64_C(c)

typedef int8_t   i8_t  ;
typedef int16_t  i16_t ;
typedef int32_t  i32_t ;
typedef int64_t  i64_t ;

# define KiB ( U64(1) << 10 )
# define MiB ( U64(1) << 20 )
# define GiB ( U64(1) << 30 )
# define TiB ( U64(1) << 40 )
# define PiB ( U64(1) << 50 )
# define EiB ( U64(1) << 60 )

# include <lup/lupver.h>
# include <lup/lupcfg.h>
# include <lup/lupapi.h>

# define __LUP__ LUP_VERSION
# define __lup__ LUP_VERSION

#endif