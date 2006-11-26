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

#include "libtime.h"

static time_t	g_ps2time_start		= 0;
static u64	g_interruptCount	= 0;
static int	g_interruptID		= -1;
static s32	g_ps2time_timezone	= -1;
static char	g_ps2time_tzname[2][64];

/*
	ps2time_init - initializes ps2time library
	must be called before making any calls
	to other ps2time functions
*/

int ps2time_init( void )
{
	CdvdClock_t clock;
	struct tm	tm_time;

	static unsigned int monthdays[12] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};

	// query the RTC
	cdInit(CDVD_INIT_NOCHECK);
	if( !cdReadClock(&clock) )
		return -1;

	tm_time.tm_sec	= BCD2DEC(clock.second);
	tm_time.tm_min	= BCD2DEC(clock.minute);
	tm_time.tm_hour = BCD2DEC(clock.hour); // hour in Japan (GMT + 9)
	tm_time.tm_mday	= BCD2DEC(clock.day);
	tm_time.tm_mon	= BCD2DEC(clock.month) - 1; // RTC months range from 1 - 12
	tm_time.tm_year	= BCD2DEC(clock.year) + 100; // RTC returns year less 2000

	if( tm_time.tm_hour < 9 ) {
		// go back to last month
		if( tm_time.tm_mday == 1 ) {

			// go back to last year if month is january
			if( tm_time.tm_mon == 0 ) {
				tm_time.tm_mon = 11;
				tm_time.tm_year = tm_time.tm_year - 1;
			}
			else {
				tm_time.tm_mon = tm_time.tm_mon - 1;
			}

			// if going back to february and year is a leapyear
			if( tm_time.tm_mon == 1 && IS_LEAP_YEAR(tm_time.tm_year + 1900)) {
				tm_time.tm_mday = 29;
			}
			else {
				tm_time.tm_mday = monthdays[ tm_time.tm_mon ];
			}

		}
		else {
			tm_time.tm_mday = tm_time.tm_mday - 1;
		}
		tm_time.tm_hour = (24 - (9 - tm_time.tm_hour)) % 24;
	}
	else {
		tm_time.tm_hour -= 9;
	}

	g_ps2time_start = ps2time_mktime(&tm_time);

	// setup EE timer1
	*T1_MODE = 0x0000;

	// enable the overflow interrupt handler
	g_interruptID = AddIntcHandler(T1_INTC, ps2time_intr_handler, 0);
	EnableIntc(T1_INTC);
	g_interruptCount = 0;

	*T1_COUNT	= 0;
	*T1_MODE	= Tn_MODE(0x02, 0, 0, 0, 0, 0x01, 0, 0x01, 0, 0);

	// set the timezone as offset in minutes from GMT
	ps2time_setTimezone( configGetTimezone() );

	return 0;
}

/*
	ps2time_intr_handler
	time interrupt callback
*/

int	ps2time_intr_handler( int ca )
{
	g_interruptCount++;
	*T1_MODE |= (1 << 11);
	
	return -1;
}

/*
	ps2time_time - returns the number of seconds
	elapsed since 00:00 hours, Jan 1, 1970 UTC.
*/

time_t ps2time_time( time_t *t )
{
	struct timeval tv;

	ps2time_gettimeofday( &tv, NULL );

	if( t )
		*t = tv.tv_sec;

	return tv.tv_sec;
}

/*
	ps2time_time_msec - returns the number of milliseconds
	elapsed since 00:00 hours, Jan 1, 1970 UTC.
*/

u64 ps2time_time_msec( u64 *t )
{
	struct timeval tv;
	u64 ret;

	ps2time_gettimeofday( &tv, NULL );
	ret = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	if( t )
		*t = ret;

	return ret;
}

/*
	ps2time_gettimeofday - retrieves current time expressed
	as seconds and microseconds since the epoch.
*/

int ps2time_gettimeofday( struct timeval *tv, void *tzp )
{
	u64 temp;

	temp = *T1_COUNT + (g_interruptCount << 16);
	temp = temp * 1000000 / kBUSCLKBY256;

	tv->tv_sec = g_ps2time_start + temp / 1000000;
	tv->tv_usec = temp % 1000000;

	return 0;
}

/*
	ps2time_mktime - Based on implementation by C.A. Lademann
	<cal@zls.com> Corrections by richard.kettlewell@kewill.com
*/

time_t ps2time_mktime(struct tm *t)
{
	struct tm *u;
	time_t  epoch = 0;
	int n;
	int             mon [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	y, m, i;

	if(t->tm_year < 70)
		return((time_t)-1);

	n = t->tm_year + 1900 - 1;
	epoch = (t->tm_year - 70) * YEAR + 
		((n / 4 - n / 100 + n / 400) - (1969 / 4 - 1969 / 100 + 1969 / 400)) * DAY;

	y = t->tm_year + 1900;
	m = 0;

	for(i = 0; i < t->tm_mon; i++) {
		epoch += mon [m] * DAY;
		if(m == 1 && y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))
			epoch += DAY;
    
		if(++m > 11) {
			m = 0;
			y++;
		}
	}

	epoch += (t->tm_mday - 1) * DAY;
	epoch += t->tm_hour * HOUR + t->tm_min * MINUTE + t->tm_sec;

	if( (u = ps2time_localtime(&epoch)) != NULL ) {
		t->tm_sec	= u->tm_sec;
		t->tm_min	= u->tm_min;
		t->tm_hour	= u->tm_hour;
		t->tm_mday	= u->tm_mday;
		t->tm_mon	= u->tm_mon;
		t->tm_year	= u->tm_year;
		t->tm_wday	= u->tm_wday;
		t->tm_yday	= u->tm_yday;
		t->tm_isdst	= u->tm_isdst;
	}
	
	return(epoch);
}


/*
	ps2time_gmtime - Converts timer to tm structure adjusting
	to UTC. Based on gmtime implementation from RTLGnat.
*/

struct tm *ps2time_gmtime( const time_t *timep )
{
	static struct tm ret, *r;

	/* days per month -- nonleap! */
	const short __spm[12] = {
		0,
		(31),
		(31+28),
		(31+28+31),
		(31+28+31+30),
		(31+28+31+30+31),
		(31+28+31+30+31+30),
		(31+28+31+30+31+30+31),
		(31+28+31+30+31+30+31+31),
		(31+28+31+30+31+30+31+31+30),
		(31+28+31+30+31+30+31+31+30+31),
		(31+28+31+30+31+30+31+31+30+31+30),
	};
	
	time_t i;
	register time_t work = *timep % (SPD);
	r = &ret;

	r->tm_sec	= work %60; work /= 60;
	r->tm_min	= work %60;
	r->tm_hour	= work /60;

	work = *timep / (SPD);

	r->tm_wday = (4 + work) % 7;

	for( i=1970; ; ++i ) {
		register time_t k = IS_LEAP_YEAR(i) ? 366 : 365;
		
		if( work >= k )
			work -= k;
		else
			break;
	}
	
	r->tm_year	= i - 1900;
	r->tm_yday	= work;
	r->tm_mday	= 1;

	if(IS_LEAP_YEAR(i) && (work > 58)) {
		if( work == 59 )
			r->tm_mday = 2; /* 29.2. */
		
		work -= 1;
	}
	
	for( i = 11; i && (__spm[i] > work); --i);

	r->tm_mon	=i;
	r->tm_mday	+= work - __spm[i];
	
	return r;
}

/*
	ps2time_localtime - Converts timer to tm structure
	adjusting to the local timezone.
*/

struct tm *ps2time_localtime( const time_t *timep )
{	
	time_t t = *timep + g_ps2time_timezone * 60;

	return ps2time_gmtime(&t);
}

/*
	ps2time_asctime - Based on FreeBSD implementation by
	Arthur David Olson (arthur_david_olson@nih.gov)
*/

char *ps2time_asctime( const struct tm *timeptr )
{
	static char result[128];
	static const char	wday_name[][3] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	static const char	mon_name[][3] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	/*
	** Big enough for something such as
	** ??? ???-2147483648 -2147483648:-2147483648:-2147483648 -2147483648\n
	** (two three-character abbreviations, five strings denoting integers,
	** three explicit spaces, two explicit colons, a newline,
	** and a trailing ASCII nul).
	*/
	register const char *wn;
	register const char *mn;

	if( timeptr->tm_wday < 0 || timeptr->tm_wday >= 7 )
		wn = "???";
	else
		wn = wday_name[timeptr->tm_wday];
	
	if( timeptr->tm_mon < 0 || timeptr->tm_mon >= 12 )
		mn = "???";
	else
		mn = mon_name[timeptr->tm_mon];

	/*
	** The X3J11-suggested format is
	**	"%.3s %.3s%3d %02.2d:%02.2d:%02.2d %d\n"
	** Since the .2 in 02.2d is ignored, we drop it.
	*/
	(void) sprintf(result, "%.3s %.3s%3d %02d:%02d:%02d %d\n",
		wn, mn,
		timeptr->tm_mday, timeptr->tm_hour,
		timeptr->tm_min, timeptr->tm_sec,
		TM_YEAR_BASE + timeptr->tm_year);

	return result;
}

/*
	ps2time_ctime - Converts time_t value to string.
*/

char *ps2time_ctime( const time_t *timer )
{
	/* The C Standard says ctime (t) is equivalent to asctime (localtime (t)).
	In particular, ctime and asctime must yield the same pointer. */
	return ps2time_asctime( ps2time_localtime(timer) );
}

/*
	ps2time_difftime - Returns difference between two times.
*/

double ps2time_difftime( time_t t1, time_t t0 )
{
	return (double)(t1 - t0);
}

/*
	ps2time_strftime - print formatted information about a given time.
	Adapted from the routine by Eric R. Smith, Michal Jaegermann,
	Arnold Robins, and Paul Close.
*/

#undef LOCAL_DDMMYY		/* choose DD/MM/YY instead of MM/DD/YY */
#undef LOCAL_DOTTIME	/* choose HH.MM.SS instead of HH:MM:SS */

size_t ps2time_strftime( char *str, size_t maxsize, const char *fmt, const struct tm *ts )
{
	static char *mth_name[] = {
	   "January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
	};

	static char *day_name[] = {
		"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
		"Saturday"
	};


	size_t num = 0, len;
	char ch;
	char *putstr;
	char tmpbuf[80];

	if (maxsize-- <= 0)
		return 0;

	for (;;) {
		if (!(ch = *fmt++))
			break;
		
		if (num == maxsize) {
			num = 0;
			break;
		}
		
		if (ch != '%') {
			*str++ = ch;
			num++;
			continue;
		}
		
		/* assume the finished product will be sprintf'ed into tmpbuf */
		putstr = tmpbuf;

		switch (ch = *fmt++) {
		case 'A':
		case 'a':
			if (ts->tm_wday < 0 || ts->tm_wday > 6)
				putstr = "?";
			else
				if (ch == 'A')
					putstr = day_name[ts->tm_wday];
				else
					sprintf(tmpbuf, "%-.3s", day_name[ts->tm_wday]);
				break;
		case 'B':
		case 'b':
		case 'h':
			if (ts->tm_mon < 0 || ts->tm_mon > 11)
				putstr = "?";
			else if (ch == 'B')
				putstr = mth_name[ts->tm_mon];
			else
				sprintf(tmpbuf, "%-.3s", mth_name[ts->tm_mon]);
			break;
		case 'C':
			ps2time_strftime(tmpbuf, sizeof tmpbuf, "%A, %B %e, %Y", ts);
			break;
		case 'c':
			ps2time_strftime(tmpbuf, sizeof tmpbuf, "%x %X", ts);
			break;
		case 'D':
#ifndef LOCAL_DDMMYY
		case 'x':
#endif
			ps2time_strftime(tmpbuf, sizeof tmpbuf, "%m/%d/%y", ts);
			break;
		case 'd':
			sprintf(tmpbuf, "%02d", ts->tm_mday);
			break;
		case 'e':	/* day of month, blank padded */
			sprintf(tmpbuf, "%2d", ts->tm_mday);
			break;
		case 'H':
			sprintf(tmpbuf, "%02d", ts->tm_hour);
			break;
		case 'I':
			{
				int n;

				n = ts->tm_hour;
				if (n == 0)
					n = 12;
				else if (n > 12)
					n -= 12;
				sprintf(tmpbuf, "%02d", n);
				break;
			}
		case 'j':
			sprintf(tmpbuf, "%03d", ts->tm_yday + 1);
			break;
		case 'm':
			sprintf(tmpbuf, "%02d", ts->tm_mon + 1);
			break;
		case 'M':
			sprintf(tmpbuf, "%02d", ts->tm_min);
			break;
		case 'p':
			putstr = (ts->tm_hour < 12) ? "AM" : "PM";
			break;
		case 'r':
			ps2time_strftime(tmpbuf, sizeof tmpbuf, "%I:%M:%S %p", ts);
			break;
		case 'R':
			ps2time_strftime(tmpbuf, sizeof tmpbuf, "%H:%M", ts);
			break;
		case 'S':
			sprintf(tmpbuf, "%02d", ts->tm_sec);
			break;
		case 'T':
#ifndef LOCAL_DOTTIME
		case 'X':
#endif
			ps2time_strftime(tmpbuf, sizeof tmpbuf, "%H:%M:%S", ts);
			break;
		case 'U':	/* week of year - starting Sunday */
			sprintf(tmpbuf, "%02d", (ts->tm_yday - ts->tm_wday + 10) / 7);
			break;
		case 'W':	/* week of year - starting Monday */
			sprintf(tmpbuf, "%02d", (ts->tm_yday - ((ts->tm_wday + 6) % 7)+ 10) / 7);
			break;
		case 'w':
			sprintf(tmpbuf, "%d", ts->tm_wday);
			break;
		case 'y':
			sprintf(tmpbuf, "%02d", ts->tm_year % 100);
			break;
#ifdef LOCAL_DOTTIME
		case 'X':
			ps2time_strftime(tmpbuf, sizeof tmpbuf, "%H.%M.%S", ts);
			break;
#endif
#ifdef LOCAL_DDMMYY
		case 'x':
			ps2time_strftime(tmpbuf, sizeof tmpbuf, "%d/%m/%y", ts);
			break;
#endif
		case 'Y':
			sprintf(tmpbuf, "%d", ts->tm_year + 1900);
			break;
		case 'Z':
			sprintf(tmpbuf, "%s", g_ps2time_tzname[ts->tm_isdst % 1]);
			break;
		case '%':
		case '\0':
			putstr = "%";
			break;
		case 'n':	/* same as \n */
			putstr = "\n";
			break;
		case 't':	/* same as \t */
			putstr = "\t";
			break;
		default:
			sprintf(tmpbuf, "%%%c", ch);
			break;
	}
	len = strlen(putstr);
	num += len;
	if (num > maxsize) {
		len -= num - maxsize;
		num = 0;
		ch = '\0';
	}
	strncpy(str, putstr, len);
	str += len;
	if (!ch)
		break;
	}
	*str = '\0';
	return num;
}

/*
	ps2time_setTimezone - sets the timezone as offset in minutes
	from GMT.
*/

void ps2time_setTimezone( s32 offset )
{
	g_ps2time_timezone = offset;

	snprintf( g_ps2time_tzname[0], sizeof(g_ps2time_tzname[0]),
		"GMT%+.1f", g_ps2time_timezone / 60.0f );

	g_ps2time_tzname[1][0] = 0;
}
