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
//  LEDDisplayView.m
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-26.
//  Copyright 2005-2012 Maciej Bartosiak.
//

#import "LEDDisplayView.h"

@implementation LEDDisplayView

- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil)
	{
		ds = NULL;
		dc = 0;
	}
	return self;
}

- (void)drawRect:(NSRect)rect
{
	int num;
	
	if (ds == NULL)
		return;
	
	//[[NSColor redColor] set];
	[[NSColor colorWithDeviceRed:1.0
						   green:0.2
							blue:0.2 
						   alpha:1.0] set];
	
	for (num = 0; num < dc; num++)
		if (ds[num]) // we don't want to draw empty digit
			[[digits objectAtIndex: num] drawDigit:ds[num]];
}

- (void)setupDisplayWith: (segment_bitmap_t *)disps
				   count: (int) count
				 //yOffset: (float) y
			 digitHeight: (float) digitHeight
			  digitWidth: (float) digitWidth
			 digitOffset: (float) digitOffset
			  digitShare: (float) digitShare
			 digitStroke: (float) digitStroke
			   dotOffset: (float) dotOffset
{
	LEDDigit *dig;
	NSMutableArray *tmp;
	
	int i;
	//float xOff = ([self frame].size.width - ((count) * (digitWidth + digitOffset)))/2.0;
	float xOff = ([self frame].size.width - ((count) * (digitWidth + digitOffset)))/2.0 ;//+ digitWidth/4.0;
	float yOff = (([self frame].size.height + digitHeight) / 2.0 - 1.0);//- digitHeight/4.0;
	
	dc = count;
	ds = disps;
	
	tmp = [NSMutableArray arrayWithCapacity: dc];
	
	for (i = 0; i < dc; i++)
	{		
		dig = [[LEDDigit alloc] initWithDigitHeight: digitHeight
											  width: digitWidth
											  share: digitShare
											 stroke: digitStroke
										  dotOffset: dotOffset
												  x: xOff
												  y: yOff];
		
		[tmp insertObject: dig atIndex: i];
		xOff += (digitWidth + digitOffset);
	}
	
	digits = [[NSArray alloc] initWithArray: tmp];
}

@end
