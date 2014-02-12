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
//  VoyagerSimulator.m
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-09-09.
//  Copyright 2005-2012 Maciej Bartosiak
//

#import "VoyagerSimulator.h"
#import <math.h>

@implementation VoyagerSimulator

@synthesize display;

- (id)init
{
    self = [super init];
	
	NSBundle *nonpareilBundle = [NSBundle mainBundle];
	NSString *objFile = [nonpareilBundle pathForResource: NNPR_OBJ ofType:@"obj"];
    
	cpu = nut_new_processor (NNPR_RAM);
	nut_read_object_file (cpu, [objFile UTF8String]);
	
	[self readState];
	lastRun = [NSDate timeIntervalSinceReferenceDate];
	
	return self;
}

- (void)pressKey: (int)key
{
	if (key == -1)
	{
		nut_release_key(cpu);
	} else {
		nut_press_key(cpu, key);
	}
}

- (void)readKeysFrom: (NSMutableArray *) keyQueue
{
	static int delay = 0;
	int key;
	
	if (delay)
		delay--;
	else
	{
		if([keyQueue lastObject])
		{
			key = [[keyQueue lastObject] intValue];
			[keyQueue removeLastObject];
            
			[self pressKey: key];
            
            if (key == -1)
            {
                if([keyQueue lastObject])
                {
                    key = [[keyQueue lastObject] intValue];
                    [keyQueue removeLastObject];
                    [self pressKey: key];
                    delay = 2;
                }
            }
		}
	}	
}

- (void)executeCycle
{
	NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
	int i = (int)round((now - lastRun) * (NNPR_CLOCK / NNPR_WSIZE));
	lastRun = now;
	
	if (i > 5000) i = 5000;
	
	while (i--) {
		nut_execute_instruction(cpu);
    }
    if ([self displayScan])
        [display setNeedsDisplay:YES];
}

typedef struct
{
	//int reg;
	int dig;
	int bit;
} voyager_segment_info_t;

- (BOOL)displayScan
{
    voyager_segment_info_t voyager_display_map [11] [10] =
    {
        /* leftmost position has only segment g for a minus */
        {{  0, 0 }, {  0, 0 }, {  0, 0 }, {  0, 0 }, {  0, 0 }, {  0, 0 }, { 11, 4 }, {  0, 0 }, {  0, 0 }, {  0, 0 }},  // no annunciator
        {{  5, 2 }, {  5, 8 }, {  4, 8 }, { 11, 8 }, {  4, 4 }, {  5, 1 }, {  5, 4 }, {  9, 8 }, {  9, 4 }, {  0, 0 }},  // no annunciator - "*" for low bat in KML, but that's not controllable by the calculator microcode
        {{  6, 8 }, {  7, 2 }, {  6, 2 }, {  4, 2 }, {  6, 1 }, {  6, 4 }, {  7, 1 }, {  3, 8 }, {  3, 4 }, {  4, 1 }},  // USER annunciator
        {{ 12, 8 }, { 13, 2 }, { 12, 2 }, {  3, 2 }, { 12, 1 }, { 12, 4 }, { 13, 1 }, { 13, 8 }, { 13, 4 }, {  3, 1 }},  // f annunciator
        {{  8, 2 }, {  8, 8 }, {  7, 8 }, {  2, 2 }, {  7, 4 }, {  8, 1 }, {  8, 4 }, {  9, 2 }, {  9, 1 }, {  2, 1 }},  // g annunciator
        {{ 10, 8 }, { 11, 2 }, { 10, 2 }, {  1, 8 }, { 10, 1 }, { 10, 4 }, { 11, 1 }, {  2, 8 }, {  2, 4 }, {  1, 4 }},  // BEGIN annunciator
        {{  2, 8 }, {  3, 2 }, {  2, 2 }, {  3, 8 }, {  2, 1 }, {  2, 4 }, {  3, 1 }, {  4, 2 }, {  4, 1 }, {  3, 4 }},  // G annunciator (for GRAD, or overflow on 16C)
        {{  5, 2 }, {  5, 8 }, {  4, 8 }, {  1, 8 }, {  4, 4 }, {  5, 1 }, {  5, 4 }, {  6, 2 }, {  6, 1 }, {  1, 4 }},  // RAD annunciator
        {{  7, 2 }, {  7, 8 }, {  6, 8 }, {  9, 8 }, {  6, 4 }, {  7, 1 }, {  7, 4 }, {  9, 2 }, {  9, 1 }, {  9, 4 }},  // D.MY annunciator
        {{ 11, 8 }, { 12, 2 }, { 11, 2 }, {  8, 2 }, { 11, 1 }, { 11, 4 }, { 12, 1 }, {  8, 8 }, {  8, 4 }, {  8, 1 }},  // C annunciator (Complex on 15C, Carry on 16C)
        {{ 13, 2 }, { 13, 8 }, { 12, 8 }, { 10, 2 }, { 12, 4 }, { 13, 1 }, { 13, 4 }, { 10, 8 }, { 10, 4 }, { 10, 1 }},  // PRGM annunciator        
    };
    
	voyager_display_reg_t *dsp = cpu->display_chip;
    
	int digit;
	int segment;
	int vreg, vdig, vbit;
    
    BOOL need_update = NO;
    
	for (digit = 0; digit < VOYAGER_DISPLAY_DIGITS; digit++)
    {
        segment_bitmap_t segs = 0;
		//display_segments [digit] = 0;
		if (dsp->enable &&
			((! dsp->blink) || (dsp->blink_state)))
		{
            vreg = (digit < 6)? 9 : 10;
			for (segment = 0; segment <= 9; segment++)
			{
                //vreg = voyager_display_map [digit][segment].reg;
                vdig = voyager_display_map [digit][segment].dig;
                vbit = voyager_display_map [digit][segment].bit;
                
                //printf("d=%2d s=%2d   vd=%2d vb=%2d\n",digit,segment,vdig,vbit);
                
				if (vbit && (cpu->ram [vreg][vdig] & vbit))
				{
					if (segment < 9)
						//display_segments [digit] |= (1 << segment);
                        segs |= (1 << segment);
					else
						//display_segments [digit] |= SEGMENT_ANN;
                        segs |= SEGMENT_ANN;
				}
			}
		}
        if (display_segments [digit] == segs) {
            need_update = YES;
        }
        display_segments [digit] = segs;
    }
	
	if (dsp->blink)
    {
		dsp->blink_count--;
		if (! dsp->blink_count)
		{
			dsp->blink_state ^= 1;
			dsp->blink_count = VOYAGER_DISPLAY_BLINK_DIVISOR;
		}
    }
    
    return need_update;
}

- (int)displayDigits
{
	return VOYAGER_DISPLAY_DIGITS;
}

- (segment_bitmap_t *)displaySegments
{
	return display_segments;
}

- (NSString *)calculatorStateFilename {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSArray *urls = [fileManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask];
	
    if ([urls count] == 0)
        return nil;
	
	NSString *nonpareilDirPath = [[urls objectAtIndex:0] path];
	nonpareilDirPath = [nonpareilDirPath stringByAppendingPathComponent:@"nonpareil"];
	NSError *error;
	
	BOOL success = [fileManager createDirectoryAtPath:nonpareilDirPath
						  withIntermediateDirectories:YES
										   attributes:nil
												error:&error];
    if (!success) 
		return nil;
	
    
	return [nonpareilDirPath stringByAppendingPathComponent:NNPR_STATE];
}

- (void)readState
{
    NSDictionary *stateDict = [[NSMutableDictionary alloc] initWithContentsOfFile:[self calculatorStateFilename]];
    if (stateDict == nil) {
#ifdef NONPAREIL_25
		//woodstock_set_ext_flag(cpu,3,true);
#endif
        return;
    }
    
    NSUInteger i;
	
    str2reg(cpu->a, [[stateDict objectForKey:@"a"] UTF8String]);
    str2reg(cpu->b, [[stateDict objectForKey:@"b"] UTF8String]);
    str2reg(cpu->c, [[stateDict objectForKey:@"c"] UTF8String]);
    str2reg(cpu->m, [[stateDict objectForKey:@"m"] UTF8String]);
    str2reg(cpu->n, [[stateDict objectForKey:@"n"] UTF8String]);
    
    cpu->g[0]       = (digit_t)[[stateDict objectForKey:@"g0"] unsignedIntValue];
	cpu->g[1]       = (digit_t)[[stateDict objectForKey:@"g1"] unsignedIntValue];
	
    cpu->p          = (digit_t)[[stateDict objectForKey:@"p"] unsignedIntValue];
	cpu->q          = (digit_t)[[stateDict objectForKey:@"q"] unsignedIntValue];
	
	cpu->q_sel		= (bool)[[stateDict objectForKey:@"q_sel"] boolValue];
	
	cpu->fo         = (digit_t)[[stateDict objectForKey:@"fo"] unsignedIntValue];
    
    cpu->decimal    = (bool)[[stateDict objectForKey:@"decimal"] boolValue];
    cpu->carry      = (bool)[[stateDict objectForKey:@"carry"] boolValue];
	cpu->prev_carry	= (bool)[[stateDict objectForKey:@"prev_carry"] boolValue];
	
	cpu->prev_tef_last = [[stateDict objectForKey:@"prev_tef_last"] intValue];
    
    cpu->s          = (uint16_t)[[stateDict objectForKey:@"s"] unsignedIntValue];
    cpu->ext_flag   = (uint16_t)[[stateDict objectForKey:@"ext_flag"] unsignedIntValue];
    
    cpu->pc         = (uint16_t)[[stateDict objectForKey:@"pc"] unsignedIntValue];
    
    for(i=0; i<STACK_DEPTH; i++)
        cpu->stack[i] = (uint16_t)[[[stateDict objectForKey:@"stack"] objectAtIndex: i] unsignedIntValue]; //poprawić i na NSUint
	
    cpu->cxisa_addr	= (uint16_t)[[stateDict objectForKey:@"cxisa_addr"] unsignedIntValue];
    cpu->inst_state	= (inst_state_t)[[stateDict objectForKey:@"inst_state"] unsignedIntValue];
	cpu->first_word	= (uint16_t)[[stateDict objectForKey:@"first_word"] unsignedIntValue];
	cpu->long_branch_carry  = (bool)[[stateDict objectForKey:@"long_branch_carry"] boolValue];
    
	//bool key_down;      /* true while a key is down */
	//keyboard_state_t kb_state;
	//int kb_debounce_cycle_counter;
	//int key_buf;        /* most recently pressed key */
	
	/*cpu->key_down	= (bool)[[stateDict objectForKey:@"key_down"] boolValue];
	cpu->kb_state	= (keyboard_state_t)[[stateDict objectForKey:@"kb_state"] intValue];
	cpu->kb_debounce_cycle_counter = (int)[[stateDict objectForKey:@"kb_debounce_cycle_counter"] intValue];
	cpu->key_buf	= (int)[[stateDict objectForKey:@"key_buf"] intValue];*/
    
	cpu->awake			= (bool)[[stateDict objectForKey:@"awake"] boolValue];
    
    //memory
    
    cpu->ram_addr = (uint16_t)[[stateDict objectForKey:@"ram_addr"] unsignedIntValue];
    
    for(i=0; i<cpu->max_ram; i++) {
		//if (cpu->ram_exists[i])
			str2reg(cpu->ram[i], [[[stateDict objectForKey:@"memory"] objectAtIndex: i] UTF8String]); //poprawić i na NSUint
    }
	
	cpu->display_chip->enable		= (bool)[[stateDict objectForKey:@"display_chip->enable"] boolValue];	
	cpu->display_chip->blink		= (bool)[[stateDict objectForKey:@"display_chip->blink"] boolValue];
	cpu->display_chip->blink_state	= (bool)[[stateDict objectForKey:@"display_chip->blink_state"] boolValue];
	cpu->display_chip->blink_count	= (int)[[stateDict objectForKey:@"display_chip->blink_count"] intValue];
}

- (void)saveState
{
    NSMutableDictionary *stateDict = [[NSMutableDictionary alloc] init];
    char tmp[WSIZE+1] ;
    NSUInteger i;
    
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->a)] forKey:@"a"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->b)] forKey:@"b"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->c)] forKey:@"c"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->n)] forKey:@"n"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->m)] forKey:@"m"];
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->g[0]]
				 forKey:@"g0"];
	[stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->g[1]]
				 forKey:@"g1"];
	
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->p]
                 forKey:@"p"];
	[stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->q]
                 forKey:@"q"];
	[stateDict setValue:[NSNumber numberWithBool:cpu->q_sel]
                 forKey:@"q_sel"];
	
	[stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->fo]
                 forKey:@"fo"];
    
    [stateDict setValue:[NSNumber numberWithBool:cpu->decimal]
                 forKey:@"decimal"];
	
    [stateDict setValue:[NSNumber numberWithBool:cpu->carry]
                 forKey:@"carry"];
	[stateDict setValue:[NSNumber numberWithBool:cpu->prev_carry]
                 forKey:@"prev_carry"];
	
	[stateDict setValue:[NSNumber numberWithInt: cpu->prev_tef_last]
                 forKey:@"prev_tef_last"];
	
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->s]
                 forKey:@"s"];
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->ext_flag]
                 forKey:@"ext_flag"];
	
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->pc]
                 forKey:@"pc"];
    
	NSMutableArray *stack = [[NSMutableArray alloc] init];
    
    for(i=0; i<STACK_DEPTH; i++)
        [stack insertObject: [NSNumber numberWithUnsignedInt: cpu->stack[i]] atIndex:i];
    
    [stateDict setValue:stack
                 forKey:@"stack"];
	
	[stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->cxisa_addr]
                 forKey:@"cxisa_addr"];
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->inst_state]
                 forKey:@"inst_state"];
	[stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->first_word]
                 forKey:@"first_word"];
    [stateDict setValue:[NSNumber numberWithBool:cpu->long_branch_carry]
                 forKey:@"long_branch_carry"];
	
	
	//bool key_down;      /* true while a key is down */
	//keyboard_state_t kb_state;
	//int kb_debounce_cycle_counter;
	//int key_buf;        /* most recently pressed key */
	
	/*[stateDict setValue:[NSNumber numberWithBool: cpu->key_down]
				 forKey:@"key_down"];
	[stateDict setValue:[NSNumber numberWithInt: cpu->kb_state]
				 forKey:@"kb_state"];
	[stateDict setValue:[NSNumber numberWithInt: cpu->kb_debounce_cycle_counter]
				 forKey:@"kb_debounce_cycle_counter"];
	[stateDict setValue:[NSNumber numberWithInt: cpu->key_buf]
				 forKey:@"key_buf"];*/
    
	[stateDict setValue:[NSNumber numberWithBool:cpu->awake]
                 forKey:@"awake"];
	
    //memory
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->ram_addr]
                 forKey:@"ram_addr"];
    
    NSMutableArray *memory = [[NSMutableArray alloc] init];
	
    for(i=0; i<cpu->max_ram; i++)
		[memory insertObject: [NSString stringWithUTF8String:reg2str(tmp, cpu->ram[i])] atIndex:i];
    
    [stateDict setValue:memory
                 forKey:@"memory"];
    
	[stateDict setValue:[NSNumber numberWithBool: cpu->display_chip->enable]
				 forKey:@"display_chip->enable"];
	[stateDict setValue:[NSNumber numberWithBool: cpu->display_chip->blink]
				 forKey:@"display_chip->blink"];
	[stateDict setValue:[NSNumber numberWithBool: cpu->display_chip->blink_state]
				 forKey:@"display_chip->blink_state"];
	[stateDict setValue:[NSNumber numberWithInt: cpu->display_chip->blink_count]
				 forKey:@"display_chip->blink_count"];
	
	
    [stateDict writeToFile:[self calculatorStateFilename] atomically:YES];
    
}

@end
