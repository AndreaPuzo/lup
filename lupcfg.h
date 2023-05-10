#ifndef __LUPCFG_H
# define __LUPCFG_H

# if defined(_WIN32) && defined(__LUP_IS_DLL)
#  define __LUP_API_EXPORT __declspec(dllexport)
#  define __LUP_API_IMPORT __declspec(dllimport)
# else
#  define __LUP_API_EXPORT
#  define __LUP_API_IMPORT
# endif

# ifdef __LUP_EXPORT
#  define __LUP_API __LUP_API_EXPORT
# else
#  define __LUP_API __LUP_API_IMPORT
# endif

# define LUP_SUCCESS  0
# define LUP_FAILURE -1

# define LUP_TYP_BYTE 0
# define LUP_TYP_HALF 1
# define LUP_TYP_WORD 2
# define LUP_TYP_LONG 3

# define lup_bitsof(typ)  ( U32(1) << (3 + (typ)) )
# define lup_maskof(typ)  ( ( U64(1) << lup_bitsof(typ) ) - 1 )
# define lup_sizeof(typ)  ( U64(1) << (typ) )
# define lup_bytesof(siz) ( (siz) >> LUP_TYP_BYTE )
# define lup_halfsof(siz) ( (siz) >> LUP_TYP_HALF )
# define lup_wordsof(siz) ( (siz) >> LUP_TYP_WORD )
# define lup_longsof(siz) ( (siz) >> LUP_TYP_LONG )

# define lup_r_get(src, typ)            \
  (                                     \
    (src) & lup_maskof(typ)             \
  )

# define lup_r_set(dst, typ, src)       \
  (                                     \
    (dst) =                             \
      ( (dst) & ~lup_maskof(typ) ) |    \
      ( (src) &  lup_maskof(typ) )      \
  )

# define lup_f_get(src, shf, msk)       \
  (                                     \
    ( (src) & (msk) ) >> (shf)          \
  )

# define lup_f_set(dst, shf, msk, src)  \
  (                                     \
    (dst) =                             \
      ( (dst)              & ~(msk) ) | \
      ( ( (src) << (shf) ) &  (msk) )   \
  )

#endif