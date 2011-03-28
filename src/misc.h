
/**
 * \file misc.h
 * \brief Helper functions.
 *
 * \author Gregory Bard <bard@fordham.edu>
 * \author Martin Albrecht <M.R.Albrecht@rhul.ac.uk>
 */

#ifndef MISC_H
#define MISC_H
/*******************************************************************
*
*                 M4RI: Linear Algebra over GF(2)
*
*    Copyright (C) 2007, 2008 Gregory Bard <bard@fordham.edu>
*    Copyright (C) 2008 Martin Albrecht <M.R.Albrecht@rhul.ac.uk>
*
*  Distributed under the terms of the GNU General Public License (GPL)
*  version 2 or higher.
*
*    This code is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*  The full text of the GPL is available at:
*
*                  http://www.gnu.org/licenses/
*
********************************************************************/

#ifndef HAVE_SSE2
#undef HAVE_MM_MALLOC
#endif

#ifdef HAVE_MM_MALLOC
#include <mm_malloc.h>
#endif

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

/*
 * These define entirely the word width used in the library.
 */

/**
 * \brief Pretty for a boolean int.
 *
 * The value of a BIT is either 0 or 1.
 */

typedef int BIT;

/**
 * \brief Type of row and column indexes.
 *
 * This type is used for integer values that hold row/colum sized values.
 */

typedef int rci_t;

/**
 * \brief Type of word indexes.
 *
 * This type is used for the array of words that make up a row.
 */

typedef int wi_t;

#ifdef M4RI_WRAPWORD
// C++ wrapper class around an uint64_t, exclusively interesting for the developer(s) of M4RI.
#include "wordwrapper.h"
#else

/**
 * A word is the typical packed data structure to represent packed
 * bits.
 */

typedef uint64_t word;

/*
 * Explicit conversion of a word, representing 64 columns, to an integer
 * to be used as index into an array. This is used for Gray codes.
 * No error checking is done that the most significant bits in w are zero.
 */

#define CONVERT_TO_INT(w) ((int)(w))

/*
 * Explicit conversion of a word, representing 64 columns, to a BIT
 * to be used as boolean: this is an int with value 0 (false) or 1 (true).
 * No error checking is done that only the least significant bit is set (if any).
 */

#define CONVERT_TO_BIT(w) ((BIT)(w))

/*
 * Explicit conversion of a word, representing 64 columns, to an uint64_t.
 *
 * The returned value is the underlaying integer representation of these 64 columns,
 * meaning in particular that if val is an uint64_t then
 * CONVERT_TO_UINT64_T(CONVERT_TO_WORD(val)) == val.
 */

#define CONVERT_TO_UINT64_T(w) (w)

/*
 * Explicit conversion of an integer to a word.
 */

#define CONVERT_TO_WORD(i) ((word)(i))

#endif

/**
 * \brief The number of bits in a word.
 */

static int const RADIX = 64;

/**
 * \brief The number one as a word.
 */

static word const ONE = CONVERT_TO_WORD(1);

/**
 * \brief A word with all bits set.
 */

static word const FFFF = CONVERT_TO_WORD(-1);

/**
 * \brief Return the maximal element of x and y
 *
 * \param x Word
 * \param y Word
 */

#ifndef MAX
#define MAX(x,y) (((x) > (y))?(x):(y))
#endif

/**
 * \brief Return the minimal element of x and y
 *
 * \param x Word
 * \param y Word
 */

#ifndef MIN
#define MIN(x,y) (((x) < (y))?(x):(y))
#endif

/**
 * \brief Return r such that x elements fit into r blocks of length y.
 *
 * \param x Number of elements
 * \param y Block size
 */

#define DIV_CEIL(x,y) (((x) % (y)) ? (x) / (y) + 1 : (x) / (y))

/**
 *\brief Pretty for 1.
 */ 

#define TRUE 1

/**
 *\brief Pretty for 0.
 */ 

#define FALSE 0

/**
 * \brief $2^i$
 *
 * \param i Integer.
 */ 

#define TWOPOW(i) ((uint64_t)1 << (i))

/**
 * \brief Clear the bit spot (counting from the left) in the word w
 * 
 * \param w Word
 * \param spot Integer with 0 <= spot < RADIX
 */

#define CLR_BIT(w, spot) ((w) &= ~(ONE << (spot))

/**
 * \brief Set the bit spot (counting from the left) in the word w
 * 
 * \param w Word
 * \param spot Integer with 0 <= spot < RADIX
 */

#define SET_BIT(w, spot) ((w) |= (ONE << (spot)))

/**
 * \brief Get the bit spot (counting from the left) in the word w
 * 
 * \param w Word
 * \param spot Integer with 0 <= spot < RADIX
 */

#define GET_BIT(w, spot) CONVERT_TO_BIT(((w) >> (spot)) & ONE)

/**
 * \brief Write the value to the bit spot in the word w
 * 
 * \param w Word.
 * \param spot Integer with 0 <= spot < RADIX.
 * \param value Either 0 or 1.
 */

#define WRITE_BIT(w, spot, value) ((w) = (((w) & ~(ONE << (spot))) | (-CONVERT_TO_WORD(value) & (ONE << (spot)))))

/**
 * \brief Flip the spot in the word w
 *
 * \param w Word.
 * \param spot Integer with 0 <= spot < RADIX.
 */

#define FLIP_BIT(w, spot) ((w) ^= (ONE << (spot)))

/**
* \brief create a bit mask to zero out all but the (n - 1) % RADIX + 1 leftmost bits.
*
* This function returns 1..64 bits, never zero bits.
* This mask is mainly used to mask the valid bits in the most significant word,
* by using LEFT_BITMASK((M->ncols + M->offset) % RADIX).
* In other words, the set bits represent the columns with the lowest index in the word.
*
*  Thus,
*
*  n	Output
*  0=64 1111111111111111111111111111111111111111111111111111111111111111
*  1	0000000000000000000000000000000000000000000000000000000000000001
*  2    0000000000000000000000000000000000000000000000000000000000000011
*  .                                   ...
* 62    0011111111111111111111111111111111111111111111111111111111111111
* 63	0111111111111111111111111111111111111111111111111111111111111111
*
* Note that n == 64 is only passed from MIDDLE_BITMASK, and still works
* (behaves the same as n == 0): the input is modulo 64.
*
* \param n Integer with 0 <= n <= RADIX
*/

#define LEFT_BITMASK(n) (FFFF >> (RADIX - (n)) % RADIX)

/**
* \brief create a bit mask to zero out all but the n rightmost bits.
*
* This function returns 1..64 bits, never zero bits.
* This mask is mainly used to mask the n valid bits in the least significant word
* with valid bits by using RIGHT_BITMASK(RADIX - M->offset % RADIX).
* In other words, the set bits represent the columns with the highest index in the word.
*
*  Thus,
*
*  n	Output
*  1	1000000000000000000000000000000000000000000000000000000000000000
*  2    1100000000000000000000000000000000000000000000000000000000000000
*  3    1110000000000000000000000000000000000000000000000000000000000000
*  .                                   ...
* 63	1111111111111111111111111111111111111111111111111111111111111110
* 64	1111111111111111111111111111111111111111111111111111111111111111
*
* Note that n == 0 is never passed and would fail.
*
* \param n Integer with 0 < n <= RADIX
*/

#define RIGHT_BITMASK(n) (FFFF << (RADIX - (n)))

/**
* \brief create a bit mask that is the combination of LEFT_BITMASK and RIGHT_BITMASK.
*
* This function returns 1..64 bits, never zero bits.
* This mask is mainly used to mask the n valid bits in the only word with valid bits,
* when M->ncols + M->offset <= RADIX), by using MIDDLE_BITMASK(M->ncols, M->offset).
* It is equivalent to LEFT_BITMASK(n + offset) & RIGHT_BITMASK(RADIX - offset).
* In other words, the set bits represent the valid columns in the word.
*
* Note that when n == RADIX (and thus offset == 0) then LEFT_BITMASK is called with n == 64.
*
* \param n Integer with 0 < n <= RADIX - offset
* \param offset Column offset, with 0 <= offset < RADIX
*/

#define MIDDLE_BITMASK(n, offset) (LEFT_BITMASK(n) << (offset))

/**
 * \brief swap bits in the word v
 *
 * \param n
 */

static inline word swap_bits(word v) {
  v = ((v >>  1) & 0x5555555555555555ULL) | ((v & 0x5555555555555555ULL) << 1);
  v = ((v >>  2) & 0x3333333333333333ULL) | ((v & 0x3333333333333333ULL) << 2);
  v = ((v >>  4) & 0x0F0F0F0F0F0F0F0FULL) | ((v & 0x0F0F0F0F0F0F0F0FULL) << 4);
  v = ((v >>  8) & 0x00FF00FF00FF00FFULL) | ((v & 0x00FF00FF00FF00FFULL) << 8);
  v = ((v >> 16) & 0x0000FFFF0000FFFFULL) | ((v & 0x0000FFFF0000FFFFULL) << 16);
  v =  (v >> 32)                          |  (v                          << 32);
  return v;
}

/**
 * \brief Return alignment of addr w.r.t. n. For example the address
 * 17 would be 1 aligned w.r.t. 16.
 *
 * \param addr
 * \param n
 */

#define ALIGNMENT(addr, n) (((unsigned long)(addr))%(n))

/**
 * Return true if a's least significant bit is smaller than b's least significant bit.
 *
 * return true if LSBI(a) < LSBI(b),
 * where LSBI(w) is the index of the least significant bit that is set in w, or 64 if w is zero.
 *
 * \param a Word
 * \param b Word
 */

static inline int lesser_LSB(word a, word b)
{
  uint64_t const ia = CONVERT_TO_UINT64_T(a);
  uint64_t const ib = CONVERT_TO_UINT64_T(b);
  /*
   * If a is zero then we should always return false, otherwise
   * if b is zero we should return true iff a has at least one bit set.
   */
  return !(ib ? ((ia - 1) ^ ia) & ib : !ia);
}


/**** Error Handling *****/

/**
 * \brief Print error message and abort(). 
 * 
 * The function accepts additional
 * parameters like printf, so e.g. m4ri_die("foo %d bar %f\n",1 ,2.0)
 * is valid and will print the string "foo 1 bar 2.0" before dying.
 *
 * \param errormessage a string to be printed.
 *
 * \todo Allow user to register callback which is called on
 * m4ri_die().
 *
 * \warning The provided string is not free'd.
 */

void m4ri_die(const char *errormessage, ...);

/**** IO *****/

/**
 * \brief Write a sting representing the word data to destination. 
 * 
 * \param destination Address of buffer of length at least RADIX*1.3
 * \param data Source word
 * \param colon Insert a Colon after every 4-th bit. 
 * \warning Assumes destination has RADIX*1.3 bytes available 
 */
void m4ri_word_to_str( char *destination, word data, int colon);

/**
 * \brief Return 1 or 0 uniformly randomly distributed.
 *
 * \todo Allow user to provide her own random() function.
 */

static inline BIT m4ri_coin_flip() {
  if (rand() < RAND_MAX/2) {
    return 0;
  }  else {
    return 1;
  }
}

/**
 * \brief Return uniformly randomly distributed random word.
 *
 * \todo Allow user to provide her own random() function.
 */

word m4ri_random_word();

/***** Initialization *****/

/**
 * \brief Initialize global data structures for the M4RI library.
 *
 * On Linux/Solaris this is called automatically when the shared
 * library is loaded, but it doesn't harm if it is called twice.
 */

#if defined(__GNUC__)
void __attribute__ ((constructor)) m4ri_init(void);
#else
void m4ri_init(void);
#endif

#ifdef __SUNPRO_C
#pragma init(m4ri_init)
#endif

/**
 * \brief De-initialize global data structures from the M4RI library. 
 *
 * On Linux/Solaris this is called automatically when the shared
 * library is unloaded, but it doesn't harm if it is called twice.
 */

#if defined(__GNUC__)
void __attribute__ ((destructor)) m4ri_fini(void);
#else
void m4ri_fini(void);
#endif

#ifdef __SUNPRO_C
#pragma fini(m4ri_fini)
#endif

/***** Memory Management *****/

#if CPU_L2_CACHE == 0
/**
 * Fix some standard value for L2 cache size if it couldn't be
 * determined by configure.
 */
#define CPU_L2_CACHE 524288
#endif //CPU_L2_CACHE

#if CPU_L1_CACHE == 0
/**
 * Fix some standard value for L1 cache size if it couldn't be
 * determined by configure.
 */
#define CPU_L1_CACHE 16384
#endif //CPU_L1_CACHE

/**
 * \brief Calloc wrapper.
 *
 * \param count Number of elements.
 * \param size Size of each element.
 *
 * \todo Allow user to register calloc function.
 */

static inline void *m4ri_mm_calloc(size_t count, size_t size) {
  void *newthing;
#ifdef HAVE_OPENMP
#pragma omp critical
{
#endif

#ifdef HAVE_MM_MALLOC
  newthing = _mm_malloc(count * size, 16);
#else
  newthing = calloc(count, size);
#endif

#ifdef HAVE_OPENMP
 }
#endif

  if (newthing==NULL) {
    m4ri_die("m4ri_mm_calloc: calloc returned NULL\n");
    return NULL; /* unreachable. */
  }
#ifdef HAVE_MM_MALLOC
  char *b = (char*)newthing;
  memset(b, 0, count * size);
#endif
  return newthing;
}

/**
 * \brief Malloc wrapper.
 *
 * \param size Size in bytes.
 *
 * \todo Allow user to register malloc function.
 */

static inline void *m4ri_mm_malloc(size_t size) {
  void *newthing;
#ifdef HAVE_OPENMP
#pragma omp critical
{
#endif

#ifdef HAVE_MM_MALLOC
  newthing = _mm_malloc(size, 16);
#else
  newthing = malloc( size );
#endif  
#ifdef HAVE_OPENMP
 }
#endif
  if (newthing==NULL && (size>0)) {
    m4ri_die("m4ri_mm_malloc: malloc returned NULL\n");
    return NULL; /* unreachable */
  }
  else return newthing;
}


/**
 * \brief Free wrapper.
 *
 * \param condemned Pointer.
 *
 * \todo Allow user to register free function.
 */

/* void m4ri_mm_free(void *condemned, ...); */
static inline void m4ri_mm_free(void *condemned, ...) { 
#ifdef HAVE_MM_MALLOC
  _mm_free(condemned); 
#else
  free(condemned);
#endif
}

/**
 * \brief Maximum number of bytes allocated in one malloc() call.
 *
 * This value must fit in an int, even though it's type is size_t.
 */

#define MM_MAX_MALLOC (((size_t)1)<<30)

/**
 * \brief Enable memory block cache (default: disabled)
 */
#define ENABLE_MMC


/**
 * \brief Number of blocks that are cached.
 */

#define M4RI_MMC_NBLOCKS 16

/**
 * \brief Maximal size of blocks stored in cache.
 */

#define M4RI_MMC_THRESHOLD CPU_L2_CACHE

/**
 * The mmc memory management functions check a cache for re-usable
 * unused memory before asking the system for it.
 */

typedef struct _mm_block {
  /**
   * Size in bytes of the data.
   */
  size_t size;

  /**
   * Pointer to buffer of data.
   */
  void *data;

} mmb_t;

/**
 * The actual memory block cache.
 */

extern mmb_t m4ri_mmc_cache[M4RI_MMC_NBLOCKS];

/**
 * \brief Return handle for locale memory management cache.
 *
 * \attention Not thread safe.
 */

static inline mmb_t *m4ri_mmc_handle(void) {
  return m4ri_mmc_cache;
}

/**
 * \brief Allocate size bytes.
 *
 * \param size Number of bytes.
 */

static inline void *m4ri_mmc_malloc(size_t size) {

#ifdef ENABLE_MMC
  void *ret = NULL;
#endif

#ifdef HAVE_OPENMP
#pragma omp critical
{
#endif

#ifdef ENABLE_MMC
  mmb_t *mm = m4ri_mmc_handle();
  if (size <= M4RI_MMC_THRESHOLD) {
    for (int i = 0; i < M4RI_MMC_NBLOCKS; ++i) {
      if(mm[i].size == size) {
        ret = mm[i].data;
        mm[i].data = NULL;
        mm[i].size = 0;
        break;
      }
    }
  }
#endif //ENABLE_MMC

#ifdef HAVE_OPENMP
 }
#endif

#ifdef ENABLE_MMC
 if (ret)
   return ret;
 else
   return m4ri_mm_malloc(size);
#else 
 return m4ri_mm_malloc(size);
#endif
}

/**
 * \brief Allocate size times count zeroed bytes.
 *
 * \param size Number of bytes per block.
 * \param count Number of blocks.
 *
 * \warning Not thread safe.
 */

static inline void *m4ri_mmc_calloc(size_t size, size_t count) {
  void *ret = m4ri_mmc_malloc(size * count);
  memset((char*)ret, 0, count * size);
  return ret;
}

/**
 * \brief Free the data pointed to by condemned of the given size.
 *
 * \param condemned Pointer to memory.
 * \param size Number of bytes.
 *
 * \warning Not thread safe.
 */

static inline void m4ri_mmc_free(void *condemned, size_t size) {
#ifdef HAVE_OPENMP
#pragma omp critical
{
#endif
#ifdef ENABLE_MMC  
  static int j = 0;
  mmb_t *mm = m4ri_mmc_handle();
  if (size < M4RI_MMC_THRESHOLD) {
    for(int i = 0; i < M4RI_MMC_NBLOCKS; ++i) {
      if(mm[i].size == 0) {
        mm[i].size = size;
        mm[i].data = condemned;
        return;
      }
    }
    m4ri_mm_free(mm[j].data);
    mm[j].size = size;
    mm[j].data = condemned;
    j = (j+1) % M4RI_MMC_NBLOCKS;
  } else {
    m4ri_mm_free(condemned);
  }
#else
  m4ri_mm_free(condemned);
#endif //ENABLE_MMC
#ifdef HAVE_OPENMP
 }
#endif
}

/**
 * \brief Cleans up the cache.
 *
 * This function is called automatically when the shared library is
 * loaded.
 *
 * \warning Not thread safe.
 */

static inline void m4ri_mmc_cleanup(void) {
#ifdef HAVE_OPENMP
#pragma omp critical
{
#endif
  mmb_t *mm = m4ri_mmc_handle();
  for(int i = 0; i < M4RI_MMC_NBLOCKS; ++i) {
    if (mm[i].size)
      m4ri_mm_free(mm[i].data);
    mm[i].size = 0;
  }
#ifdef HAVE_OPENMP
 }
#endif
}

#endif //MISC_H
