#ifndef _TYPES_H
#define _TYPES_H

#if 0

#elif defined __GNUC__

//GNU C++

typedef long long int Uint64;
typedef int           Uint32;
typedef short int     Uint16;
typedef char          Uint8;

typedef long long int Sint64;
typedef int           Sint32;
typedef short int     Sint16;
typedef char          Sint8;
/*typedef uint64_t Uint64;
typedef uint32_t Uint32;
typedef uint16_t Uint16;
typedef uint8_t  Uint8;
typedef int64_t Sint64;
typedef int32_t Sint32;
typedef int16_t Sint16;
typedef int8_t  Sint8;*/

#elif defined _MSC_VER

//Micosoft Visual C++

typedef unsigned __int64 Uint64;
typedef unsigned __int32 Uint32;
typedef unsigned __int16 Uint16;
typedef unsigned __int8  Uint8;
typedef signed __int64 Sint64;
typedef signed __int32 Sint32;
typedef signed __int16 Sint16;
typedef signed __int8  Sint8;

#else

//unrecognized platform
//will guess type names
//and double-check in types.cpp

typedef Uint64 unsigned __int64;
typedef Uint32 unsigned __int32;
typedef Uint16 unsigned __int16;
typedef Uint8  unsigned __int8;
typedef Sint64 signed __int64;
typedef Sint32 signed __int32;
typedef Sint16 signed __int16;
typedef Sint8  signed __int8;

#endif
#endif
