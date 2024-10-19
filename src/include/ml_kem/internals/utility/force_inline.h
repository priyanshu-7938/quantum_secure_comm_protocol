#ifndef FORCE_INLINE_H
#define FORCE_INLINE_H

// Following content is adapted from https://github.com/itzmeanjan/raccoon/blob/bfa45f9f22ea7b98f5d6588a8513ff4182af79ca/include/raccoon/internals/utility/force_inline.hpp

#ifdef _MSC_VER
// MSVC
#define forceinline __forceinline

#elif defined(__GNUC__)
// GCC
#define forceinline __attribute__((always_inline)) inline

#elif defined(__clang__)
// Clang
#if __has_attribute(always_inline)
#define forceinline __attribute__((always_inline)) inline
#else
#define forceinline inline
#endif

#else
// Others
#define forceinline inline

#endif

#endif // FORCE_INLINE_H