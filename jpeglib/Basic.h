//
//	This has been modified from Dennis Lee's original version
//	

/*
 * File: basic.h
 *
 * Header file for #defs and typedefs I often use
 *
 * Copyright (C) 1992-1997 Dennis Lee
 */

#ifndef BASIC_H
#define BASIC_H

//typedef enum {FALSE, TRUE}  boolean;

#define schar               signed char
#define sshort              signed short
#define slong               signed long
#define uchar               unsigned char
#define ushort              unsigned short
#define ulong               unsigned long

#ifndef NULL
#define NULL                0
#endif
#ifndef SEEK_SET
#define SEEK_SET            0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR            1
#endif
#ifndef SEEK_END
#define SEEK_END            2
#endif

#define READ_TYPE           "rb"
#define WRITE_TYPE          "wb"

#define IMPORT              extern
#define EXPORT
#define LOCAL               static
#define GLOBAL

#endif
