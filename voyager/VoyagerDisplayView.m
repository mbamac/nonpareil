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
//  VoyagerDisplayView.m
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-09-09.
//  Copyright 2005-2012 Maciej Bartosiak
//

#import "VoyagerDisplayView.h"

@implementation VoyagerDisplayView

- (id)initWithFrame:(NSRect)frameRect
{
	//NSFont *font;
	
	if ((self = [super initWithFrame:frameRect]) != nil)
	{
		ds = NULL;
		dc = 0;
		
		attrs = [[NSMutableDictionary alloc] init];
		[attrs setObject:[NSColor blackColor] forKey:NSForegroundColorAttributeName];
		
		if (([NSFont fontWithName:@"Monaco" size:9.0]) != nil)
			[attrs setObject:[NSFont fontWithName:@"Monaco" size:9.0] forKey:NSFontAttributeName];
		else
			[attrs setObject:[NSFont systemFontOfSize:9.0] forKey:NSFontAttributeName];
	}
	return self;
}

- (void)drawRect:(NSRect)rect
{
	int num;
	
	if (ds == NULL)
		return;
	
	[[NSColor blackColor] set];
	
	for (num = 0; num < dc; num++)
		if (ds[num]) // we don't want to draw empty digit
			[[digits objectAtIndex: num] drawDigit:ds[num]];
		
		
	// Now is time for annunciatiors

#define ANNUNC_OFF 5.0
	if (((ds[1] >> 17) & 1))
		[[NSString stringWithUTF8String: "*"] drawAtPoint: NSMakePoint(15.0,ANNUNC_OFF)
											 withAttributes: attrs];
	if (((ds[2] >> 17) & 1))
		[[NSString stringWithUTF8String: "USER"] drawAtPoint: NSMakePoint(47.0,ANNUNC_OFF)
										withAttributes: attrs];
	if (((ds[3] >> 17) & 1))
		[[NSString stringWithUTF8String: "f"] drawAtPoint: NSMakePoint(89.0,ANNUNC_OFF)
										   withAttributes: attrs];
	if (((ds[4] >> 17) & 1))
		[[NSString stringWithUTF8String: "g"] drawAtPoint: NSMakePoint(107.0,ANNUNC_OFF)
										withAttributes: attrs];
	if (((ds[5] >> 17) & 1))
		[[NSString stringWithUTF8String: "BEGIN"] drawAtPoint: NSMakePoint(125.0,ANNUNC_OFF)
										withAttributes: attrs];
	if (((ds[6] >> 17) & 1))
		[[NSString stringWithUTF8String: "G"] drawAtPoint: NSMakePoint(183.0,ANNUNC_OFF)
										  withAttributes: attrs];
	if (((ds[7] >> 17) & 1))
		[[NSString stringWithUTF8String: "RAD"] drawAtPoint: NSMakePoint(189.0,ANNUNC_OFF)
											withAttributes: attrs];
	if (((ds[8] >> 17) & 1))
		[[NSString stringWithUTF8String: "D.MY"] drawAtPoint: NSMakePoint(212.0,ANNUNC_OFF)
										   withAttributes: attrs];
	if (((ds[9] >> 17) & 1))
		[[NSString stringWithUTF8String: "C"] drawAtPoint: NSMakePoint(246.0,ANNUNC_OFF)
										   withAttributes: attrs];
	if (((ds[10] >> 17) & 1))
		[[NSString stringWithUTF8String: "PRGM"] drawAtPoint: NSMakePoint(273.0,ANNUNC_OFF)
										withAttributes: attrs];
	
	[self setNeedsDisplay: NO];
}

//- (void)setupDisplayWith:(segment_bitmap_t *)disps count: (int)count
- (void)setupDisplayWith: (segment_bitmap_t *)disps
				   count: (int) count
				 yOffset: (float) y
			 digitHeight: (float) digitHeight
			  digitWidth: (float) digitWidth
			 digitOffset: (float) digitOffset
			  digitShare: (float) digitShare
			 digitStroke: (float) digitStroke
			   dotOffset: (float) dotOffset
{
	VoyagerDigit *dig;
	NSMutableArray *tmp;
	
	int i;
	float xOff = ([self frame].size.width - ((count) * (digitWidth + digitOffset)))/2.0;	
	dc = count;
	ds = disps;
	
	tmp = [NSMutableArray arrayWithCapacity: dc];
	
	for (i = 0; i < dc; i++)
	{
		/*dig = [[VoyagerDigit alloc] initWithDigitHeight: (float) 25.0 
												  width: (float) 15.0 
												  share: (float) 0.1
												 stroke: (float) 3.5
											  dotOffset: (float) 3.0
													  x: (float) xoff
													  y: (float) 24.0];*/
		dig = [[VoyagerDigit alloc] initWithDigitHeight: digitHeight
												  width: digitWidth
												  share: digitShare
												 stroke: digitStroke
											  dotOffset: dotOffset
													  x: xOff
													  y: y];
		[tmp insertObject: dig atIndex: i];
		xOff += (digitWidth + digitOffset);
	}
	
	digits = [[NSArray alloc] initWithArray: tmp];
}

@end
