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
//  WoodstockSimulator.m
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-12-26.
//  Copyright 2005-2012 Maciej Bartosiak
//

#import "WoodstockSimulator.h"
//#import <math.h>

@implementation WoodstockSimulator

@synthesize display;

- (id)init
{
    self = [super init];
	
	NSBundle *nonpareilBundle = [NSBundle mainBundle];
	NSString *objFile = [nonpareilBundle pathForResource: NNPR_OBJ ofType:@"obj"];
	
	cpu = woodstock_new_processor (NNPR_RAM);
	woodstock_read_object_file (cpu, [objFile UTF8String]);

	[self readState];
	lastRun = [NSDate timeIntervalSinceReferenceDate];
	
	return self;
}

- (void)pressKey: (int)key
{
	if (key == -1)
	{
		woodstock_release_key(cpu);
	} else {
		woodstock_press_key(cpu, key);
	}
}

- (void)readKeysFrom: (NSMutableArray *) keyQueue
{
	//static int delay = 1;
	int key;
	
	//if (delay)
	//	delay--;
	//else
	//{
		if([keyQueue lastObject])
		{
			key = [[keyQueue lastObject] intValue];
			[keyQueue removeLastObject];
			[self pressKey: key];
			//delay = 1;
		}		
	//}	
}

- (void)executeCycle
{
	NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
	int i = (int)round((now - lastRun) * (NNPR_CLOCK / NNPR_WSIZE));
	lastRun = now;
	
	if (i > 5000) i = 5000;
	
	while (i--)
		woodstock_execute_instruction(cpu);
    if ([self displayScan])
        [display setNeedsDisplay:YES];
}

- (BOOL)getFlag: (int)num
{
    return woodstock_get_ext_flag(cpu, num);
}

- (void)setFlag: (int)num withBool: (BOOL)state
{
	woodstock_set_ext_flag(cpu, num, (bool)state);
}

- (BOOL)displayScan
{
    static segment_bitmap_t char_gen[] =
    //	0    1    2	   3	4    5    6    7    8    9    r    H    o    P    E
    {  63,   6,  91,  79, 102, 109, 125,   7, 127, 111,  80, 113,  92, 115, 121,   0, // H->F 118->113
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		//                                                              ,    -    .
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 384,  64, 128,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 };
	
	int display_position = 0;
	int display_scan_position;// = WSIZE - 1;
    BOOL need_update = NO;
	
    // From WOODSTOCK_LEFT_SCAN (WSIZE-1) = 13 to 2
	for (display_scan_position = WOODSTOCK_LEFT_SCAN; display_scan_position >= WOODSTOCK_RIGHT_SCAN; display_scan_position--) {
		segment_bitmap_t segs = 0;
		int a = cpu->a [display_scan_position];
		int b = cpu->b [display_scan_position];
		
		if (cpu->display_enable) {
			if (b & 2) {
				if ((a >= 2) && ((a & 7) != 7))
					segs = char_gen ['-'];
			}
			else
				segs = char_gen [a];
			
			if (b & 1)
				segs |= char_gen ['.'];
		}
		
        if (display_segments[display_position] != segs)
            need_update = YES;
        
		display_segments [display_position++] = segs;
	}
    
    return need_update;
}

- (int)displayDigits
{
	return WOODSTOCK_DISPLAY_DIGITS;
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
    
    int i;
    str2reg(cpu->a, [[stateDict objectForKey:@"a"] UTF8String]);
    str2reg(cpu->b, [[stateDict objectForKey:@"b"] UTF8String]);
    str2reg(cpu->c, [[stateDict objectForKey:@"c"] UTF8String]);
    str2reg(cpu->y, [[stateDict objectForKey:@"y"] UTF8String]);
    str2reg(cpu->z, [[stateDict objectForKey:@"z"] UTF8String]);
    str2reg(cpu->t, [[stateDict objectForKey:@"t"] UTF8String]);
    str2reg(cpu->m1, [[stateDict objectForKey:@"m1"] UTF8String]);
    str2reg(cpu->m2, [[stateDict objectForKey:@"m2"] UTF8String]);
    
    cpu->f          = (digit_t)[[stateDict objectForKey:@"f"] unsignedIntValue];
    cpu->p          = (digit_t)[[stateDict objectForKey:@"p"] unsignedIntValue];
    
    cpu->decimal    = (bool)[[stateDict objectForKey:@"decimal"] boolValue];
    cpu->carry      = (bool)[[stateDict objectForKey:@"carry"] boolValue];
    
    cpu->s          = (uint16_t)[[stateDict objectForKey:@"s"] unsignedIntValue];
    cpu->ext_flag   = (uint16_t)[[stateDict objectForKey:@"ext_flag"] unsignedIntValue];
    
    cpu->pc         = (uint16_t)[[stateDict objectForKey:@"pc"] unsignedIntValue];
    
    
    cpu->del_rom_flag   = (bool)[[stateDict objectForKey:@"del_rom_flag"] boolValue];
    cpu->del_rom        = (uint8_t)[[stateDict objectForKey:@"del_rom"] boolValue];
    
    cpu->inst_state     = [[stateDict objectForKey:@"inst_state"] unsignedIntValue];
    
    cpu->sp         = [[stateDict objectForKey:@"sp"] unsignedIntValue];
    
    for(i=0; i<STACK_SIZE; i++) {
        cpu->stack[i] = (uint16_t)[[[stateDict objectForKey:@"stack"] objectAtIndex: i] unsignedIntValue]; //poprawić i na NSUint
    }
    
    cpu->display_enable = (bool)[[stateDict objectForKey:@"display_enable"] boolValue];
    
    cpu->bank           = (bool)[[stateDict objectForKey:@"bank"] boolValue];
    
    //memory
    
    cpu->ram_addr = (uint16_t)[[stateDict objectForKey:@"ram_addr"] unsignedIntValue];
    
    for(i=0; i<cpu->max_ram; i++) {
        str2reg(cpu->ram[i], [[[stateDict objectForKey:@"memory"] objectAtIndex: i] UTF8String]); //poprawić i na NSUint
    }
}

- (void)saveState
{
    NSMutableDictionary *stateDict = [[NSMutableDictionary alloc] init];
    char tmp[WSIZE+1] ;
    int i;
    
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->a)] forKey:@"a"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->b)] forKey:@"b"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->c)] forKey:@"c"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->y)] forKey:@"y"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->z)] forKey:@"z"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->t)] forKey:@"t"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->m1)] forKey:@"m1"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->m2)] forKey:@"m2"];
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->f]
                 forKey:@"f"];
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->p]
                 forKey:@"p"];
    
    [stateDict setValue:[NSNumber numberWithBool:cpu->decimal]
                 forKey:@"decimal"];
    [stateDict setValue:[NSNumber numberWithBool:cpu->carry]
                 forKey:@"carry"];
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->s]
                 forKey:@"s"];
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->ext_flag]
                 forKey:@"ext_flag"];
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->pc]
                 forKey:@"pc"];
    
    
    [stateDict setValue:[NSNumber numberWithBool:cpu->del_rom_flag]
                 forKey:@"del_rom_flag"];
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->del_rom]
                 forKey:@"del_rom"];
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->inst_state]
                 forKey:@"inst_state"];
    
    [stateDict setValue:[NSNumber numberWithInt: cpu->sp]
                 forKey:@"sp"];
    
    NSMutableArray *stack = [[NSMutableArray alloc] init];
    
    for(i=0; i<STACK_SIZE; i++) {
        [stack insertObject: [NSNumber numberWithUnsignedInt: cpu->stack[i]] atIndex:i];
    }
    
    [stateDict setValue:stack
                 forKey:@"stack"];
    
    [stateDict setValue:[NSNumber numberWithBool:cpu->display_enable]
                 forKey:@"display_enable"];
    
    [stateDict setValue:[NSNumber numberWithBool:cpu->bank]
                 forKey:@"bank"];
    
    //memory
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->ram_addr]
                 forKey:@"ram_addr"];
    
    NSMutableArray *memory = [[NSMutableArray alloc] init];
    for(i=0; i<cpu->max_ram; i++) {
        [memory insertObject: [NSString stringWithUTF8String:reg2str(tmp, cpu->ram[i])] atIndex:i];
    }
    
    [stateDict setValue:memory
                 forKey:@"memory"];
    
    [stateDict writeToFile:[self calculatorStateFilename] atomically:YES];
    
}

@end
