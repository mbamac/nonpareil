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
//  VoyagerSimulator.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-09-09.
//  Copyright 2005-2012 Maciej Bartosiak
//

#import <Cocoa/Cocoa.h>
#import "VoyagerDisplayView.h"
#import "display.h"

#ifdef NONPAREIL_11C

#define NNPR_OBJ	@"11c"
#define NNPR_STATE	@"state.nonpareil-11c"
#define NNPR_RAM	40
#define NNPR_CLOCK	215000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_12C

#define NNPR_OBJ	@"12c"
#define NNPR_STATE	@"state.nonpareil-12c"
#define NNPR_RAM	40
#define NNPR_CLOCK	215000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_15C

#define NNPR_OBJ	@"15c"
#define NNPR_STATE	@"state.nonpareil-15c"
#define NNPR_RAM	80
#define NNPR_CLOCK	215000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_16C

#define NNPR_OBJ	@"16c"
#define NNPR_STATE	@"state.nonpareil-16c"
#define NNPR_RAM	40
#define NNPR_CLOCK	215000.0
#define NNPR_WSIZE	56.0

#else

#error No type defined

#endif

#import "display.h"
#import "digit_ops.h"
#import "utils.h"
#import "voyager_lcd.h"
#import "proc_nut.h"

@interface VoyagerSimulator : NSObject
{
	NSTimeInterval	lastRun;
	VoyagerDisplayView *display;
    
	cpu_t *cpu;
	
	segment_bitmap_t display_segments [MAX_DIGIT_POSITION];
}
- (id)init;
- (void)pressKey: (int)key;
- (void)readKeysFrom: (NSMutableArray *) keyQueue;
- (void)executeCycle;
- (BOOL)displayScan;
- (int)displayDigits;
- (segment_bitmap_t *)displaySegments;
- (void)saveState;
- (void)readState;

@property (nonatomic, strong) VoyagerDisplayView *display;

@end
