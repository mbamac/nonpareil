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
//  ClassicSimulator.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-08.
//  Copyright 2005-2012 Maciej Bartosiak.
//

#import <Cocoa/Cocoa.h>
#import "LEDDisplayView.h"
#import "display.h"

#ifdef NONPAREIL_35

#define NNPR_OBJ	@"35"
#define NNPR_STATE	@"state.nonpareil-35"
#define NNPR_RAM	0
#define NNPR_CLOCK	196000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_45

#define NNPR_OBJ	@"45"
#define NNPR_STATE	@"state.nonpareil-45"
#define NNPR_RAM	10
#define NNPR_CLOCK	196000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_55

#define NNPR_OBJ	@"55"
#define NNPR_STATE	@"state.nonpareil-55"
#define NNPR_RAM	60
#define NNPR_CLOCK	196000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_80

#define NNPR_OBJ	@"80"
#define NNPR_STATE	@"state.nonpareil-80"
#define NNPR_RAM	0
#define NNPR_CLOCK	196000.0
#define NNPR_WSIZE	56.0

#else

#error No type defined

#endif

#import "display.h"
#import "digit_ops.h"
#import "utils.h"
#import "proc_classic.h"

@interface ClassicSimulator : NSObject
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
- (void)setFlag: (int)num withBool: (BOOL)state;
- (int)displayDigits;
- (segment_bitmap_t *)displaySegments;
- (void)saveState;
- (void)readState;

@property (nonatomic, strong) LEDDisplayView *display;

@end
