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
//  SpiceSimulator.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-26.
//  Copyright 2005-2012 Maciej Bartosiak.
//

#import <Cocoa/Cocoa.h>
#import "display.h"
#import "LEDDisplayView.h"

#ifdef NONPAREIL_32E

#define NNPR_OBJ	@"32e"
#define NNPR_STATE	@"state.nonpareil-32e"
#define NNPR_RAM	32
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_33C

#define NNPR_OBJ	@"33c"
#define NNPR_STATE	@"state.nonpareil-33c"
#define NNPR_RAM	32
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_34C

#define NNPR_OBJ	@"34c"
#define NNPR_STATE	@"state.nonpareil-34c"
#define NNPR_RAM	64
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_37E

#define NNPR_OBJ	@"37e"
#define NNPR_STATE	@"state.nonpareil-37e"
#define NNPR_RAM	48
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_38C

#define NNPR_OBJ	@"38c"
#define NNPR_STATE	@"state.nonpareil-38c"
#define NNPR_RAM	48
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_38E

#define NNPR_OBJ	@"38e"
#define NNPR_STATE	@"state.nonpareil-38e"
#define NNPR_RAM	48
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#else

#error No type defined

#endif

#import "display.h"
#import "digit_ops.h"
#import "utils.h"
#import "proc_woodstock.h"

@interface SpiceSimulator : NSObject
{
	NSTimeInterval	lastRun;
    LEDDisplayView *display;
    
	cpu_t *cpu;
	
    segment_bitmap_t display_segments [MAX_DIGIT_POSITION];
}

- (id)init;
- (void)pressKey: (int)key;
- (void)readKeysFrom: (NSMutableArray *) keyQueue;
- (void)executeCycle;
- (BOOL)displayScan;
- (BOOL)getFlag: (int)num;
- (void)setFlag: (int)num withBool: (BOOL)state;
- (int)displayDigits;
- (segment_bitmap_t *)displaySegments;
- (void)saveState;
- (void)readState;

@property (nonatomic, strong) LEDDisplayView *display;

@end
