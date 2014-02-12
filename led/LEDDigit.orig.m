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
- (id)initWithX: (float)x Y: (float)y
{
	NSAffineTransform *tmp;
		
    self = [super init];
	
	a = [[NSBezierPath bezierPath] retain];
	[a moveToPoint:			NSMakePoint(          DIGIT_o, DIGIT_H        )];
	[a relativeLineToPoint:	NSMakePoint(  DIGIT_W - DIGIT_O,   0.0        )];
	[a relativeLineToPoint:	NSMakePoint( -DIGIT_S        ,-DIGIT_S        )];
	[a relativeLineToPoint:	NSMakePoint( -DIGITWS + DIGIT_O,   0.0        )];
	[a closePath];
	
	
	b = [[NSBezierPath bezierPath] retain];
	[b moveToPoint:			NSMakePoint(          DIGIT_W, DIGIT_H - DIGIT_o)];
	[b relativeLineToPoint:	NSMakePoint(              0.0,-DIGIT_h + DIGIT_O)];
	[b relativeLineToPoint:	NSMakePoint(        -DIGIT_S , DIGIT_s          )];
	[b relativeLineToPoint:	NSMakePoint(              0.0, DIGITHS - DIGIT_O)];
	[b closePath];
	
	
	c = [b copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: 0.0 yBy: DIGIT_H];
	[tmp scaleXBy: 1.0 yBy: -1.0];
	[c transformUsingAffineTransform: tmp];
	
	d = [a copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: 0.0 yBy: DIGIT_H];
	[tmp scaleXBy: 1.0 yBy: -1.0];
	[d transformUsingAffineTransform: tmp];
	
	e = [b copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: DIGIT_W yBy: DIGIT_H];
	[tmp scaleXBy: -1.0 yBy: -1.0];
	[e transformUsingAffineTransform: tmp];
	
	f = [c copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: DIGIT_W yBy: DIGIT_H];
	[tmp scaleXBy: -1.0 yBy: -1.0];
	[f transformUsingAffineTransform: tmp];
	
	g = [[NSBezierPath bezierPath] retain];
	[g moveToPoint:			NSMakePoint(           DIGIT_o, DIGIT_h)];
	[g relativeLineToPoint:	NSMakePoint( DIGIT_S          , DIGIT_s)];
	[g relativeLineToPoint:	NSMakePoint( DIGITWS - DIGIT_O,     0.0)];
	[g relativeLineToPoint:	NSMakePoint( DIGIT_S          ,-DIGIT_s)];
	[g relativeLineToPoint:	NSMakePoint(-DIGIT_S          ,-DIGIT_s)];
	[g relativeLineToPoint:	NSMakePoint(-DIGITWS + DIGIT_O,     0.0)];
	[g closePath];
	
	h = [[NSBezierPath bezierPath] retain];
	[h moveToPoint:			NSMakePoint(   DIGIT_W+DIGIT_S,   0.0)];
	[h relativeLineToPoint:	NSMakePoint(             0.0, DIGIT_S)];
	[h relativeLineToPoint:	NSMakePoint(           DIGIT_S,   0.0)];
	[h relativeLineToPoint:	NSMakePoint(             0.0,-DIGIT_S)];
	[h closePath];
	
	i = [[NSBezierPath bezierPath] retain];
	[i moveToPoint:			NSMakePoint(  DIGIT_W + DIGIT_S, -DIGIT_o)];
	[i relativeLineToPoint:	NSMakePoint(            DIGIT_S,      0.0)];
	[i relativeLineToPoint:	NSMakePoint(           -DIGIT_S, -DIGIT_S)];
	[i relativeLineToPoint:	NSMakePoint(       -DIGIT_S/2.0,    0.0  )];
	[i closePath];
	
	tmp = [NSAffineTransform transform];
	//[tmp shearXBy:0.1]; 
	[tmp translateXBy:DIGIT_OFF/2.0 yBy:0.0];
	[tmp translateXBy: x yBy: y];
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
	return [self initWithX:0.0 Y:0.0];
}

- (void) drawDigit: (segment_bitmap_t)dig
{
	//[[NSColor whiteColor] set];
	//[a fill];
	//[b fill];
	//[c fill];
	//[d fill];
	//[e fill];
	//[f fill];
	//[g fill];
	//[h fill];
	//[i fill];
	
	//[[NSColor redColor] set];
	if((dig >> 0) & 1) [a fill];
	if((dig >> 1) & 1) [b fill];
	if((dig >> 2) & 1) [c fill];
	if((dig >> 3) & 1) [d fill];
	if((dig >> 4) & 1) [e fill];
	if((dig >> 5) & 1) [f fill];
	if((dig >> 6) & 1) [g fill];
	if((dig >> 7) & 1) [h fill];
	if((dig >> 8) & 1) [i fill];
}

@end
