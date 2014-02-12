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
//  ClassicController.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-08.
//  Copyright 2005-2012 Maciej Bartosiak
//

#import "ClassicController.h"

@implementation ClassicController

#define JIFFY_PER_SEC 30.0

- (void)awakeFromNib
{	
	[NSApp setDelegate:self];
		
	keyQueue = [[NSMutableArray alloc] init];
	simulator = [[ClassicSimulator alloc] init];
	simulator.display = display;
	//[display setupDisplayWith: [simulator displaySegments]
	//					count: [simulator displayDigits]
	//					withX: ((320.0 - (15.0 * DIGIT_W) - (DIGIT_S * 2.0) * 14.0)/2.0)/*CLASSIC_DIGIT_START_OFF*/
	//						Y: 15.0];

	[display setupDisplayWith: [simulator displaySegments]
						count: [simulator displayDigits]
					  //yOffset: 18.0
				  digitHeight: 20.0
				   digitWidth: 9.0 
				  digitOffset: 10.0
				   digitShare: 0.0
				  digitStroke: 1.3
					dotOffset: 5.0];
	
	timer = [NSTimer scheduledTimerWithTimeInterval:(1.0/JIFFY_PER_SEC)
											  target:self
											selector:@selector(run:)
											userInfo:nil
											 repeats:YES];
	
}

- (IBAction)buttonPressed:(id)sender
{
	[keyQueue insertObject:[NSNumber numberWithInteger: [sender tag]] atIndex:0];
	[keyQueue insertObject:[NSNumber numberWithInt: -1] atIndex:0];
}

- (IBAction)modeSwitch:(id)sender
{
	NSInteger fff = [sender selectedSegment];
	if (fff == 2)
	{
		[simulator setFlag: 3 withBool:false];
		[simulator setFlag:11 withBool:false];
		//NSLog(@"Run");
	} else if (fff == 1) {
		[simulator setFlag: 3 withBool:true];
		[simulator setFlag:11 withBool:false];
		//NSLog(@"Prgm");
	} else if (fff == 0) {
		[simulator setFlag: 3 withBool:false];
		[simulator setFlag:11 withBool:true];
		//NSLog(@"Times");
	} //else {
		//NSLog(@"Wrong %d", fff);
	//}
}

- (void)run:(NSTimer *)aTimer
{
	[simulator readKeysFrom: keyQueue];
	[simulator executeCycle];
}

- (void)quit
{
    [timer invalidate];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	[simulator saveState];
    [self quit];
}

//--------------------------------------------------------------------------------------------------------
// NSWindow delegate methods
//--------------------------------------------------------------------------------------------------------
- (void)windowDidBecomeKey:(NSNotification *)aNotification
{
    [[aNotification object] setAlphaValue:1.0];
}

- (void)windowDidResignKey:(NSNotification *)aNotification
{
    [[aNotification object] setAlphaValue:0.85];
}

//- (void)applicationWillTerminate:(NSNotification *)aNotification {
//    [self quit];
//}


@end
