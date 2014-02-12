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

#import <Cocoa/Cocoa.h>
#import "LEDDisplayView.h"
#import "ClassicSimulator.h"

@interface ClassicController : NSObject
{
    IBOutlet LEDDisplayView *display;
	
	ClassicSimulator *simulator;
	
	NSTimer		    *timer;
	NSMutableArray	*keyQueue;
}
- (IBAction)buttonPressed:(id)sender;
- (void)run:(NSTimer *)aTimer;
- (void)quit;
@end
