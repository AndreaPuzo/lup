#ifndef __LUPAPI_H
# define __LUPAPI_H

# define LUP_SRS_C  0
# define LUP_SRS_1  1
# define LUP_SRS_P  2
# define LUP_SRS_Z  3
# define LUP_SRS_S  4
# define LUP_SRS_O  5
# define LUP_SRS_D  6
# define LUP_SRS_V  7
# define LUP_SRS_T  8
# define LUP_SRS_I  9
# define LUP_SRS_U  10
# define LUP_SRS_PL 11

# define LUP_SRF_C  ( U32(1) << LUP_SRS_C  )
# define LUP_SRF_1  ( U32(1) << LUP_SRS_1  )
# define LUP_SRF_P  ( U32(1) << LUP_SRS_P  )
# define LUP_SRF_Z  ( U32(1) << LUP_SRS_Z  )
# define LUP_SRF_S  ( U32(1) << LUP_SRS_S  )
# define LUP_SRF_O  ( U32(1) << LUP_SRS_O  )
# define LUP_SRF_D  ( U32(1) << LUP_SRS_D  )
# define LUP_SRF_V  ( U32(1) << LUP_SRS_V  )
# define LUP_SRF_T  ( U32(1) << LUP_SRS_T  )
# define LUP_SRF_I  ( U32(1) << LUP_SRS_I  )
# define LUP_SRF_U  ( U32(1) << LUP_SRS_U  )
# define LUP_SRF_PL ( U32(3) << LUP_SRS_PL )

# define LUP_INT_DZ 0x00
# define LUP_INT_SS 0x01
# define LUP_INT_CK 0x02
# define LUP_INT_BK 0x03
# define LUP_INT_UD 0x04
# define LUP_INT_PP 0x05
# define LUP_INT_PF 0x06
# define LUP_INT_SK 0x07
# define LUP_INT_GP 0x08

# define LUP_N_INTS 0x100
# define LUP_N_REGS 0x10

typedef struct lup_cpu_s lup_cpu_t ;
typedef struct lup_mem_s lup_mem_t ;
typedef struct lup_ios_s lup_ios_t ;
typedef struct lup_s     lup_t     ;

struct lup_cpu_s {
  u32_t sr                ;
  u64_t ip                ;
  u64_t ck                ;
  u64_t cr [ LUP_N_REGS ] ;
  u64_t gr [ LUP_N_REGS ] ;

# define LUP_PAG_R 0
# define LUP_PAG_W 1
# define LUP_PAG_X 2

  u64_t ( * v2p ) (
    lup_t * ,
    u64_t   ,
    u64_t   ,
    u32_t
  ) ;
} ;

struct lup_mem_s {
  u64_t  siz ;
  void * dat ;
} ;

struct lup_ios_s {
  void * ptr ;

  void ( * out ) (
    void * ,
    u32_t  ,
    u64_t  ,
    void *
  ) ;

  void ( * in ) (
    void * ,
    u32_t  ,
    u64_t  ,
    void *
  ) ;
} ;

struct lup_s {
  lup_cpu_t cpu ;
  lup_mem_t mem ;
  lup_ios_t ios ;
} ;

# define lup_sr(vm) (vm)->cpu.sr
# define lup_ip(vm) (vm)->cpu.ip
# define lup_ck(vm) (vm)->cpu.ck
# define lup_cr(vm) (vm)->cpu.cr
# define lup_gr(vm) (vm)->cpu.gr

# define lup_pl_get(vm)      lup_f_get(lup_sr(vm), LUP_SRS_PL, LUP_SRF_PL)
# define lup_pl_set(vm, dat) lup_f_set(lup_sr(vm), LUP_SRS_PL, LUP_SRF_PL, dat)

# define lup_cr_get(vm, idx, typ)      lup_r_get(lup_cr(vm)[idx], typ)
# define lup_cr_set(vm, idx, typ, dat) lup_r_set(lup_cr(vm)[idx], typ, dat)
# define lup_gr_get(vm, idx, typ)      lup_r_get(lup_gr(vm)[idx], typ)
# define lup_gr_set(vm, idx, typ, dat) lup_r_set(lup_gr(vm)[idx], typ, dat)

__LUP_API void lup_int (
  lup_t * vm ,
  u32_t   n
) ;

__LUP_API void lup_iret (
  lup_t * vm
) ;

__LUP_API void lup_write (
  lup_t * vm  ,
  u64_t   adr ,
  u64_t   siz ,
  void *  dat
) ;

__LUP_API void lup_read (
  lup_t * vm  ,
  u64_t   adr ,
  u64_t   siz ,
  void *  dat
) ;

__LUP_API void lup_clock (
  lup_t * vm
) ;

#endif
