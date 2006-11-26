/*
=================================================================
ps2time - a time library for the ps2
Copyright (C) 2006 Torben "ntba2" Koenke

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
MA  02110-1301  USA
=================================================================
*/

#ifndef _LIBTIME_H_
#define _LIBTIME_H_


#ifdef __cplusplus 
extern "C" { 
#endif 


#include <time.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <libcdvd.h>
#include <osd_config.h>

#define BCD2DEC(bcd)		(((((bcd)>>4) & 0x0F) * 10) + ((bcd) & 0x0F)) 
#define DEC2BCD(dec)		((((dec)/10)<<4)|((dec)%10))

#define IS_LEAP_YEAR(Y)     ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )

#define	MINUTE			60
#define	HOUR			60  * MINUTE
#define	DAY			24  * HOUR
#define	YEAR			365 * DAY
#define SPD			24 * 60 * 60	// seconds per day

#if !defined(TM_YEAR_BASE)
	#define TM_YEAR_BASE	1900
#endif

#define kBUSCLK			(147456000)
#define kBUSCLKBY256		(kBUSCLK / 256)

#define T1_INTC			10
#define T1_COUNT		((volatile unsigned long*)0x10000800)
#define T1_MODE			((volatile unsigned long*)0x10000810)

#define Tn_MODE(CLKS,GATE,GATS,GATM,ZRET,CUE,CMPE,OVFE,EQUF,OVFF) \
	(u32)((u32)(CLKS)  | ((u32)(GATE) << 2) |	\
	((u32)(GATS) << 3)  | ((u32)(GATM) << 4) |	\
	((u32)(ZRET) << 6)  | ((u32)(CUE) << 7)  |	\
	((u32)(CMPE) << 8)  | ((u32)(OVFE) << 9) |	\
	((u32)(EQUF) << 10) | ((u32)(OVFF) << 11))

#ifndef TIMEVAL
#define TIMEVAL
struct timeval {
 long    tv_sec;         /* seconds */
 long    tv_usec;        /* and microseconds */
};
#endif

int		ps2time_intr_handler( int ca );
int		ps2time_init( void );

time_t		ps2time_time( time_t *t );
u64		ps2time_time_msec( u64 *t );
time_t		ps2time_mktime( struct tm *t );
struct tm	*ps2time_gmtime( const time_t *timep );
struct tm	*ps2time_localtime( const time_t *timep );
char		*ps2time_asctime( const struct tm *timeptr );
char		*ps2time_ctime( const time_t *timer );
double		ps2time_difftime( time_t t1, time_t t0 );
size_t		ps2time_strftime( char *str, size_t maxsize, const char *fmt, const struct tm *ts );
int		ps2time_gettimeofday( struct timeval *tv, void *tzp );
void		ps2time_setTimezone( s32 offset );

#ifdef __cplusplus 
} 
#endif 

#endif // _LIBPS2TIME_H_
