#pragma once


/*
 *	General C-Types
 */
typedef unsigned char 	    uint8_t;
typedef unsigned short 	    uint16_t;
typedef unsigned int 	    uint32_t;
typedef unsigned long long 	uint64_t;


typedef signed char 	    int8_t;
typedef signed short 	    int16_t;
typedef signed int 		    int32_t;
typedef signed long long	int64_t;


typedef unsigned char       u_char;

typedef uint8_t             color_t;
typedef unsigned int        size_t;
typedef int                 pid_t;
typedef int64_t             ino_t;
typedef int64_t             off_t;
typedef int                 dev_t;
typedef int                 mode_t;
typedef int                 nlink_t;
typedef int                 uid_t;
typedef int                 gid_t;
typedef int                 blksize_t;
typedef int64_t             blkcnt_t;

#define time_t              int64_t
#define NULL                0

 
/*
 *	Interruption handler
 */
typedef void (*int_handler)(void);


