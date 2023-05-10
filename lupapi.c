#include <lup/lup.h>
#include <string.h>

int internal_clock (
  lup_t * vm  ,
  u64_t   n
)
{
  int res = LUP_SUCCESS ;

  /* check the clock counter overflow */

  if (~U64(0) - lup_ck(vm) <= n) {
    /* (E) the internal clock overflowed,
     *     the result of some operations
     *     could be compromised
     */
    lup_int(vm, LUP_INT_CK) ;
    res = LUP_FAILURE ;
  }

  /* increase the clock counter */
  lup_ck(vm) += n ;

  return res ;
}

int internal_write (
  lup_t * vm  ,
  u64_t   adr ,
  u64_t   siz ,
  void *  dat
)
{
  /* check the address */

  if (vm->mem.siz < adr + siz) {
    /* (E) the section is out of physical
     *     memory so writing is not possible
     */
    lup_int(vm, LUP_INT_GP) ;
    return LUP_FAILURE ;
  }

  /* write the first `siz`-bytes of data
   * pointed by `dat` into the memory
   */
  (void) memcpy((u8_t *)vm->mem.dat + adr, dat, siz) ;

  /* update the clock counter */
  return internal_clock(vm, lup_longsof(siz) + 1) ;
}

int internal_read (
  lup_t * vm  ,
  u64_t   adr ,
  u64_t   siz ,
  void *  dat
)
{
  /* check the address */

  if (vm->mem.siz < adr + siz) {
    /* (E) the section is out of physical
     *     memory so reading is not possible
     */
    lup_int(vm, LUP_INT_GP) ;
    return LUP_FAILURE ;
  }

  /* read the first `siz`-bytes of data
   * pointed by `dat` from the memory
   */
  (void) memcpy(dat, (u8_t *)vm->mem.dat + adr, siz) ;

  /* update the clock counter */
  return internal_clock(vm, lup_longsof(siz) + 1) ;
}

__LUP_API void lup_int (
  lup_t * vm ,
  u32_t   n
)
{
  if (0 != (lup_sr(vm) & LUP_SRF_I)) {
    /* make the interrupt index safe */
    n &= LUP_N_INTS - 1 ;

    /* build the address of the `n`-th IDE
     * (Interrupt Descriptor Entry)
     */
    u64_t adr = lup_cr(vm)[0] + n * sizeof(u64_t) ;

    /* read the IDE from the main memory */

    if (LUP_SUCCESS != internal_read(vm, adr, sizeof(u64_t), &adr))
      return ;

    /* decode the IDE and compute the offset */

    if (0 == ((adr >> 48) & 1)) {
      /* (E) the IDE is not present in the
       *     IDT (Interrupt Descriptor Table)
       */
      lup_int(vm, LUP_INT_PP) ;
      return ;
    }

    if (((adr >> 49) & 3) <= lup_pl_get(vm)) {
      /* (E) the IDE it is not accessible by
       *     the user who invoked it
       */
      lup_int(vm, LUP_INT_PP) ;
      return ;
    }

    /* clear the highest half of the IDE */
    adr = (adr << 16) >> 16 ;

    /* save the current context */

    lup_cr(vm)[1] = ((u64_t)n << 32) | lup_sr(vm) ;
    lup_cr(vm)[2] = lup_ip(vm) ;

    /* jump to the ISR (Interrupt Service Routine) */

    lup_pl_set(vm, 0) ;
    lup_sr(vm) &= ~LUP_SRF_I ;
    lup_sr(vm) |=  LUP_SRF_U ;
    lup_ip(vm) = adr ;
  }
}

__LUP_API void lup_iret (
  lup_t * vm
)
{
  if (0 != (lup_sr(vm) & LUP_SRF_U)) {
    /* restore the previous context */

    lup_sr(vm) = (u32_t)lup_cr(vm)[1] ;
    lup_ip(vm) =        lup_cr(vm)[2] ;
  } else {
    /* (E) IRET has been invoked outside
     *     the scope of an ISR
     */
    lup_int(vm, LUP_INT_GP) ;
  }
}

__LUP_API void lup_read (
  lup_t * vm  ,
  u64_t   adr ,
  u64_t   siz ,
  void *  dat
)
{
  /* translate the virtual address */

  if (NULL != vm->cpu.v2p) {
    adr = vm->cpu.v2p(vm, adr, siz, LUP_PAG_R) ;
  }

  /* read data from the memory */
  (void) internal_read(vm, adr, siz, dat) ;
}

__LUP_API void lup_write (
  lup_t * vm  ,
  u64_t   adr ,
  u64_t   siz ,
  void *  dat
)
{
  /* translate the virtual address */

  if (NULL != vm->cpu.v2p) {
    adr = vm->cpu.v2p(vm, adr, siz, LUP_PAG_R) ;
  }

  /* write data into the memory */
  (void) internal_read(vm, adr, siz, dat) ;
}

static void internal_fetch (
  lup_t * vm
)
{
  u64_t  adr = lup_ip(vm) ;
  u64_t  siz = sizeof(u32_t) ;
  void * dat = lup_cr(vm) + 4 ;

  /* if the fetch fails then the instruction to
   * perform is `XHG LONG R0, R0`
   */
  lup_cr(vm)[4] = 0x68009070 ;

  /* translate the virtual address */

  if (NULL != vm->cpu.v2p) {
    adr = vm->cpu.v2p(vm, adr, siz, LUP_PAG_R | LUP_PAG_X) ;
  }

  /* read data from the memory */
  (void) internal_read(vm, adr, siz, dat) ;
}

static inline u64_t internal_sext (
  u32_t bts ,
  u64_t dat
)
{
  return
    ( ((dat >> bts) & 1) * (~U64(0) << bts) ) |
    ( (dat & ((U64(1) << bts) - 1)) )
  ;
}

__LUP_API void lup_clock (
  lup_t * vm
)
{
  /* fetch the next instruction */

  internal_fetch(vm) ;

  /* decode the current instruction */

  u32_t ins = (u32_t)lup_cr(vm)[4] ;
  u32_t opc = (u32_t)((ins >> 24) & U32(0x000000FF)) ;
  u32_t typ = (u32_t)((ins >> 24) & U32(0x00000003)) ;
  u32_t a   = (u32_t)((ins >> 20) & U32(0x0000000F)) ;
  u32_t b   = (u32_t)((ins >> 16) & U32(0x0000000F)) ;
  u32_t c   = (u32_t)((ins >> 12) & U32(0x0000000F)) ;
  u64_t u20 = (u64_t)((ins >>  0) & U32(0x000FFFFF)) ;
  u64_t u16 = (u64_t)((ins >>  0) & U32(0x0000FFFF)) ;
  u64_t u12 = (u64_t)((ins >>  0) & U32(0x00000FFF)) ;
  u64_t i20 = internal_sext(20, u20) ;
  u64_t i16 = internal_sext(16, u16) ;
  u64_t i12 = internal_sext(12, u12) ;

  /* execute the current instruction */

  u64_t r0, r1, r2, r3 ;

  switch (opc) {

  /* TODO: execute the instruction */

  default :
    /* (E) undefined instruction */
    lup_int(vm, LUP_INT_UD) ;
    break ;
  }

  if (0 != (lup_sr(vm) & LUP_SRF_T)) {
    /* (I) interrupt the current workflow
     *     to execute the single step handler
     *     used by the debugger
     */
    lup_int(vm, LUP_INT_SS) ;
  }
}