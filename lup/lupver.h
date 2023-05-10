#ifndef __LUPVER_H
# define __LUPVER_H

# define LUP_VERSION_MAJOR 0x00
# define LUP_VERSION_MINOR 0x00
# define LUP_VERSION_PATCH 0x00

# define LUP_VERSION            \
  (                             \
    (LUP_VERSION_MAJOR << 16) | \
    (LUP_VERSION_MINOR <<  8) | \
    (LUP_VERSION_PATCH <<  0)   \
  )

#endif
