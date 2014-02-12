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

#ifndef utils_h
#define utils_h

#define WSIZE 14
#define EXPSIZE 3  // two exponent and one exponent sign digit

typedef digit_t reg_t [WSIZE];
typedef uint32_t addr_t;
typedef uint16_t rom_word_t;

void usage (FILE *f);
void fatal (int ret, char *format, ...);
void *alloc (size_t size);
void warning (char *format, ...);
void trim_trailing_whitespace (char *s);
size_t fread_bytes  (FILE *stream,
					 void *ptr,
					 size_t byte_count,
					 bool *eof,
					 bool *error);

char* reg2str (char *str, reg_t reg);
void str2reg(reg_t reg, const char *str);
void *alloc(size_t size);

#endif
