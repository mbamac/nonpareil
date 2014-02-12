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
//  LEDDigit.m
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-26.
//  Copyright 2005 Maciej Bartosiak.
//

#import "LEDDigit.h"

@implementation LEDDigit

- (id)initWithDigitHeight: (float) digitH 
					width: (float) digitW 
					share: (float) share
				   stroke: (float) stroke
				dotOffset: (float) dotOff
						x: (float) x
						y: (float) y
{
	NSAffineTransform *tmp;
	
	float digith = digitH / 2.0;
	//float digitoff = (digitW + digitS * 2.0);
	
    self = [super init];
	
	[NSBezierPath setDefaultLineWidth: stroke];
	//[NSBezierPath setDefaultLineCapStyle: NSRoundLineCapStyle];
	
	a = [NSBezierPath bezierPath];
	[a moveToPoint:	NSMakePoint(    0.0,    0.0)];
	[a lineToPoint:	NSMakePoint( digitW,    0.0)];
	
	b = [NSBezierPath bezierPath];
	[b moveToPoint:	NSMakePoint( digitW,	0.0)];
	[b lineToPoint:	NSMakePoint( digitW, digith)];
	
	c = [NSBezierPath bezierPath];
	[c moveToPoint:	NSMakePoint( digitW, digith)];
	[c lineToPoint:	NSMakePoint( digitW, digitH)];
	
	d = [NSBezierPath bezierPath];
	[d moveToPoint:	NSMakePoint(	0.0, digitH)];
	[d lineToPoint:	NSMakePoint( digitW, digitH)];
	
	e = [NSBezierPath bezierPath];
	[e moveToPoint:	NSMakePoint(	0.0, digith)];
	[e lineToPoint:	NSMakePoint(	0.0, digitH)];
	
	f = [NSBezierPath bezierPath];
	[f moveToPoint:	NSMakePoint(	0.0,	0.0)];
	[f lineToPoint:	NSMakePoint(	0.0, digith)];
	
	g = [NSBezierPath bezierPath];
	[g moveToPoint:	NSMakePoint(	0.0, digith)];
	[g lineToPoint:	NSMakePoint( digitW, digith)];
	
	// "dot" segment
	h = [NSBezierPath bezierPath];
	[h moveToPoint:	NSMakePoint(digitW+dotOff-1.0, digitH-1.0)];
	[h lineToPoint:	NSMakePoint(digitW+dotOff-1.0, digitH+1.0)];
	[h lineToPoint:	NSMakePoint(digitW+dotOff-2.0, digitH+1.0)];
	[h lineToPoint:	NSMakePoint(digitW+dotOff-2.0, digitH-1.0)];
	//[h closePath];
	
	// "," segment
	
	i = [NSBezierPath bezierPath];
	[i moveToPoint:	NSMakePoint(digitW + dotOff-1.0, digitH /*-DIGIT_o*/)];
	[i lineToPoint:	NSMakePoint(digitW + dotOff-1.0-dotOff/3.0, digitH+dotOff/2.0)];
	
	tmp = [NSAffineTransform transform];
	[tmp shearXBy: share]; 
	[tmp translateXBy: x + 0.5 yBy: y + 0.5];
	[tmp scaleXBy:1.0 yBy:-1.0];
	[a transformUsingAffineTransform: tmp];
	[b transformUsingAffineTransform: tmp];
	[c transformUsingAffineTransform: tmp];
	[d transformUsingAffineTransform: tmp];
	[e transformUsingAffineTransform: tmp];
	[f transformUsingAffineTransform: tmp];
	[g transformUsingAffineTransform: tmp];
	[h transformUsingAffineTransform: tmp];
	[i transformUsingAffineTransform: tmp];
	
	return self;
}

- (id)init
{
	return [self initWithDigitHeight: (float) 16.0 
							   width: (float) 8.0
							   share: (float) 0.1
							  stroke: (float) 1.5
						   dotOffset: (float) 6.0
								   x: (float) 0.0
								   y: (float) 0.0];
}

- (void) drawDigit: (segment_bitmap_t)dig
{
#if 0
	[[NSColor darkGrayColor] set];
	[a stroke];
	[b stroke];
	[c stroke];
	[d stroke];
	[e stroke];
	[f stroke];
	[g stroke];
	[h stroke];
	[i stroke];
	[[NSColor redColor] set];
#endif
	if((dig >> 0) & 1) [a stroke];
	if((dig >> 1) & 1) [b stroke];
	if((dig >> 2) & 1) [c stroke];
	if((dig >> 3) & 1) [d stroke];
	if((dig >> 4) & 1) [e stroke];
	if((dig >> 5) & 1) [f stroke];
	if((dig >> 6) & 1) [g stroke];
	if((dig >> 7) & 1) [h stroke];
	if((dig >> 8) & 1) [i stroke];
}

@end
