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
//  NonpareilController.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-09-09.
//  Copyright 2005-2012 Maciej Bartosiak
//

#import "VoyagerController.h"

@implementation VoyagerController

#define JIFFY_PER_SEC 30.0

- (void)awakeFromNib
{	
	[NSApp setDelegate:self];
		
	keyQueue = [[NSMutableArray alloc] init];
	
	simulator = [[VoyagerSimulator alloc] init];
	simulator.display = display;
	//[display setupDisplayWith:[simulator displaySegments] count: [simulator displayDigits]];
	[display setupDisplayWith: [simulator displaySegments]
						count: [simulator displayDigits]
					  yOffset: 24.0
				  digitHeight: 25.0
				   digitWidth: 15.0 
				  digitOffset: 10.5
				   digitShare: 0.1
				  digitStroke: 3.5
					dotOffset: 3.5];
	
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

- (void)run:(NSTimer *)aTimer
{
	[simulator readKeysFrom: keyQueue];
	
	[simulator executeCycle];
	//if([simulator executeCycle])
	//{
	//	[display updateDisplay];
	//}
}

- (void)quit
{
    [timer invalidate];
    //if (! write_ram_file (ram))
	//[simulator printState];
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
