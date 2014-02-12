/*
 $Id$
 Copyright 1995, 2003, 2004, 2005 Eric L. Smith <eric@brouhaha.com>
 
 Nonpareil is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.  Note that I am not
 granting permission to redistribute or modify Nonpareil under the
 terms of any later version of the General Public License.
 
 Nonpareil is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program (in the file "COPYING"); if not, write to the
 Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 MA 02111, USA.
 */

//
// any changes since 0.77 copyright 2005-2012 Maciej Bartosiak
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <strings.h>

#include "digit_ops.h"

#include "utils.h"

void trim_trailing_whitespace (char *s)
{
	int i;
	char c;
	
	i = (int)strlen (s);
	while (--i >= 0)
    {
		c = s [i];
		if ((c == '\n') || (c == '\r') || (c == ' ') || (c == '\t'))
			s [i] = '\0';
		else
			break;
    }
}

size_t fread_bytes  (FILE *stream,
					 void *ptr,
					 size_t byte_count,
					 bool *eof,
					 bool *error)
{
	size_t total = 0;
	
	*eof = false;
	*error = false;
	
	while (byte_count)
    {
		size_t count;
		
		count = fread (ptr, 1, byte_count, stream);
		ptr += count;
		total += count;
		byte_count -= count;
		if (byte_count)
		{
			if (ferror (stream))
			{
				*error = true;
				return total;
			}
			if (feof (stream))
			{
				*eof = true;
				return total;
			}
		}
    }
	return total;
}

void fatal (int ret, char *format, ...)
{
	va_list ap;
	
	if (format)
    {
		fprintf (stderr, "fatal error: ");
		va_start (ap, format);
		vfprintf (stderr, format, ap);
		va_end (ap);
    }
	//if (ret == 1)
	//	usage (stderr);
	exit (ret);
}

char* reg2str (char *str, reg_t reg)
{
    static char hexmap[] = "0123456789abcdef";
	int i = WSIZE - 1;
    char *tmp = str;
	
    do {
        *tmp++ = hexmap[reg[i]];
    } while (i--);
    *tmp = 0;
    return str;
}


void str2reg(reg_t reg, const char *str)
{
    int cchr;
    int i = WSIZE - 1;
    
    do {
        cchr = *str++;
        
        if((cchr >= 'a') && (cchr <= 'f')) // a,b...f
            reg[i] = cchr - 'a' + 10;
        else if((cchr >= 'A') && (cchr <= 'F')) // a,b...f
            reg[i] = cchr - 'A' + 10;
        else if(cchr >= 0x30) //1, 2...
            reg[i] = cchr - '0';
        else {
            printf("warning: wrong code\n");
            reg[i] = 0;
        }
    } while (i--);
}


void *alloc (size_t size)
{
	void *addr;
	
    addr = malloc (size);
    memset(addr, 0, size);
	return addr;
}
