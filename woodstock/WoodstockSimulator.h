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
//  WoodstockSimulator.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-12-26.
//  Copyright 2005-2012 Maciej Bartosiak
//

#import <Cocoa/Cocoa.h>
#import "display.h"
#import "LEDDisplayView.h"

#ifdef NONPAREIL_21

#define NNPR_OBJ	@"21"
#define NNPR_STATE	@"state.nonpareil-21"
#define NNPR_RAM	0
#define NNPR_CLOCK	185000
#define NNPR_WSIZE	56.0

#elif NONPAREIL_25

#define NNPR_OBJ	@"25"
#define NNPR_STATE	@"state.nonpareil-25"
#define NNPR_RAM	16
#define NNPR_CLOCK	185000
#define NNPR_WSIZE	56.0

#else

#error No type defined

#endif

#import "display.h"
#import "digit_ops.h"
#import "utils.h"
#import "proc_woodstock.h"

@interface WoodstockSimulator : NSObject
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
