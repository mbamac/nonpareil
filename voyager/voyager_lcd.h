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

#define VOYAGER_DISPLAY_DIGITS 11
#define VOYAGER_DISPLAY_BLINK_DIVISOR 10//45 //150

// The Voyager display doesn't have a peripheral address like the
// Coconut display, but we have to pick a chip number somehow, so we'll
// use the same one.
//#define PFADDR_LCD_DISPLAY 0xfd

struct nut_reg_t;

typedef struct
{
	bool enable;
	//int count;
	
	bool blink;
	bool blink_state;
	int blink_count;
} voyager_display_reg_t;

void voyager_display_reset (struct nut_reg_t *nut_reg);
void voyager_display_sleep(struct nut_reg_t *nut_reg);
void voyager_display_init (struct nut_reg_t *nut_reg);
