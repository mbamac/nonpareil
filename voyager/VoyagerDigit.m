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
//  VoyagerDigit.m
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-09-26.
//  Copyright 2005-2012 Maciej Bartosiak.
//

#import "VoyagerDigit.h"

@implementation VoyagerDigit

- (id)initWithDigitHeight: (float) digitH 
					width: (float) digitW 
					share: (float) share
				   stroke: (float) stroke
				dotOffset: (float) dotOff
						x: (float) x
						y: (float) y
{
	NSAffineTransform *tmp;
	
	float digito = 1.0;
	float digitO = digito * 2.0;
	float digith = digitH / 2.0;
	float digits = stroke / 2.0;
	float digitWS = digitW - (stroke * 2.0);
	float digitHS = digith - (stroke * 1.5); // (stroke * 1.5) = stroke + digits 
			
    self = [super init];
	
	a = [NSBezierPath bezierPath];
	[a moveToPoint:			NSMakePoint(            digito, digitH        )];
	[a relativeLineToPoint:	NSMakePoint(  digitW  - digitO,    0.0        )];
	[a relativeLineToPoint:	NSMakePoint( -stroke          ,-stroke        )];
	[a relativeLineToPoint:	NSMakePoint( -digitWS + digitO,    0.0        )];
	[a closePath];
	
	
	b = [NSBezierPath bezierPath];
	[b moveToPoint:			NSMakePoint(            digitW, digitH  - digito)];
	[b relativeLineToPoint:	NSMakePoint(               0.0,-digith  + digitO)];
	[b relativeLineToPoint:	NSMakePoint(          -stroke , digits          )];
	[b relativeLineToPoint:	NSMakePoint(               0.0, digitHS - digitO)];
	[b closePath];
	
	
	c = [b copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: 0.0 yBy: digitH];
	[tmp scaleXBy: 1.0 yBy: -1.0];
	[c transformUsingAffineTransform: tmp];
	
	d = [a copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: 0.0 yBy: digitH];
	[tmp scaleXBy: 1.0 yBy: -1.0];
	[d transformUsingAffineTransform: tmp];
	
	e = [b copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: digitW yBy: digitH];
	[tmp scaleXBy: -1.0 yBy: -1.0];
	[e transformUsingAffineTransform: tmp];
	
	f = [c copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: digitW yBy: digitH];
	[tmp scaleXBy: -1.0 yBy: -1.0];
	[f transformUsingAffineTransform: tmp];
	
	g = [NSBezierPath bezierPath];
	[g moveToPoint:			NSMakePoint(           digito, digith)];
	[g relativeLineToPoint:	NSMakePoint( stroke          , digits)];
	[g relativeLineToPoint:	NSMakePoint( digitWS - digitO,    0.0)];
	[g relativeLineToPoint:	NSMakePoint( stroke          ,-digits)];
	[g relativeLineToPoint:	NSMakePoint(-stroke          ,-digits)];
	[g relativeLineToPoint:	NSMakePoint(-digitWS + digitO,    0.0)];
	[g closePath];
	
	// "dot" segment
	h = [NSBezierPath bezierPath];
	[h moveToPoint:			NSMakePoint(   digitW+dotOff,    0.0)];
	[h relativeLineToPoint:	NSMakePoint(             0.0, stroke)];
	[h relativeLineToPoint:	NSMakePoint(          stroke,    0.0)];
	[h relativeLineToPoint:	NSMakePoint(             0.0,-stroke)];
	[h closePath];
	
	// "," segment
	i = [NSBezierPath bezierPath];
	[i moveToPoint:			NSMakePoint(  digitW + dotOff,  -digito)];
	[i relativeLineToPoint:	NSMakePoint(           stroke,      0.0)];
	[i relativeLineToPoint:	NSMakePoint(          -stroke,  -stroke)];
	[i relativeLineToPoint:	NSMakePoint(      -stroke/2.0,      0.0)];
	[i closePath];
	
	tmp = [NSAffineTransform transform];
	[tmp shearXBy: share]; 
	//[tmp translateXBy:DIGIT_OFF/2.0 yBy:0.0];
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
	return [self initWithDigitHeight: (float) 25.0 
							   width: (float) 15.0 
							   share: (float) 0.1
							  stroke: (float) 3.5
						   dotOffset: (float) 5.0
								   x: (float) 0.0
								   y: (float) 24.0];
}

- (void) drawDigit: (segment_bitmap_t)dig
{		
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
