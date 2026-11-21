#pragma once

#ifndef GCC_ATTR
#ifdef __GNUC__
#define GCC_ATTR(x) __attribute__(x)
#else
#define GCC_ATTR(x)
#endif
#endif

#if !OO_DEBUG
// Force inlining of function
#define ALWAYS_INLINE_FUNC GCC_ATTR((always_inline))
#else
// Don't force inlining of function (because gdb is silly)
#define ALWAYS_INLINE_FUNC
#endif

// result dependent only on params and globals
#define PURE_FUNC GCC_ATTR((pure))
// pure + no pointer dereferences or globals
#define CONST_FUNC GCC_ATTR((const))
#define NONNULL_FUNC GCC_ATTR((nonnull))  // Pointer parameters may not be NULL
#define DEPRECATED_FUNC GCC_ATTR((deprecated))  // Warn if this function is used
#define NO_RETURN_FUNC GCC_ATTR((noreturn))     // Function can never return
#define NO_INLINE_FUNC GCC_ATTR((noinline))  // Function must never be inlined

#define INLINE_PURE_FUNC ALWAYS_INLINE_FUNC PURE_FUNC
#define INLINE_CONST_FUNC ALWAYS_INLINE_FUNC CONST_FUNC
