/**
 * Definitions of general application relevance.
 *
 * @file
 */
#ifndef KDT_DEF_H
#define KDT_DEF_H

///{ Core definitions.

#ifndef KDT_ALPHA
/// Desired number of parallel contact requests during node lookup procedure.
#define KDT_ALPHA 3
#endif

#ifndef KDT_B
/// Number of bits used by a Kademlia ID.
#define KDT_B 160
#endif

#ifndef KDT_B1
/// Maximum number of buckets to store in routing table.
#define KDT_B1 KDT_B
#endif

#ifndef KDT_B8
/// Number of bytes used by a Kademlia ID.
#define KDT_B8 (KDT_B/8)
#endif

#ifndef KDT_K
/// Number of contacts in a bucket.
#define KDT_K 20
#endif

#ifndef KDT_THREADS
/// Number of worker threads used.
#define KDT_THREADS 4
#endif

#ifndef KDT_N_BACKLOG
/// Highest number of allowed pending network connections.
#define KDT_N_BACKLOG 24
#endif

#ifndef KDT_N_BUFFER_I_COUNT
/// Maximum number of pending inbound network messages.
#define KDT_N_BUFFER_I_COUNT 128
#endif

#ifndef KDT_N_BUFFER_O_COUNT
/// Maximum number of pending outbound network messages.
#define KDT_N_BUFFER_O_COUNT 256
#endif

#ifndef KDT_N_BUFFER_SIZE
/// Maximum size of inbound or outbound network message.
#define KDT_N_BUFFER_SIZE 65536
#endif

#ifndef KDT_T_EXPIRE
/// Time, in seconds, after which a stored key/value pair expires.
#define KDT_T_EXPIRE 86410
#endif

#ifndef KDT_T_REFRESH
/// Time, in seconds, after which an unaccessed bucket must be refreshed.
#define KDT_T_REFRESH 3600
#endif

#ifndef KDT_T_REPLICATE
/// Interval, in seconds, at which stored data is sent to KDM_K closest nodes.
#define KDT_T_REPLICATE 3600
#endif

#ifndef KDT_T_REPUBLISH
/// Interval, in seconds, at which an original data publisher must republish it.
#define KDT_T_REPUBLISH 86400
#endif

#if KDT_B % 8 != 0
#error KDT_B must be a multiple of 8.
#endif

#if KDT_B1 > KDT_B
#error KDT_B1 must be smaller than or equal to KDT_B.
#endif

//#if KDT_THREADS < 1
//#error KDT_THREADS must be at least 1.
//#endif

#if KDT_T_EXPIRE <= KDT_T_REPUBLISH
#error KDT_T_EXPIRE must be larger than KDT_T_REPUBLISH.
#endif

///}

///{ Platform definitions.

#ifdef KDT_USE_POSIX
#include <unistd.h>
#endif

#if defined(__unix__) || defined(__unix)
#define KDT_USE_VT100
#endif

#if defined(__GNUC__) && __GNUC__ >= 5

#define KDT_GCC5_BUILTINS

#define KDT_ENDIANESS __BYTE_ORDER__
#define KDT_ENDIANESS_BIG __ORDER_BIG_ENDIAN__
#define KDT_ENDIANESS_LITTLE __ORDER_LITTLE_ENDIAN__
#define KDT_SIZEOF_INT __SIZEOF_INT__

#else

#ifndef KDT_ENDIANESS
#define KDT_ENDIANESS
#endif
#define KDT_ENDIANESS_BIG
#define KDT_ENDIANESS_LITTLE
#ifndef KDT_SIZEOF_INT
#define KDT_SIZEOF_INT 4
#endif

#include <assert.h>
static_assert(sizeof(int) == KDT_SIZEOF_INT, "KDT_SIZEOF_INT invalid.");

#endif

///}

///{Terminal-related definitions.

#ifdef KDT_USE_VT100
#define COLOR_BLACK   "\x1b[30m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"
#define COLOR_DEFAULT "\x1b[39m"
#else
#define COLOR_BLACK
#define COLOR_RED
#define COLOR_GREEN
#define COLOR_YELLOW
#define COLOR_BLUE
#define COLOR_MAGENTA
#define COLOR_CYAN
#define COLOR_WHITE
#define COLOR_DEFAULT
#endif

#define BLACK(STRING)   COLOR_BLACK   STRING COLOR_DEFAULT
#define RED(STRING)     COLOR_RED     STRING COLOR_DEFAULT
#define GREEN(STRING)   COLOR_GREEN   STRING COLOR_DEFAULT
#define YELLOW(STRING)  COLOR_YELLOW  STRING COLOR_DEFAULT
#define BLUE(STRING)    COLOR_BLUE    STRING COLOR_DEFAULT
#define MAGENTA(STRING) COLOR_MAGENTA STRING COLOR_DEFAULT
#define CYAN(STRING)    COLOR_CYAN    STRING COLOR_DEFAULT
#define WHITE(STRING)   COLOR_WHITE   STRING COLOR_DEFAULT


///}

#endif