/*
oofnd/Compiler.h

What GCC and Clang predefine and cl.exe spells differently. Included by
oofnd/PrintfFormat.h, which every oofnd and game unit reaches.
*/

#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
// The function's full signature, as GCC's and Clang's __PRETTY_FUNCTION__.
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

// OO_BUILTIN_UNREACHABLE(): tells the optimiser control never gets here.
#if defined(__GNUC__) || defined(__clang__)
#define OO_BUILTIN_UNREACHABLE() __builtin_unreachable()
#else
#define OO_BUILTIN_UNREACHABLE() __assume(0)
#endif
