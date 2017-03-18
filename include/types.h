#ifndef __TYPES_H__
#define __TYPES_H__

// $Id: types.h 49 2006-09-20 22:03:44Z jlieder $

// Integer Types (for Intel-based machines)

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int int16;
typedef unsigned short int uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;

// Generic Types (architecture independent)

typedef unsigned char byte;
typedef unsigned int  uint;	// a simple unsigned integer
typedef unsigned long word;	// integer of machine word size
typedef unsigned long iptr;	// integer with pointer precision
typedef unsigned long size;

// Generic Types (Intel notation)

typedef uint8  ia_byte;		// BYTE
typedef uint16 ia_word;		// WORD
typedef uint32 ia_dword;	// DWORD
typedef uint64 ia_qword;	// QWORD

// Other Types

typedef char *strz;		// Zero-terminated character string
typedef int bool;		// Boolean value

#define NULL	((void * const) 0)

#define FALSE	0
#define TRUE	1

#endif
