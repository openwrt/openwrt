/**
 * @author (c) Eyal Rozenberg <eyalroz1@gmx.com>
 *             2021-2023, Haifa, Palestine/Israel
 * @author (c) Marco Paland (info@paland.com)
 *             2014-2019, PALANDesign Hannover, Germany
 *
 * @note Others have made smaller contributions to this file: see the
 * contributors page at https://github.com/eyalroz/printf/graphs/contributors
 * or ask one of the authors. The original code for exponential specifiers was
 * contributed by Martijn Jasperse <m.jasperse@gmail.com>.
 *
 * @brief Small stand-alone implementation of the printf family of functions
 * (`(v)printf`, `(v)s(n)printf` etc., geared towards use on embedded systems with
 * limited resources.
 *
 * @note the implementations are thread-safe; re-entrant; use no functions from
 * the standard library; and do not dynamically allocate any memory.
 *
 * @license The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Define this globally (e.g. gcc -DPRINTF_INCLUDE_CONFIG_H=1 ...) to include the
// printf_config.h header file
#if PRINTF_INCLUDE_CONFIG_H
#include "printf_config.h"
#endif

#include <printf/printf.h>

#ifdef __cplusplus
#include <cstdint>
#include <climits>
#else
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#endif // __cplusplus

#if PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD
# define printf_    printf
# define sprintf_   sprintf
# define vsprintf_  vsprintf
# define snprintf_  snprintf
# define vsnprintf_ vsnprintf
# define vprintf_   vprintf
#endif


// 'ntoa' conversion buffer size, this must be big enough to hold one converted
// numeric number including padded zeros (dynamically created on stack)
#ifndef PRINTF_INTEGER_BUFFER_SIZE
#define PRINTF_INTEGER_BUFFER_SIZE    32
#endif

// size of the fixed (on-stack) buffer for printing individual decimal numbers.
// this must be big enough to hold one converted floating-point value including
// padded zeros.
#ifndef PRINTF_DECIMAL_BUFFER_SIZE
#define PRINTF_DECIMAL_BUFFER_SIZE    32
#endif

// Support for the decimal notation floating point conversion specifiers (%f, %F)
#ifndef PRINTF_SUPPORT_DECIMAL_SPECIFIERS
#define PRINTF_SUPPORT_DECIMAL_SPECIFIERS 1
#endif

// Support for the exponential notation floating point conversion specifiers (%e, %g, %E, %G)
#ifndef PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
#define PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS 1
#endif

// Support for the length write-back specifier (%n)
#ifndef PRINTF_SUPPORT_WRITEBACK_SPECIFIER
#define PRINTF_SUPPORT_WRITEBACK_SPECIFIER 1
#endif

// Default precision for the floating point conversion specifiers (the C standard sets this at 6)
#ifndef PRINTF_DEFAULT_FLOAT_PRECISION
#define PRINTF_DEFAULT_FLOAT_PRECISION  6
#endif

// Default choice of type to use for internal floating-point computations
#ifndef PRINTF_USE_DOUBLE_INTERNALLY
#define PRINTF_USE_DOUBLE_INTERNALLY  1
#endif

// According to the C languages standard, printf() and related functions must be able to print any
// integral number in floating-point notation, regardless of length, when using the %f specifier -
// possibly hundreds of characters, potentially overflowing your buffers. In this implementation,
// all values beyond this threshold are switched to exponential notation.
#ifndef PRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL
#define PRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL 9
#endif

// Support for the long long integral types (with the ll, z and t length modifiers for specifiers
// %d,%i,%o,%x,%X,%u, and with the %p specifier).
#ifndef PRINTF_SUPPORT_LONG_LONG
#define PRINTF_SUPPORT_LONG_LONG 1
#endif

// The number of terms in a Taylor series expansion of log_10(x) to
// use for approximation - including the power-zero term (i.e. the
// value at the point of expansion).
#ifndef PRINTF_LOG10_TAYLOR_TERMS
#define PRINTF_LOG10_TAYLOR_TERMS 4
#endif

#if PRINTF_LOG10_TAYLOR_TERMS <= 1
#error "At least one non-constant Taylor expansion is necessary for the log10() calculation"
#endif

// Be extra-safe, and don't assume format specifiers are completed correctly
// before the format string end.
#ifndef PRINTF_CHECK_FOR_NUL_IN_FORMAT_SPECIFIER
#define PRINTF_CHECK_FOR_NUL_IN_FORMAT_SPECIFIER 1
#endif

#define PRINTF_PREFER_DECIMAL     false
#define PRINTF_PREFER_EXPONENTIAL true

///////////////////////////////////////////////////////////////////////////////

// The following will convert the number-of-digits into an exponential-notation literal
#define PRINTF_CONCATENATE(s1, s2) s1##s2
#define PRINTF_EXPAND_THEN_CONCATENATE(s1, s2) PRINTF_CONCATENATE(s1, s2)
#define PRINTF_FLOAT_NOTATION_THRESHOLD ((floating_point_t) PRINTF_EXPAND_THEN_CONCATENATE(1e,PRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL))

// internal flag definitions
#define FLAGS_ZEROPAD     (1U <<  0U)
#define FLAGS_LEFT        (1U <<  1U)
#define FLAGS_PLUS        (1U <<  2U)
#define FLAGS_SPACE       (1U <<  3U)
#define FLAGS_HASH        (1U <<  4U)
#define FLAGS_UPPERCASE   (1U <<  5U)
#define FLAGS_CHAR        (1U <<  6U)
#define FLAGS_SHORT       (1U <<  7U)
#define FLAGS_INT         (1U <<  8U)
  // Only used with PRINTF_SUPPORT_MSVC_STYLE_INTEGER_SPECIFIERS
#define FLAGS_LONG        (1U <<  9U)
#define FLAGS_LONG_LONG   (1U << 10U)
#define FLAGS_PRECISION   (1U << 11U)
#define FLAGS_ADAPT_EXP   (1U << 12U)
#define FLAGS_POINTER     (1U << 13U)
  // Note: Similar, but not identical, effect as FLAGS_HASH
#define FLAGS_SIGNED      (1U << 14U)
#define FLAGS_LONG_DOUBLE (1U << 15U)
  // Only used with PRINTF_SUPPORT_MSVC_STYLE_INTEGER_SPECIFIERS

#ifdef PRINTF_SUPPORT_MSVC_STYLE_INTEGER_SPECIFIERS

#define FLAGS_INT8 FLAGS_CHAR


#if   (SHRT_MAX   == 32767LL)
#define FLAGS_INT16       FLAGS_SHORT
#elif (INT_MAX    == 32767LL)
#define FLAGS_INT16       FLAGS_INT
#elif (LONG_MAX   == 32767LL)
#define FLAGS_INT16       FLAGS_LONG
#elif (LLONG_MAX  == 32767LL)
#define FLAGS_INT16       FLAGS_LONG_LONG
#else
#error "No basic integer type has a size of 16 bits exactly"
#endif

#if   (SHRT_MAX   == 2147483647LL)
#define FLAGS_INT32       FLAGS_SHORT
#elif (INT_MAX    == 2147483647LL)
#define FLAGS_INT32       FLAGS_INT
#elif (LONG_MAX   == 2147483647LL)
#define FLAGS_INT32       FLAGS_LONG
#elif (LLONG_MAX  == 2147483647LL)
#define FLAGS_INT32       FLAGS_LONG_LONG
#else
#error "No basic integer type has a size of 32 bits exactly"
#endif

#if   (SHRT_MAX   == 9223372036854775807LL)
#define FLAGS_INT64       FLAGS_SHORT
#elif (INT_MAX    == 9223372036854775807LL)
#define FLAGS_INT64       FLAGS_INT
#elif (LONG_MAX   == 9223372036854775807LL)
#define FLAGS_INT64       FLAGS_LONG
#elif (LLONG_MAX  == 9223372036854775807LL)
#define FLAGS_INT64       FLAGS_LONG_LONG
#else
#error "No basic integer type has a size of 64 bits exactly"
#endif

#endif // PRINTF_SUPPORT_MSVC_STYLE_INTEGER_SPECIFIERS


typedef unsigned int printf_flags_t;

#define BASE_BINARY    2
#define BASE_OCTAL     8
#define BASE_DECIMAL  10
#define BASE_HEX      16

typedef uint8_t numeric_base_t;

#if PRINTF_SUPPORT_LONG_LONG
typedef unsigned long long printf_unsigned_value_t;
typedef long long          printf_signed_value_t;
#else
typedef unsigned long printf_unsigned_value_t;
typedef long          printf_signed_value_t;
#endif

// The printf()-family functions return an `int`; it is therefore
// unnecessary/inappropriate to use size_t - often larger than int
// in practice - for non-negative related values, such as widths,
// precisions, offsets into buffers used for printing and the sizes
// of these buffers. instead, we use:
typedef unsigned int printf_size_t;
#define PRINTF_MAX_POSSIBLE_BUFFER_SIZE INT_MAX
  // If we were to nitpick, this would actually be INT_MAX + 1,
  // since INT_MAX is the maximum return value, which excludes the
  // trailing '\0'.

#if (PRINTF_SUPPORT_DECIMAL_SPECIFIERS || PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS)
#include <float.h>
#if FLT_RADIX != 2
#error "Non-binary-radix floating-point types are unsupported."
#endif

/**
 * This library supports taking float-point arguments up to and including
 * long double's; but - it currently does _not_ support internal
 * representation and manipulation of values as long doubles; the options
 * are either single-precision `float` or double-precision `double`.
 */
#if PRINTF_USE_DOUBLE_INTERNALLY
typedef double floating_point_t;
#define FP_TYPE_MANT_DIG DBL_MANT_DIG
#else
typedef float  floating_point_t;
#define FP_TYPE_MANT_DIG FLT_MANT_DIG
#endif

#define NUM_DECIMAL_DIGITS_IN_INT64_T 18

#if FP_TYPE_MANT_DIG == 24

typedef uint32_t printf_fp_uint_t;
#define FP_TYPE_SIZE_IN_BITS   32
#define FP_TYPE_EXPONENT_MASK  0xFFU
#define FP_TYPE_BASE_EXPONENT  127
#define FP_TYPE_MAX            FLT_MAX
#define FP_TYPE_MAX_10_EXP     FLT_MAX_10_EXP
#define FP_TYPE_MAX_SUBNORMAL_EXPONENT_OF_10 -38
#define FP_TYPE_MAX_SUBNORMAL_POWER_OF_10 1e-38f
#define PRINTF_MAX_PRECOMPUTED_POWER_OF_10  10

#elif FP_TYPE_MANT_DIG == 53

typedef uint64_t printf_fp_uint_t;
#define FP_TYPE_SIZE_IN_BITS   64
#define FP_TYPE_EXPONENT_MASK  0x7FFU
#define FP_TYPE_BASE_EXPONENT  1023
#define FP_TYPE_MAX            DBL_MAX
#define FP_TYPE_MAX_10_EXP     DBL_MAX_10_EXP
#define FP_TYPE_MAX_10_EXP     DBL_MAX_10_EXP
#define FP_TYPE_MAX_SUBNORMAL_EXPONENT_OF_10 -308
#define FP_TYPE_MAX_SUBNORMAL_POWER_OF_10 1e-308
#define PRINTF_MAX_PRECOMPUTED_POWER_OF_10  NUM_DECIMAL_DIGITS_IN_INT64_T - 1


#else
#error "Unsupported floating point type configuration"
#endif
#define FP_TYPE_STORED_MANTISSA_BITS (FP_TYPE_MANT_DIG - 1)

typedef union {
  printf_fp_uint_t  U;
  floating_point_t  F;
} floating_point_with_bit_access;

// This is unnecessary in C99, since compound initializers can be used,
// but:
// 1. Some compilers are finicky about this;
// 2. Some people may want to convert this to C89;
// 3. If you try to use it as C++, only C++20 supports compound literals
static inline floating_point_with_bit_access get_bit_access(floating_point_t x)
{
  floating_point_with_bit_access dwba;
  dwba.F = x;
  return dwba;
}

static inline int get_sign_bit(floating_point_t x)
{
  // The sign is stored in the highest bit
  return (int) (get_bit_access(x).U >> (FP_TYPE_SIZE_IN_BITS - 1));
}

static inline int get_exp2(floating_point_with_bit_access x)
{
  // The exponent in an IEEE-754 floating-point number occupies a contiguous
  // sequence of bits (e.g. 52..62 for 64-bit doubles), but with a non-trivial representation: An
  // unsigned offset from some negative value (with the extremal offset values reserved for
  // special use).
  return (int)((x.U >> FP_TYPE_STORED_MANTISSA_BITS ) & FP_TYPE_EXPONENT_MASK) - FP_TYPE_BASE_EXPONENT;
}
#define PRINTF_ABS(_x) ( (_x) > 0 ? (_x) : -(_x) )

#endif // (PRINTF_SUPPORT_DECIMAL_SPECIFIERS || PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS)

// Note in particular the behavior here on LONG_MIN or LLONG_MIN; it is valid
// and well-defined, but if you're not careful you can easily trigger undefined
// behavior with -LONG_MIN or -LLONG_MIN
#define ABS_FOR_PRINTING(_x) ((printf_unsigned_value_t) ( (_x) > 0 ? (_x) : -((printf_signed_value_t)_x) ))

// wrapper (used as buffer) for output function type
//
// One of the following must hold:
// 1. max_chars is 0
// 2. buffer is non-null
// 3. function is non-null
//
// ... otherwise bad things will happen.
typedef struct {
  void (*function)(char c, void* extra_arg);
  void* extra_function_arg;
  char* buffer;
  printf_size_t pos;
  printf_size_t max_chars;
} output_gadget_t;

// Note: This function currently assumes it is not passed a '\0' c,
// or alternatively, that '\0' can be passed to the function in the output
// gadget. The former assumption holds within the printf library. It also
// assumes that the output gadget has been properly initialized.
static inline void putchar_via_gadget(output_gadget_t* gadget, char c)
{
  printf_size_t write_pos = gadget->pos++;
    // We're _always_ increasing pos, so as to count how may characters
    // _would_ have been written if not for the max_chars limitation
  if (write_pos >= gadget->max_chars) {
    return;
  }
  if (gadget->function != NULL) {
    // No check for c == '\0' .
    gadget->function(c, gadget->extra_function_arg);
  }
  else {
    // it must be the case that gadget->buffer != NULL , due to the constraint
    // on output_gadget_t ; and note we're relying on write_pos being non-negative.
    gadget->buffer[write_pos] = c;
  }
}

// Possibly-write the string-terminating '\0' character
static inline void append_termination_with_gadget(output_gadget_t* gadget)
{
  if (gadget->function != NULL || gadget->max_chars == 0) {
    return;
  }
  if (gadget->buffer == NULL) {
    return;
  }
  printf_size_t null_char_pos = gadget->pos < gadget->max_chars ? gadget->pos : gadget->max_chars - 1;
  gadget->buffer[null_char_pos] = '\0';
}

// We can't use putchar_ as is, since our output gadget
// only takes pointers to functions with an extra argument
static inline void putchar_wrapper(char c, void* unused)
{
  (void) unused;
  putchar_(c);
}

static inline output_gadget_t discarding_gadget(void)
{
  output_gadget_t gadget;
  gadget.function = NULL;
  gadget.extra_function_arg = NULL;
  gadget.buffer = NULL;
  gadget.pos = 0;
  gadget.max_chars = 0;
  return gadget;
}

static inline output_gadget_t buffer_gadget(char* buffer, size_t buffer_size)
{
  printf_size_t usable_buffer_size = (buffer_size > PRINTF_MAX_POSSIBLE_BUFFER_SIZE) ?
    PRINTF_MAX_POSSIBLE_BUFFER_SIZE : (printf_size_t) buffer_size;
  output_gadget_t result = discarding_gadget();
  if (buffer != NULL) {
    result.buffer = buffer;
    result.max_chars = usable_buffer_size;
  }
  return result;
}

static inline output_gadget_t function_gadget(void (*function)(char, void*), void* extra_arg)
{
  output_gadget_t result = discarding_gadget();
  result.function = function;
  result.extra_function_arg = extra_arg;
  result.max_chars = PRINTF_MAX_POSSIBLE_BUFFER_SIZE;
  return result;
}

static inline output_gadget_t extern_putchar_gadget(void)
{
  return function_gadget(putchar_wrapper, NULL);
}

// internal secure strlen
// @return The length of the string (excluding the terminating 0) limited by 'maxsize'
// @note strlen uses size_t, but wes only use this function with printf_size_t
// variables - hence the signature.
static inline printf_size_t strnlen_s_(const char* str, printf_size_t maxsize)
{
  const char* s;
  for (s = str; *s && maxsize--; ++s);
  return (printf_size_t)(s - str);
}


// internal test if char is a digit (0-9)
// @return true if char is a digit
static inline bool is_digit_(char ch)
{
  return (ch >= '0') && (ch <= '9');
}


// internal ASCII string to printf_size_t conversion
static printf_size_t atou_(const char** str)
{
  printf_size_t i = 0U;
  while (is_digit_(**str)) {
    i = i * 10U + (printf_size_t)(*((*str)++) - '0');
  }
  return i;
}


// output the specified string in reverse, taking care of any zero-padding
static void out_rev_(output_gadget_t* output, const char* buf, printf_size_t len, printf_size_t width, printf_flags_t flags)
{
  const printf_size_t start_pos = output->pos;

  // pad spaces up to given width
  if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
    for (printf_size_t i = len; i < width; i++) {
      putchar_via_gadget(output, ' ');
    }
  }

  // reverse string
  while (len) {
    putchar_via_gadget(output, buf[--len]);
  }

  // append pad spaces up to given width
  if (flags & FLAGS_LEFT) {
    while (output->pos - start_pos < width) {
      putchar_via_gadget(output, ' ');
    }
  }
}


// Invoked by print_integer after the actual number has been printed, performing necessary
// work on the number's prefix (as the number is initially printed in reverse order)
static void print_integer_finalization(output_gadget_t* output, char* buf, printf_size_t len, bool negative, numeric_base_t base, printf_size_t precision, printf_size_t width, printf_flags_t flags)
{
  printf_size_t unpadded_len = len;

  // pad with leading zeros
  {
    if (!(flags & FLAGS_LEFT)) {
      if (width && (flags & FLAGS_ZEROPAD) && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
        width--;
      }
      while ((flags & FLAGS_ZEROPAD) && (len < width) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
        buf[len++] = '0';
      }
    }

    while ((len < precision) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
      buf[len++] = '0';
    }

    if (base == BASE_OCTAL && (len > unpadded_len)) {
      // Since we've written some zeros, we've satisfied the alternative format leading space requirement
      flags &= ~FLAGS_HASH;
    }
  }

  // handle hash
  if (flags & (FLAGS_HASH | FLAGS_POINTER)) {
    if (!(flags & FLAGS_PRECISION) && len && ((len == precision) || (len == width))) {
      // Let's take back some padding digits to fit in what will eventually
      // be the format-specific prefix
      if (unpadded_len < len) {
        len--; // This should suffice for BASE_OCTAL
      }
      if (len && (base == BASE_HEX || base == BASE_BINARY) && (unpadded_len < len)) {
        len--; // ... and an extra one for 0x or 0b
      }
    }
    if ((base == BASE_HEX) && !(flags & FLAGS_UPPERCASE) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
      buf[len++] = 'x';
    }
    else if ((base == BASE_HEX) && (flags & FLAGS_UPPERCASE) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
      buf[len++] = 'X';
    }
    else if ((base == BASE_BINARY) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
      buf[len++] = 'b';
    }
    if (len < PRINTF_INTEGER_BUFFER_SIZE) {
      buf[len++] = '0';
    }
  }

  if (len < PRINTF_INTEGER_BUFFER_SIZE) {
    if (negative) {
      buf[len++] = '-';
    }
    else if (flags & FLAGS_PLUS) {
      buf[len++] = '+';  // ignore the space if the '+' exists
    }
    else if (flags & FLAGS_SPACE) {
      buf[len++] = ' ';
    }
  }

  out_rev_(output, buf, len, width, flags);
}

// An internal itoa-like function
static void print_integer(output_gadget_t* output, printf_unsigned_value_t value, bool negative, numeric_base_t base, printf_size_t precision, printf_size_t width, printf_flags_t flags)
{
  char buf[PRINTF_INTEGER_BUFFER_SIZE];
  printf_size_t len = 0U;

  if (!value) {
    if ( !(flags & FLAGS_PRECISION) ) {
      buf[len++] = '0';
      flags &= ~FLAGS_HASH;
      // We drop this flag this since either the alternative and regular modes of the specifier
      // don't differ on 0 values, or (in the case of octal) we've already provided the special
      // handling for this mode.
    }
    else if (base == BASE_HEX) {
      flags &= ~FLAGS_HASH;
      // We drop this flag this since either the alternative and regular modes of the specifier
      // don't differ on 0 values
    }
  }
  else {
    do {
      const char digit = (char)(value % base);
      buf[len++] = (char)(digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10);
      value /= base;
    } while (value && (len < PRINTF_INTEGER_BUFFER_SIZE));
  }

  print_integer_finalization(output, buf, len, negative, base, precision, width, flags);
}

#if (PRINTF_SUPPORT_DECIMAL_SPECIFIERS || PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS)

// Stores a fixed-precision representation of a floating-point number relative
// to a fixed precision (which cannot be determined by examining this structure)
struct floating_point_components {
  int_fast64_t integral;
  int_fast64_t fractional;
    // ... truncation of the actual fractional part of the floating_point_t value, scaled
    // by the precision value
  bool is_negative;
};

static const floating_point_t powers_of_10[PRINTF_MAX_PRECOMPUTED_POWER_OF_10 + 1] = {
  1e00, 1e01, 1e02, 1e03, 1e04, 1e05, 1e06, 1e07, 1e08, 1e09, 1e10
#if PRINTF_MAX_PRECOMPUTED_POWER_OF_10 > 10
  , 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17
#endif
};

// Note: This value does not mean that all floating-point values printed with the
// library will be correct up to this precision; it is just an upper-bound for
// avoiding buffer overruns and such
#define PRINTF_MAX_SUPPORTED_PRECISION (NUM_DECIMAL_DIGITS_IN_INT64_T - 1)


// Break up a floating-point number - which is known to be a finite non-negative number -
// into its base-10 parts: integral - before the decimal point, and fractional - after it.
// Taken the precision into account, but does not change it even internally.
static struct floating_point_components get_components(floating_point_t number, printf_size_t precision)
{
  struct floating_point_components number_;
  number_.is_negative = get_sign_bit(number);
  floating_point_t abs_number = (number_.is_negative) ? -number : number;
  number_.integral = (int_fast64_t) abs_number;
  floating_point_t scaled_remainder = (abs_number - (floating_point_t) number_.integral) * powers_of_10[precision];
  number_.fractional = (int_fast64_t) scaled_remainder; // for precision == 0U, this will be 0

  floating_point_t remainder = scaled_remainder - (floating_point_t) number_.fractional;
  const floating_point_t one_half = (floating_point_t)  0.5;

  if (remainder > one_half) {
    ++number_.fractional;
    // handle rollover, e.g. case 0.99 with precision 1 is 1.0
    if ((floating_point_t) number_.fractional >= powers_of_10[precision]) {
      number_.fractional = 0;
      ++number_.integral;
    }
  }
  else if ((remainder == one_half) && (number_.fractional & 1U)) {
    // Banker's rounding, i.e. round half to even:
    // 1.5 -> 2, but 2.5 -> 2
    ++number_.fractional;
  }

  if (precision == 0U) {
    remainder = abs_number - (floating_point_t) number_.integral;
    if ((remainder == one_half) && (number_.integral & 1U)) {
      // Banker's rounding, i.e. round half to even:
      // 1.5 -> 2, but 2.5 -> 2
      ++number_.integral;
    }
  }
  return number_;
}

#if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
struct scaling_factor {
  floating_point_t raw_factor;
  bool multiply; // if true, need to multiply by raw_factor; otherwise need to divide by it
};

static floating_point_t apply_scaling(floating_point_t num, struct scaling_factor normalization)
{
  return normalization.multiply ? num * normalization.raw_factor : num / normalization.raw_factor;
}

static floating_point_t unapply_scaling(floating_point_t normalized, struct scaling_factor normalization)
{
#ifdef __GNUC__
// accounting for a static analysis bug in GCC 6.x and earlier
#pragma GCC diagnostic push
#if !defined(__has_warning)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#elif __has_warning("-Wmaybe-uninitialized")
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#endif
  return normalization.multiply ? normalized / normalization.raw_factor : normalized * normalization.raw_factor;
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
}

static struct scaling_factor update_normalization(struct scaling_factor sf, floating_point_t extra_multiplicative_factor)
{
  struct scaling_factor result;
  if (sf.multiply) {
    result.multiply = true;
    result.raw_factor = sf.raw_factor * extra_multiplicative_factor;
  }
  else {
    int factor_exp2 = get_exp2(get_bit_access(sf.raw_factor));
    int extra_factor_exp2 = get_exp2(get_bit_access(extra_multiplicative_factor));

    // Divide the larger-exponent raw raw_factor by the smaller
    if (PRINTF_ABS(factor_exp2) > PRINTF_ABS(extra_factor_exp2)) {
      result.multiply = false;
      result.raw_factor = sf.raw_factor / extra_multiplicative_factor;
    }
    else {
      result.multiply = true;
      result.raw_factor = extra_multiplicative_factor / sf.raw_factor;
    }
  }
  return result;
}

static struct floating_point_components get_normalized_components(bool negative, printf_size_t precision, floating_point_t non_normalized, struct scaling_factor normalization, int floored_exp10)
{
  struct floating_point_components components;
  components.is_negative = negative;
  floating_point_t scaled = apply_scaling(non_normalized, normalization);

  bool close_to_representation_extremum = ( (-floored_exp10 + (int) precision) >= FP_TYPE_MAX_10_EXP - 1 );
  if (close_to_representation_extremum) {
    // We can't have a normalization factor which also accounts for the precision, i.e. moves
    // some decimal digits into the mantissa, since it's unrepresentable, or nearly unrepresentable.
    // So, we'll give up early on getting extra precision...
    return get_components(negative ? -scaled : scaled, precision);
  }
  components.integral = (int_fast64_t) scaled;
  floating_point_t remainder = non_normalized - unapply_scaling((floating_point_t) components.integral, normalization);
  floating_point_t prec_power_of_10 = powers_of_10[precision];
  struct scaling_factor account_for_precision = update_normalization(normalization, prec_power_of_10);
  floating_point_t scaled_remainder = apply_scaling(remainder, account_for_precision);
  floating_point_t rounding_threshold = 0.5;

  components.fractional = (int_fast64_t) scaled_remainder; // when precision == 0, the assigned value should be 0
  scaled_remainder -= (floating_point_t) components.fractional; //when precision == 0, this will not change scaled_remainder

  components.fractional += (scaled_remainder >= rounding_threshold);
  if (scaled_remainder == rounding_threshold) {
    // banker's rounding: Round towards the even number (making the mean error 0)
    components.fractional &= ~((int_fast64_t) 0x1);
  }
  // handle rollover, e.g. the case of 0.99 with precision 1 becoming (0,100),
  // and must then be corrected into (1, 0).
  // Note: for precision = 0, this will "translate" the rounding effect from
  // the fractional part to the integral part where it should actually be
  // felt (as prec_power_of_10 is 1)
  if ((floating_point_t) components.fractional >= prec_power_of_10) {
    components.fractional = 0;
    ++components.integral;
  }
  return components;
}
#endif // PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS

static void print_broken_up_decimal(
  struct floating_point_components number_, output_gadget_t* output, printf_size_t precision,
  printf_size_t width, printf_flags_t flags, char *buf, printf_size_t len)
{
  if (precision != 0U) {
    // do fractional part, as an unsigned number

    printf_size_t count = precision;

    // %g/%G mandates we skip the trailing 0 digits...
    if ((flags & FLAGS_ADAPT_EXP) && !(flags & FLAGS_HASH) && (number_.fractional > 0)) {
      while(true) {
        int_fast64_t digit = number_.fractional % 10U;
        if (digit != 0) {
          break;
        }
        --count;
        number_.fractional /= 10U;

      }
      // ... and even the decimal point if there are no
      // non-zero fractional part digits (see below)
    }

    if (number_.fractional > 0 || !(flags & FLAGS_ADAPT_EXP) || (flags & FLAGS_HASH) ) {
      while (len < PRINTF_DECIMAL_BUFFER_SIZE) {
        --count;
        buf[len++] = (char)('0' + number_.fractional % 10U);
        if (!(number_.fractional /= 10U)) {
          break;
        }
      }
      // add extra 0s
      while ((len < PRINTF_DECIMAL_BUFFER_SIZE) && (count > 0U)) {
        buf[len++] = '0';
        --count;
      }
      if (len < PRINTF_DECIMAL_BUFFER_SIZE) {
        buf[len++] = '.';
      }
    }
  }
  else {
    if ((flags & FLAGS_HASH) && (len < PRINTF_DECIMAL_BUFFER_SIZE)) {
      buf[len++] = '.';
    }
  }

  // Write the integer part of the number (it comes after the fractional
  // since the character order is reversed)
  while (len < PRINTF_DECIMAL_BUFFER_SIZE) {
    buf[len++] = (char)('0' + (number_.integral % 10));
    if (!(number_.integral /= 10)) {
      break;
    }
  }

  // pad leading zeros
  if (!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD)) {
    if (width && (number_.is_negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
      width--;
    }
    while ((len < width) && (len < PRINTF_DECIMAL_BUFFER_SIZE)) {
      buf[len++] = '0';
    }
  }

  if (len < PRINTF_DECIMAL_BUFFER_SIZE) {
    if (number_.is_negative) {
      buf[len++] = '-';
    }
    else if (flags & FLAGS_PLUS) {
      buf[len++] = '+';  // ignore the space if the '+' exists
    }
    else if (flags & FLAGS_SPACE) {
      buf[len++] = ' ';
    }
  }

  out_rev_(output, buf, len, width, flags);
}

// internal ftoa for fixed decimal floating point
static void print_decimal_number(output_gadget_t* output, floating_point_t number, printf_size_t precision, printf_size_t width, printf_flags_t flags, char* buf, printf_size_t len)
{
  struct floating_point_components value_ = get_components(number, precision);
  print_broken_up_decimal(value_, output, precision, width, flags, buf, len);
}

#if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS

// A floor function - but one which only works for numbers whose
// floor value is representable by an int.
static int bastardized_floor(floating_point_t x)
{
  if (x >= 0) { return (int) x; }
  int n = (int) x;
  return ( ((floating_point_t) n) == x ) ? n : n-1;
}

// Computes the base-10 logarithm of the input number - which must be an actual
// positive number (not infinity or NaN, nor a sub-normal)
static floating_point_t log10_of_positive(floating_point_t positive_number)
{
  // The implementation follows David Gay (https://www.ampl.com/netlib/fp/dtoa.c).
  //
  // Since log_10 ( M * 2^x ) = log_10(M) + x , we can separate the components of
  // our input number, and need only solve log_10(M) for M between 1 and 2 (as
  // the base-2 mantissa is always 1-point-something). In that limited range, a
  // Taylor series expansion of log10(x) should serve us well enough; and we'll
  // take the mid-point, 1.5, as the point of expansion.

  floating_point_with_bit_access dwba = get_bit_access(positive_number);
  // based on the algorithm by David Gay (https://www.ampl.com/netlib/fp/dtoa.c)
  int exp2 = get_exp2(dwba);
  // drop the exponent, so dwba.F comes into the range [1,2)
  dwba.U = (dwba.U & (((printf_fp_uint_t) (1) << FP_TYPE_STORED_MANTISSA_BITS) - 1U)) |
           ((printf_fp_uint_t) FP_TYPE_BASE_EXPONENT << FP_TYPE_STORED_MANTISSA_BITS);
  floating_point_t z = (dwba.F - (floating_point_t) 1.5);
  return (
    // Taylor expansion around 1.5:
              (floating_point_t) 0.1760912590556812420           // Expansion term 0: ln(1.5)            / ln(10)
    + z     * (floating_point_t) 0.2895296546021678851 // Expansion term 1: (M - 1.5)   * 2/3  / ln(10)
#if PRINTF_LOG10_TAYLOR_TERMS > 2
    - z*z   * (floating_point_t) 0.0965098848673892950 // Expansion term 2: (M - 1.5)^2 * 2/9  / ln(10)
#if PRINTF_LOG10_TAYLOR_TERMS > 3
    + z*z*z * (floating_point_t) 0.0428932821632841311 // Expansion term 2: (M - 1.5)^3 * 8/81 / ln(10)
#endif
#endif
    // exact log_2 of the exponent x, with logarithm base change
    + (floating_point_t) exp2 * (floating_point_t) 0.30102999566398119521 // = exp2 * log_10(2) = exp2 * ln(2)/ln(10)
  );
}


static floating_point_t pow10_of_int(int floored_exp10)
{
  // A crude hack for avoiding undesired behavior with barely-normal or slightly-subnormal values.
  if (floored_exp10 == FP_TYPE_MAX_SUBNORMAL_EXPONENT_OF_10) {
    return FP_TYPE_MAX_SUBNORMAL_POWER_OF_10;
  }
  // Compute 10^(floored_exp10) but (try to) make sure that doesn't overflow
  floating_point_with_bit_access dwba;
  int exp2 = bastardized_floor((floating_point_t) (floored_exp10 * 3.321928094887362 + 0.5));
  const floating_point_t z  = (floating_point_t) (floored_exp10 * 2.302585092994046 - exp2 * 0.6931471805599453);
  const floating_point_t z2 = z * z;
  dwba.U = ((printf_fp_uint_t)(exp2) + FP_TYPE_BASE_EXPONENT) << FP_TYPE_STORED_MANTISSA_BITS;
  // compute exp(z) using continued fractions,
  // see https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
  dwba.F *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));
  return dwba.F;
}

static void print_exponential_number(output_gadget_t* output, floating_point_t number, printf_size_t precision, printf_size_t width, printf_flags_t flags, char* buf, printf_size_t len)
{
  const bool negative = get_sign_bit(number);
  // This number will decrease gradually (by factors of 10) as we "extract" the exponent out of it
  floating_point_t abs_number =  negative ? -number : number;

  int floored_exp10;
  bool abs_exp10_covered_by_powers_table;
  struct scaling_factor normalization;


  // Determine the decimal exponent
  if (abs_number == (floating_point_t) 0.0) {
    // TODO: This is a special-case for 0.0 (and -0.0); but proper handling is required for denormals more generally.
    floored_exp10 = 0; // ... and no need to set a normalization factor or check the powers table
  }
  else  {
    floating_point_t exp10 = log10_of_positive(abs_number);
    floored_exp10 = bastardized_floor(exp10);
    floating_point_t p10 = pow10_of_int(floored_exp10);
    // correct for rounding errors
    if (abs_number < p10) {
      floored_exp10--;
      p10 /= 10;
    }
    abs_exp10_covered_by_powers_table = PRINTF_ABS(floored_exp10) < PRINTF_MAX_PRECOMPUTED_POWER_OF_10;
    normalization.raw_factor = abs_exp10_covered_by_powers_table ? powers_of_10[PRINTF_ABS(floored_exp10)] : p10;
  }

  // We now begin accounting for the widths of the two parts of our printed field:
  // the decimal part after decimal exponent extraction, and the base-10 exponent part.
  // For both of these, the value of 0 has a special meaning, but not the same one:
  // a 0 exponent-part width means "don't print the exponent"; a 0 decimal-part width
  // means "use as many characters as necessary".

  bool fall_back_to_decimal_only_mode = false;
  if (flags & FLAGS_ADAPT_EXP) {
    int required_significant_digits = (precision == 0) ? 1 : (int) precision;
    // Should we want to fall-back to "%f" mode, and only print the decimal part?
    fall_back_to_decimal_only_mode = (floored_exp10 >= -4 && floored_exp10 < required_significant_digits);
    // Now, let's adjust the precision
    // This also decided how we adjust the precision value - as in "%g" mode,
    // "precision" is the number of _significant digits_, and this is when we "translate"
    // the precision value to an actual number of decimal digits.
    int precision_ = fall_back_to_decimal_only_mode ?
                     (int) precision - 1 - floored_exp10 :
        (int) precision - 1; // the presence of the exponent ensures only one significant digit comes before the decimal point
    precision = (precision_ > 0 ? (unsigned) precision_ : 0U);
    flags |= FLAGS_PRECISION;   // make sure print_broken_up_decimal respects our choice above
  }

#ifdef __GNUC__
// accounting for a static analysis bug in GCC 6.x and earlier
#pragma GCC diagnostic push
#if !defined(__has_warning)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#elif __has_warning("-Wmaybe-uninitialized")
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#endif
  normalization.multiply = (floored_exp10 < 0 && abs_exp10_covered_by_powers_table);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
  bool should_skip_normalization = (fall_back_to_decimal_only_mode || floored_exp10 == 0);
  struct floating_point_components decimal_part_components =
    should_skip_normalization ?
    get_components(negative ? -abs_number : abs_number, precision) :
    get_normalized_components(negative, precision, abs_number, normalization, floored_exp10);

  // Account for roll-over, e.g. rounding from 9.99 to 100.0 - which effects
  // the exponent and may require additional tweaking of the parts
  if (fall_back_to_decimal_only_mode) {
    if ((flags & FLAGS_ADAPT_EXP) && floored_exp10 >= -1 && decimal_part_components.integral == powers_of_10[floored_exp10 + 1]) {
      floored_exp10++; // Not strictly necessary, since floored_exp10 is no longer really used
      if (precision > 0U) { precision--; }
      // ... and it should already be the case that decimal_part_components.fractional == 0
    }
    // TODO: What about rollover strictly within the fractional part?
  }
  else {
    if (decimal_part_components.integral >= 10) {
      floored_exp10++;
      decimal_part_components.integral = 1;
      decimal_part_components.fractional = 0;
    }
  }

  // the floored_exp10 format is "E%+03d" and largest possible floored_exp10 value for a 64-bit double
  // is "307" (for 2^1023), so we set aside 4-5 characters overall
  printf_size_t exp10_part_width = fall_back_to_decimal_only_mode ? 0U : (PRINTF_ABS(floored_exp10) < 100) ? 4U : 5U;

  printf_size_t decimal_part_width =
    ((flags & FLAGS_LEFT) && exp10_part_width) ?
      // We're padding on the right, so the width constraint is the exponent part's
      // problem, not the decimal part's, so we'll use as many characters as we need:
      0U :
      // We're padding on the left; so the width constraint is the decimal part's
      // problem. Well, can both the decimal part and the exponent part fit within our overall width?
      ((width > exp10_part_width) ?
        // Yes, so we limit our decimal part's width.
        // (Note this is trivially valid even if we've fallen back to "%f" mode)
        width - exp10_part_width :
        // No; we just give up on any restriction on the decimal part and use as many
        // characters as we need
        0U);

  const printf_size_t printed_exponential_start_pos = output->pos;
  print_broken_up_decimal(decimal_part_components, output, precision, decimal_part_width, flags, buf, len);

  if (! fall_back_to_decimal_only_mode) {
    putchar_via_gadget(output, (flags & FLAGS_UPPERCASE) ? 'E' : 'e');
    print_integer(output,
                  ABS_FOR_PRINTING(floored_exp10),
                  floored_exp10 < 0, 10, 0, exp10_part_width - 1,
                FLAGS_ZEROPAD | FLAGS_PLUS);
    if (flags & FLAGS_LEFT) {
      // We need to right-pad with spaces to meet the width requirement
      while (output->pos - printed_exponential_start_pos < width) {
        putchar_via_gadget(output, ' ');
      }
    }
  }
}
#endif  // PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS

static void print_floating_point(output_gadget_t* output, floating_point_t value, printf_size_t precision, printf_size_t width, printf_flags_t flags, bool prefer_exponential)
{
  char buf[PRINTF_DECIMAL_BUFFER_SIZE];
  printf_size_t len = 0U;

  // test for special values
  if (value != value) {
    out_rev_(output, "nan", 3, width, flags);
    return;
  }
  if (value < -FP_TYPE_MAX) {
    out_rev_(output, "fni-", 4, width, flags);
    return;
  }
  if (value > FP_TYPE_MAX) {
    out_rev_(output, (flags & FLAGS_PLUS) ? "fni+" : "fni", (flags & FLAGS_PLUS) ? 4U : 3U, width, flags);
    return;
  }

  if (!prefer_exponential &&
      ((value > PRINTF_FLOAT_NOTATION_THRESHOLD) || (value < -PRINTF_FLOAT_NOTATION_THRESHOLD))) {
    // The required behavior of standard printf is to print _every_ integral-part digit -- which could mean
    // printing hundreds of characters, overflowing any fixed internal buffer and necessitating a more complicated
    // implementation.
#if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
    print_exponential_number(output, value, precision, width, flags, buf, len);
#endif
    return;
  }

  // set default precision, if not set explicitly
  if (!(flags & FLAGS_PRECISION)) {
    precision = PRINTF_DEFAULT_FLOAT_PRECISION;
  }

  // limit precision so that our integer holding the fractional part does not overflow
  while ((len < PRINTF_DECIMAL_BUFFER_SIZE) && (precision > PRINTF_MAX_SUPPORTED_PRECISION)) {
    buf[len++] = '0'; // This respects the precision in terms of result length only
    precision--;
  }

#if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
  if (prefer_exponential)
    print_exponential_number(output, value, precision, width, flags, buf, len);
  else
#endif
    print_decimal_number(output, value, precision, width, flags, buf, len);
}

#endif  // (PRINTF_SUPPORT_DECIMAL_SPECIFIERS || PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS)

// Advances the format pointer past the flags, and returns the parsed flags
// due to the characters passed
static printf_flags_t parse_flags(const char** format)
{
  printf_flags_t flags = 0U;
  do {
    switch (**format) {
      case '0': flags |= FLAGS_ZEROPAD; (*format)++; break;
      case '-': flags |= FLAGS_LEFT;    (*format)++; break;
      case '+': flags |= FLAGS_PLUS;    (*format)++; break;
      case ' ': flags |= FLAGS_SPACE;   (*format)++; break;
      case '#': flags |= FLAGS_HASH;    (*format)++; break;
      default : return flags;
    }
  } while (true);
}

static inline void format_string_loop(output_gadget_t* output, const char* format, va_list args)
{
#if PRINTF_CHECK_FOR_NUL_IN_FORMAT_SPECIFIER
#define ADVANCE_IN_FORMAT_STRING(cptr_) do { (cptr_)++; if (!*(cptr_)) return; } while(0)
#else
#define ADVANCE_IN_FORMAT_STRING(cptr_) (cptr_)++
#endif


  while (*format)
  {
    if (*format != '%') {
      // A regular content character
      putchar_via_gadget(output, *format);
      format++;
      continue;
    }
    // We're parsing a format specifier: %[flags][width][.precision][length]
    ADVANCE_IN_FORMAT_STRING(format);

    printf_flags_t flags = parse_flags(&format);

    // evaluate width field
    printf_size_t width = 0U;
    if (is_digit_(*format)) {
      width = (printf_size_t) atou_(&format);
    }
    else if (*format == '*') {
      const int w = va_arg(args, int);
      if (w < 0) {
        flags |= FLAGS_LEFT;    // reverse padding
        width = (printf_size_t)-w;
      }
      else {
        width = (printf_size_t)w;
      }
      ADVANCE_IN_FORMAT_STRING(format);
    }

    // evaluate precision field
    printf_size_t precision = 0U;
    if (*format == '.') {
      flags |= FLAGS_PRECISION;
      ADVANCE_IN_FORMAT_STRING(format);
      if (is_digit_(*format)) {
        precision = (printf_size_t) atou_(&format);
      }
      else if (*format == '*') {
        const int precision_ = va_arg(args, int);
        precision = precision_ > 0 ? (printf_size_t) precision_ : 0U;
        ADVANCE_IN_FORMAT_STRING(format);
      }
    }

    // evaluate length field
    switch (*format) {
#ifdef PRINTF_SUPPORT_MSVC_STYLE_INTEGER_SPECIFIERS
      case 'I' : {
        ADVANCE_IN_FORMAT_STRING(format);
        // Greedily parse for size in bits: 8, 16, 32 or 64
        switch(*format) {
          case '8':               flags |= FLAGS_INT8;
            ADVANCE_IN_FORMAT_STRING(format);
            break;
          case '1':
            ADVANCE_IN_FORMAT_STRING(format);
          if (*format == '6') { format++; flags |= FLAGS_INT16; }
            break;
          case '3':
            ADVANCE_IN_FORMAT_STRING(format);
            if (*format == '2') { ADVANCE_IN_FORMAT_STRING(format); flags |= FLAGS_INT32; }
            break;
          case '6':
            ADVANCE_IN_FORMAT_STRING(format);
            if (*format == '4') { ADVANCE_IN_FORMAT_STRING(format); flags |= FLAGS_INT64; }
            break;
          default: break;
        }
        break;
      }
#endif
      case 'l' :
        flags |= FLAGS_LONG;
        ADVANCE_IN_FORMAT_STRING(format);
        if (*format == 'l') {
          flags |= FLAGS_LONG_LONG;
          ADVANCE_IN_FORMAT_STRING(format);
        }
        break;
      case 'L' :
        flags |= FLAGS_LONG_DOUBLE;
        ADVANCE_IN_FORMAT_STRING(format);
        break;
      case 'h' :
        flags |= FLAGS_SHORT;
        ADVANCE_IN_FORMAT_STRING(format);
        if (*format == 'h') {
          flags |= FLAGS_CHAR;
          ADVANCE_IN_FORMAT_STRING(format);
        }
        break;
      case 't' :
        flags |= (sizeof(ptrdiff_t) <= sizeof(int) ) ? FLAGS_INT : (sizeof(ptrdiff_t) == sizeof(long)) ? FLAGS_LONG : FLAGS_LONG_LONG;
        ADVANCE_IN_FORMAT_STRING(format);
        break;
      case 'j' :
        flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
        ADVANCE_IN_FORMAT_STRING(format);
        break;
      case 'z' :
        flags |= (sizeof(size_t) <= sizeof(int) ) ? FLAGS_INT : (sizeof(size_t) == sizeof(long)) ? FLAGS_LONG : FLAGS_LONG_LONG;
        ADVANCE_IN_FORMAT_STRING(format);
        break;
      default:
        break;
    }

    // evaluate specifier
    switch (*format) {
      case 'd' :
      case 'i' :
      case 'u' :
      case 'x' :
      case 'X' :
      case 'o' :
      case 'b' : {

        if (*format == 'd' || *format == 'i') {
          flags |= FLAGS_SIGNED;
        }

        numeric_base_t base;
        if (*format == 'x' || *format == 'X') {
          base = BASE_HEX;
        }
        else if (*format == 'o') {
          base =  BASE_OCTAL;
        }
        else if (*format == 'b') {
          base =  BASE_BINARY;
        }
        else {
          base = BASE_DECIMAL;
          flags &= ~FLAGS_HASH; // decimal integers have no alternative presentation
        }

        if (*format == 'X') {
          flags |= FLAGS_UPPERCASE;
        }

        format++;
        // ignore '0' flag when precision is given
        if (flags & FLAGS_PRECISION) {
          flags &= ~FLAGS_ZEROPAD;
        }

        if (flags & FLAGS_SIGNED) {
          // A signed specifier: d, i or possibly I + bit size if enabled

          if (flags & FLAGS_LONG_LONG) {
#if PRINTF_SUPPORT_LONG_LONG
            const long long value = va_arg(args, long long);
            print_integer(output, ABS_FOR_PRINTING(value), value < 0, base, precision, width, flags);
#endif
          }
          else if (flags & FLAGS_LONG) {
            const long value = va_arg(args, long);
            print_integer(output, ABS_FOR_PRINTING(value), value < 0, base, precision, width, flags);
          }
          else {
            // We never try to interpret the argument as something potentially-smaller than int,
            // due to integer promotion rules: Even if the user passed a short int, short unsigned
            // etc. - these will come in after promotion, as int's (or unsigned for the case of
            // short unsigned when it has the same size as int)
            const int value =
              (flags & FLAGS_CHAR) ? (signed char) va_arg(args, int) :
              (flags & FLAGS_SHORT) ? (short int) va_arg(args, int) :
              va_arg(args, int);
            print_integer(output, ABS_FOR_PRINTING(value), value < 0, base, precision, width, flags);
          }
        }
        else {
          // An unsigned specifier: u, x, X, o, b

          flags &= ~(FLAGS_PLUS | FLAGS_SPACE);

          if (flags & FLAGS_LONG_LONG) {
#if PRINTF_SUPPORT_LONG_LONG
            print_integer(output, (printf_unsigned_value_t) va_arg(args, unsigned long long), false, base, precision, width, flags);
#endif
          }
          else if (flags & FLAGS_LONG) {
            print_integer(output, (printf_unsigned_value_t) va_arg(args, unsigned long), false, base, precision, width, flags);
          }
          else {
            const unsigned int value =
              (flags & FLAGS_CHAR) ? (unsigned char)va_arg(args, unsigned int) :
              (flags & FLAGS_SHORT) ? (unsigned short int)va_arg(args, unsigned int) :
              va_arg(args, unsigned int);
            print_integer(output, (printf_unsigned_value_t) value, false, base, precision, width, flags);
          }
        }
        break;
      }
#if PRINTF_SUPPORT_DECIMAL_SPECIFIERS
      case 'f' :
      case 'F' : {
        floating_point_t value = (floating_point_t) (flags & FLAGS_LONG_DOUBLE ? va_arg(args, long double) : va_arg(args, double));
        if (*format == 'F') flags |= FLAGS_UPPERCASE;
        print_floating_point(output, value, precision, width, flags, PRINTF_PREFER_DECIMAL);
        format++;
        break;
      }
#endif
#if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
      case 'e':
      case 'E':
      case 'g':
      case 'G': {
        floating_point_t value = (floating_point_t) (flags & FLAGS_LONG_DOUBLE ? va_arg(args, long double) : va_arg(args, double));
        if ((*format == 'g')||(*format == 'G')) flags |= FLAGS_ADAPT_EXP;
        if ((*format == 'E')||(*format == 'G')) flags |= FLAGS_UPPERCASE;
        print_floating_point(output, value, precision, width, flags, PRINTF_PREFER_EXPONENTIAL);
        format++;
        break;
      }
#endif  // PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
      case 'c' : {
        printf_size_t l = 1U;
        // pre padding
        if (!(flags & FLAGS_LEFT)) {
          while (l++ < width) {
            putchar_via_gadget(output, ' ');
          }
        }
        // char output
        putchar_via_gadget(output, (char) va_arg(args, int) );
        // post padding
        if (flags & FLAGS_LEFT) {
          while (l++ < width) {
            putchar_via_gadget(output, ' ');
          }
        }
        format++;
        break;
      }

      case 's' : {
        const char* p = va_arg(args, char*);
        if (p == NULL) {
          out_rev_(output, ")llun(", 6, width, flags);
        }
        else {
          printf_size_t l = strnlen_s_(p, precision ? precision : PRINTF_MAX_POSSIBLE_BUFFER_SIZE);
          // pre padding
          if (flags & FLAGS_PRECISION) {
            l = (l < precision ? l : precision);
          }
          if (!(flags & FLAGS_LEFT)) {
            while (l++ < width) {
              putchar_via_gadget(output, ' ');
            }
          }
          // string output
          while ((*p != 0) && (!(flags & FLAGS_PRECISION) || precision)) {
            putchar_via_gadget(output, *(p++));
            --precision;
          }
          // post padding
          if (flags & FLAGS_LEFT) {
            while (l++ < width) {
              putchar_via_gadget(output, ' ');
            }
          }
        }
        format++;
        break;
      }

      case 'p' : {
        width = sizeof(void*) * 2U + 2; // 2 hex chars per byte + the "0x" prefix
        flags |= FLAGS_ZEROPAD | FLAGS_POINTER;
        uintptr_t value = (uintptr_t)va_arg(args, void*);
        (value == (uintptr_t) NULL) ?
          out_rev_(output, ")lin(", 5, width, flags) :
          print_integer(output, (printf_unsigned_value_t) value, false, BASE_HEX, precision, width, flags);
        format++;
        break;
      }

      case '%' :
        putchar_via_gadget(output, '%');
        format++;
        break;

      // Many people prefer to disable support for %n, as it lets the caller
      // engineer a write to an arbitrary location, of a value the caller
      // effectively controls - which could be a security concern in some cases.
#if PRINTF_SUPPORT_WRITEBACK_SPECIFIER
      case 'n' : {
        if       (flags & FLAGS_CHAR)      *(va_arg(args, char*))      = (char) output->pos;
        else if  (flags & FLAGS_SHORT)     *(va_arg(args, short*))     = (short) output->pos;
        else if  (flags & FLAGS_LONG)      *(va_arg(args, long*))      = (long) output->pos;
#if PRINTF_SUPPORT_LONG_LONG
        else if  (flags & FLAGS_LONG_LONG) *(va_arg(args, long long*)) = (long long int) output->pos;
#endif // PRINTF_SUPPORT_LONG_LONG
        else                               *(va_arg(args, int*))       = (int) output->pos;
        format++;
        break;
      }
#endif // PRINTF_SUPPORT_WRITEBACK_SPECIFIER

      default :
        putchar_via_gadget(output, *format);
        format++;
        break;
    }
  }
}

// internal vsnprintf - used for implementing _all library functions
static int vsnprintf_impl(output_gadget_t* output, const char* format, va_list args)
{
  // Note: The library only calls vsnprintf_impl() with output->pos being 0. However, it is
  // possible to call this function with a non-zero pos value for some "remedial printing".
  format_string_loop(output, format, args);

  // termination
  append_termination_with_gadget(output);

  // return written chars without terminating \0
  return (int)output->pos;
}

///////////////////////////////////////////////////////////////////////////////

int vprintf_(const char* format, va_list arg)
{
  output_gadget_t gadget = extern_putchar_gadget();
  return vsnprintf_impl(&gadget, format, arg);
}

int vsnprintf_(char* s, size_t n, const char* format, va_list arg)
{
  output_gadget_t gadget = buffer_gadget(s, n);
  return vsnprintf_impl(&gadget, format, arg);
}

int vsprintf_(char* s, const char* format, va_list arg)
{
  return vsnprintf_(s, PRINTF_MAX_POSSIBLE_BUFFER_SIZE, format, arg);
}

int vfctprintf(void (*out)(char c, void* extra_arg), void* extra_arg, const char* format, va_list arg)
{
  if (out == NULL) { return 0; }
  output_gadget_t gadget = function_gadget(out, extra_arg);
  return vsnprintf_impl(&gadget, format, arg);
}

int printf_(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  const int ret = vprintf_(format, args);
  va_end(args);
  return ret;
}

int sprintf_(char* s, const char* format, ...)
{
  va_list args;
  va_start(args, format);
  const int ret = vsprintf_(s, format, args);
  va_end(args);
  return ret;
}

int snprintf_(char* s, size_t n, const char* format, ...)
{
  va_list args;
  va_start(args, format);
  const int ret = vsnprintf_(s, n, format, args);
  va_end(args);
  return ret;
}

int fctprintf(void (*out)(char c, void* extra_arg), void* extra_arg, const char* format, ...)
{
  va_list args;
  va_start(args, format);
  const int ret = vfctprintf(out, extra_arg, format, args);
  va_end(args);
  return ret;
}

