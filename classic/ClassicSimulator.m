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
//  ClassicSimulator.m
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-08.
//  Copyright 2005-2012 Maciej Bartosiak
//

#define NONPAREIL_INTERNAL
#import "ClassicSimulator.h"
#import "LEDDisplayView.h"
#import <math.h>

@implementation ClassicSimulator

@synthesize display;

- (id)init 
{
    self = [super init];
	
	NSBundle *nonpareilBundle = [NSBundle mainBundle];
	NSString *objFile = [nonpareilBundle pathForResource: NNPR_OBJ ofType:@"obj"];
	
	cpu = classic_new_processor (NNPR_RAM);
	classic_read_object_file (cpu, [objFile UTF8String]);
	
	[self readState];
	lastRun = [NSDate timeIntervalSinceReferenceDate];
	
	return self;
}

- (void)pressKey: (int)key
{
	if (key == -1)
	{
		classic_release_key(cpu);
	} else {
		classic_press_key(cpu, key);
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
#ifdef NONPAREIL_55
			delay = 1;
#endif
		}		
	}	
}

- (void)executeCycle
{
	NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
	int i = (int)round((now - lastRun) * (NNPR_CLOCK / NNPR_WSIZE));
	lastRun = now;
	
	if (i > 5000) i = 5000;
	
	while (i--)
		classic_execute_instruction(cpu);
	if ([self displayScan])
        [display setNeedsDisplay:YES];
}

- (BOOL)displayScan
{
    static segment_bitmap_t char_gen[] = 
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  64, 128,   0,
		63,  6,  91,  79, 102, 109, 125,   7, 127, 111,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 };	
    
	int display_scan_position = CLASSIC_LEFT_SCAN; // WSIZE - 1 = 13
    int display_digit_position = 0;
    BOOL need_update = NO;
	
    // Kręcimy się od 13 do 0
	for (display_scan_position = CLASSIC_LEFT_SCAN; display_scan_position >= CLASSIC_RIGHT_SCAN; display_scan_position--) {
		
		int a = cpu->a [display_scan_position];
		int b = cpu->b [display_scan_position];
        segment_bitmap_t segs = 0;
        display_digit_position = 13 - display_scan_position;
		
        segs = 0;  // blank
        
        if (cpu->display_enable && (b <= 7))
        {
            if ((display_scan_position == 2) ||
                (display_scan_position == 13)) {
                if (a >= 8)
                    segs = char_gen ['-'];
            }
            else
                segs = char_gen ['0' + a];
            if (b == 2) {
                //if ((display_digit_position) < MAX_DIGIT_POSITION)
                segs |= char_gen ['.'];
            }
        }
        if (display_segments [display_digit_position] != segs)
            need_update = YES;
        display_segments [display_digit_position] = segs;
    }
	
    return need_update;
}

- (void)setFlag: (int)num withBool: (BOOL)state
{
	classic_set_ext_flag(cpu, num, (bool)state);
}

- (int)displayDigits
{
	return CLASSIC_DISPLAY_DIGITS;
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
    NSDictionary *stateDict = [[NSMutableDictionary alloc] initWithContentsOfFile: [self calculatorStateFilename]];
    if (stateDict == nil) {
        return;
    }
    
    int i;
    str2reg(cpu->a, [[stateDict objectForKey:@"a"] UTF8String]);
    str2reg(cpu->b, [[stateDict objectForKey:@"b"] UTF8String]);
    str2reg(cpu->c, [[stateDict objectForKey:@"c"] UTF8String]);
    str2reg(cpu->d, [[stateDict objectForKey:@"d"] UTF8String]);
    str2reg(cpu->e, [[stateDict objectForKey:@"e"] UTF8String]);
    str2reg(cpu->f, [[stateDict objectForKey:@"f"] UTF8String]);
    str2reg(cpu->m, [[stateDict objectForKey:@"m"] UTF8String]);
    
    cpu->p          = (digit_t)[[stateDict objectForKey:@"p"] unsignedIntValue];
    
    cpu->carry      = (bool)[[stateDict objectForKey:@"carry"] boolValue];
    //cpu->prev_carry = (bool)[[stateDict objectForKey:@"prev_carry"] boolValue];
    
    cpu->pc         = (uint8_t)[[stateDict objectForKey:@"pc"] unsignedIntValue];
    cpu->rom        = (uint8_t)[[stateDict objectForKey:@"rom"] unsignedIntValue];
    cpu->group      = (uint8_t)[[stateDict objectForKey:@"group"] unsignedIntValue];
    
    cpu->del_rom    = (uint8_t)[[stateDict objectForKey:@"del_rom"] unsignedIntValue];
    cpu->del_grp    = (uint8_t)[[stateDict objectForKey:@"del_grp"] unsignedIntValue];
    
    cpu->ret_pc     = (uint8_t)[[stateDict objectForKey:@"ret_pc"] unsignedIntValue];
    
    cpu->s          = (uint16_t)[[stateDict objectForKey:@"s"] unsignedIntValue];
    cpu->ext_flag   = (uint16_t)[[stateDict objectForKey:@"ext_flag"] unsignedIntValue];
    
    //memory
    
    for(i=0; i<cpu->max_ram; i++) {
        str2reg(cpu->ram[i], [[[stateDict objectForKey:@"memory"] objectAtIndex: i] UTF8String]); //poprawić i na NSUint
    }
    
    cpu->display_enable   = (bool)[[stateDict objectForKey:@"display_enable"] boolValue];
    
}

- (void)saveState
{
    NSMutableDictionary *stateDict = [[NSMutableDictionary alloc] init];
    char tmp[]="01234567890123456";
    int i;
    
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->a)] forKey:@"a"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->b)] forKey:@"b"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->c)] forKey:@"c"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->d)] forKey:@"d"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->e)] forKey:@"e"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->f)] forKey:@"f"];
    [stateDict setValue:[NSString stringWithUTF8String:reg2str(tmp, cpu->m)] forKey:@"m"];
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->p]
                 forKey:@"p"];
    
    [stateDict setValue:[NSNumber numberWithBool:cpu->carry]
                 forKey:@"carry"];
    //[stateDict setValue:[NSNumber numberWithBool:cpu->prev_carry]
    //             forKey:@"prev_carry"];
    
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->pc]
                 forKey:@"pc"];
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->rom]
                 forKey:@"rom"];
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->group]
                 forKey:@"group"];
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->del_rom]
                 forKey:@"del_rom"];
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->del_grp]
                 forKey:@"del_grp"];
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->ret_pc]
                 forKey:@"ret_pc"];
    
    
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->s]
                 forKey:@"s"];
    [stateDict setValue:[NSNumber numberWithUnsignedInt: cpu->ext_flag]
                 forKey:@"ext_flag"];
    NSMutableArray *memory = [[NSMutableArray alloc] init];
    for(i=0; i<cpu->max_ram; i++) {
        [memory insertObject: [NSString stringWithUTF8String:reg2str(tmp, cpu->ram[i])] atIndex:i];
    }
    
    [stateDict setValue:memory
                 forKey:@"memory"];
	
    [stateDict setValue:[NSNumber numberWithBool:cpu->display_enable]
                 forKey:@"display_enable"];
    
    [stateDict writeToFile:[self calculatorStateFilename] atomically:YES];
    
}

@end
