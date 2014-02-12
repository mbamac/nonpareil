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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "display.h"
#include "digit_ops.h"
#include "utils.h"
#include "voyager_lcd.h"
#include "proc_nut.h"


static void voyager_op_display_off (cpu_t *nut_reg, int opcode)
{
	voyager_display_reg_t *display = nut_reg->display_chip;
	
	display->enable = 0;
	display->blink = 0;
}


static void voyager_op_display_toggle (cpu_t *nut_reg, int opcode)
{
	voyager_display_reg_t *display = nut_reg->display_chip;
	
	display->enable = ! display->enable;
}


static void voyager_op_display_blink (cpu_t *nut_reg, int opcode)
{
	voyager_display_reg_t *display = nut_reg->display_chip;
	
	display->enable = 1;
	display->blink = 1;
	display->blink_state = 1;
	display->blink_count = VOYAGER_DISPLAY_BLINK_DIVISOR;
	//display->count = 0;  // force immediate display update
}


static void voyager_display_init_ops (cpu_t *nut_reg)
{	
	//nut_reg->display_digits = VOYAGER_DISPLAY_DIGITS;
	nut_reg->op_fcn [0x030] = voyager_op_display_blink;
	nut_reg->op_fcn [0x2e0] = voyager_op_display_off;
	nut_reg->op_fcn [0x320] = voyager_op_display_toggle;
}


void voyager_display_reset (cpu_t *nut_reg)
{
    voyager_display_reg_t *display = nut_reg->display_chip;
    
	display->enable = 0;
	display->blink = 0;
}



// For each of 11 digits, we need segments a-g for the actual digit,
// segment h for the decimal point, segment i for the tail of the comma,
// and segment j for the annunciator.


void voyager_display_sleep(cpu_t *nut_reg)
{
    voyager_display_reg_t *display = nut_reg->display_chip;
	
    if (display->enable)
    {
        /* going to light sleep */
#ifdef AUTO_POWER_OFF
        // $$$ how does display timer work on Voyager?
        /* start display timer if LCD chip is selected */
        if (nut_reg->pf_addr == PFADDR_LCD_DISPLAY)
            display->timer = DISPLAY_TIMEOUT;
#endif /* AUTO_POWER_OFF */
    }
    else
    /* going to deep sleep */
        nut_reg->carry = 1;
}

void voyager_display_init (cpu_t *nut_reg)
{	
	voyager_display_reg_t *display;
	
	voyager_display_init_ops (nut_reg);
	
	display = (voyager_display_reg_t *)alloc (sizeof (voyager_display_reg_t));
	
	nut_reg->display_chip = display;
}
