#ifndef __ARCH_ARMV7_COMMON_INCLUDE_MEMORY__
#define __ARCH_ARMV7_COMMON_INCLUDE_MEMORY__


#include "common/include/data.h"


#ifndef PAGE_SIZE
#define PAGE_SIZE   4096
#endif

#ifndef PAGE_BITS
#define PAGE_BITS   12
#endif

#ifndef PAGE_LEVELS
#define PAGE_LEVELS 2
#endif


#ifndef ALIGN_MIN
#define ALIGN_MIN   1
#endif

#ifndef ALIGN_MAX
#define ALIGN_MAX   64
#endif

#ifndef ALIGN_DEFAULT
#define ALIGN_DEFAULT   4
#endif


#ifndef PFN_TO_ADDR
#define PFN_TO_ADDR(pfn)        ((pfn) << 12)
#endif

#ifndef ADDR_TO_PFN
#define ADDR_TO_PFN(addr)       ((addr) >> 12)
#endif


#ifndef SFN_TO_ADDR
#define SFN_TO_ADDR(pfn)        ((pfn) << 20)
#endif

#ifndef ADDR_TO_SFN
#define ADDR_TO_SFN(addr)       ((addr) >> 20)
#endif



#ifndef L1TABLE_ENTRY_COUNT
#define L1TABLE_ENTRY_COUNT     4096
#endif

#ifndef L2TABLE_ENTRY_COUNT
#define L2TABLE_ENTRY_COUNT     256
#endif


#ifndef GET_L1PTE_INDEX
#define GET_L1PTE_INDEX(addr)   ((addr) >> 20)
#endif

#ifndef GET_L2PTE_INDEX
#define GET_L2PTE_INDEX(addr)   (((addr) >> 12) & 0xfful)
#endif

#ifndef GET_PAGE_OFFSET
#define GET_PAGE_OFFSET(addr)   ((addr) & 0xffful)
#endif


/*
 * AP[2:0]  Kernel  User
 * 000      x       x
 * 001      rw      x
 * 010      rw      r
 * 011      rw      rw
 * 100      -       -
 * 101      r       x
 * 110      r       r
 * 111      r       r
 */

/*
 * TEX[2] == 1 -> Cacheable
 * TEX[1:0]/CB  Attri
 * 00           Non-cacheable
 * 01           Write-back, write-allocate
 * 10           Write-through, no write-allocate
 * 11           Write-back, no write-allocate
 * 
 * TEX[2] == 0 -> Special
 * TEX[1:0]:C:B Type            Shareable       Desc
 * 00:0:0       Strong-ordered  Shareable       Strongly-ordered
 * 00:0:1       Device          Shareable       Shareable device
 * 00:1:0       Normal          S bit           Outer and inner write-through, no write-allocate
 * 00:1:1       Normal          S bit           Outer and inner write-back, no write-allocate
 * 01:0:0       Normal          S bit           Outer and inner non-cacheable
 * 01:0:1       -               -               Reserved
 * 01:1:0       Impl            Impl            Implementation-defined
 * 01:1:1       Normal          S bit           Outer and inner write-back, write-allocate
 * 10:0:0       Device          Non-shareable   Non-shareable device
 * 10:0:1       -               -               Reserved
 * 10:1:X       -               -               Reserved
 * 11:X:X       -               -               Reserved
 */

struct l1pte {
    union {
        u32         value;
        
        struct {
            u32     present         : 1;
            u32     reserved1       : 2;
            u32     non_secure      : 1;
            u32     reserved2       : 1;
            u32     domain          : 4;
            u32     reserved3       : 1;
            u32     pfn_ext         : 2;
            u32     pfn             : 20;
        };
    };
} packedstruct;

struct l1section {
    union {
        u32         value;
        
        struct {
            u32     reserved1       : 1;
            u32     present         : 1;
            u32     cache_inner     : 2;    // c, b
            u32     no_exec         : 1;
            u32     domain          : 4;
            u32     reserved2       : 1;
            u32     user_write      : 1;    // AP[0]
            u32     user_access     : 1;    // AP[1]
            u32     cache_outer     : 2;    // TEX[1:0]
            u32     cacheable       : 1;    // TEX[2]
            u32     read_only       : 1;    // AP[2]
            u32     shareable       : 1;
            u32     non_global      : 1;
            u32     reserved3       : 1;
            u32     non_secure      : 1;
            u32     sfn             : 12;
        };
    };
} packedstruct;

struct l2pte {
    union {
        u32         value;
        
        struct {
            u32     no_exec         : 1;
            u32     present         : 1;
            u32     cache_inner     : 2;    // c, b
            u32     user_write      : 1;    // AP[0]
            u32     user_access     : 1;    // AP[1]
            u32     cache_outer     : 2;    // TEX[1:0]
            u32     cacheable       : 1;    // TEX[2]
            u32     read_only       : 1;    // AP[2]    for both user and kernel
            u32     shareable       : 1;
            u32     non_global      : 1;
            u32     pfn             : 20;
        };
    };
} packedstruct;

struct page_frame {
    union {
        u8 value_u8[4096];
        u32 value_u32[1024];
    };
} packedstruct;

struct l1table {
    union {
        u8 value_u8[16384];
        u32 value_u32[4096];
        
        struct l1pte value_l1pte[4096];
        struct l1section value_l1section[4096];
    };
} packedstruct;

struct l2table {
    union {
        u8 value_u8[4096];
        u32 value_u32[1024];
        
        struct {
            struct l2pte value_l2pte[256];
            struct l2pte value_l2pte_dup[3][256];
        };
    };
} packedstruct;


#endif
