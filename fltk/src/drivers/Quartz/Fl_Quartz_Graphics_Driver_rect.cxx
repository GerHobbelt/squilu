//
// "$Id: Fl_Quartz_Graphics_Driver_rect.cxx 12622 2018-01-05 16:01:39Z manolo $"
//
// Rectangle drawing routines for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2016 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//


#include "../../config_lib.h"
#ifdef FL_CFG_GFX_QUARTZ

#include <FL/Fl.H>
#include <FL/x.H>


/**
  \file quartz_rect.cxx
  \brief Apple Quartz specific line and polygon drawing with integer coordinates.
*/


#include "Fl_Quartz_Graphics_Driver.H"


// --- line and polygon drawing with integer coordinates

void Fl_Quartz_Graphics_Driver::point(int x, int y) {
  CGContextFillRect(gc_, CGRectMake(x - 0.5, y - 0.5, 1, 1) );
}

void Fl_Quartz_Graphics_Driver::rect(int x, int y, int w, int h) {
  if (w<=0 || h<=0) return;
  if ( (!has_feature(PRINTER)) && quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGRect rect = CGRectMake(x, y, w-1, h-1);
  CGContextStrokeRect(gc_, rect);
  if ( (!has_feature(PRINTER)) && quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::rectf(int x, int y, int w, int h) {
  if (w<=0 || h<=0) return;
  CGRect rect = CGRectMake(x - 0.5, y - 0.5, w , h);
  CGContextFillRect(gc_, rect);
}

void Fl_Quartz_Graphics_Driver::line(int x, int y, int x1, int y1) {
  if (quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x1, y1);
  CGContextStrokePath(gc_);
  if (quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::line(int x, int y, int x1, int y1, int x2, int y2) {
  if (quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x1, y1);
  CGContextAddLineToPoint(gc_, x2, y2);
  CGContextStrokePath(gc_);
  if (quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::xyline(int x, int y, int x1) {
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x1, y);
  CGContextStrokePath(gc_);
  if (high_resolution()) {
    /* On retina displays, all xyline() and yxline() functions produce lines that are half-unit
     (or one pixel) too short at both ends. This is corrected by filling at both ends rectangles
     of size one unit by line-width.
     */
    CGContextFillRect(gc_, CGRectMake(x-0.5 , y  - quartz_line_width_/2, 1 , quartz_line_width_));
    CGContextFillRect(gc_, CGRectMake(x1-0.5 , y  - quartz_line_width_/2, 1 , quartz_line_width_));
  }
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::xyline(int x, int y, int x1, int y2) {
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x1, y);
  CGContextAddLineToPoint(gc_, x1, y2);
  CGContextStrokePath(gc_);
  if (high_resolution()) {
    CGContextFillRect(gc_, CGRectMake(x-0.5, y  - quartz_line_width_/2, 1 , quartz_line_width_));
    CGContextFillRect(gc_, CGRectMake(x1  -  quartz_line_width_/2, y2-0.5, quartz_line_width_, 1));
  }
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::xyline(int x, int y, int x1, int y2, int x3) {
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x1, y);
  CGContextAddLineToPoint(gc_, x1, y2);
  CGContextAddLineToPoint(gc_, x3, y2);
  CGContextStrokePath(gc_);
  if (high_resolution()) {
    CGContextFillRect(gc_, CGRectMake(x-0.5, y  - quartz_line_width_/2, 1 , quartz_line_width_));
    CGContextFillRect(gc_, CGRectMake(x3-0.5, y2  - quartz_line_width_/2, 1 , quartz_line_width_));
  }
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::yxline(int x, int y, int y1) {
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x, y1);
  CGContextStrokePath(gc_);
  if (high_resolution()) {
    CGContextFillRect(gc_, CGRectMake(x  -  quartz_line_width_/2, y-0.5, quartz_line_width_, 1));
    CGContextFillRect(gc_, CGRectMake(x  -  quartz_line_width_/2, y1-0.5, quartz_line_width_, 1));
  }
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::yxline(int x, int y, int y1, int x2) {
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x, y1);
  CGContextAddLineToPoint(gc_, x2, y1);
  CGContextStrokePath(gc_);
  if (high_resolution()) {
    CGContextFillRect(gc_, CGRectMake(x  -  quartz_line_width_/2, y-0.5, quartz_line_width_, 1));
    CGContextFillRect(gc_, CGRectMake(x2-0.5, y1  - quartz_line_width_/2, 1 , quartz_line_width_));
  }
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::yxline(int x, int y, int y1, int x2, int y3) {
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x, y1);
  CGContextAddLineToPoint(gc_, x2, y1);
  CGContextAddLineToPoint(gc_, x2, y3);
  CGContextStrokePath(gc_);
  if (high_resolution()) {
    CGContextFillRect(gc_, CGRectMake(x  -  quartz_line_width_/2, y-0.5, quartz_line_width_, 1));
    CGContextFillRect(gc_, CGRectMake(x2  -  quartz_line_width_/2, y3-0.5, quartz_line_width_, 1));
  }
  if (has_feature(PRINTER) || quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::loop(int x, int y, int x1, int y1, int x2, int y2) {
  if (quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x1, y1);
  CGContextAddLineToPoint(gc_, x2, y2);
  CGContextClosePath(gc_);
  CGContextStrokePath(gc_);
  if (quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::loop(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3) {
  if (quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x1, y1);
  CGContextAddLineToPoint(gc_, x2, y2);
  CGContextAddLineToPoint(gc_, x3, y3);
  CGContextClosePath(gc_);
  CGContextStrokePath(gc_);
  if (quartz_line_width_ > 1.5f) CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::polygon(int x, int y, int x1, int y1, int x2, int y2) {
  CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x1, y1);
  CGContextAddLineToPoint(gc_, x2, y2);
  CGContextClosePath(gc_);
  CGContextFillPath(gc_);
  CGContextSetShouldAntialias(gc_, false);
}

void Fl_Quartz_Graphics_Driver::polygon(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3) {
  CGContextSetShouldAntialias(gc_, true);
  CGContextMoveToPoint(gc_, x, y);
  CGContextAddLineToPoint(gc_, x1, y1);
  CGContextAddLineToPoint(gc_, x2, y2);
  CGContextAddLineToPoint(gc_, x3, y3);
  CGContextClosePath(gc_);
  CGContextFillPath(gc_);
  CGContextSetShouldAntialias(gc_, false);
}

// --- clipping

// intersects current and x,y,w,h rectangle and returns result as a new Fl_Region
static Fl_Region intersect_region_and_rect(Fl_Region current, int x,int y,int w, int h)
{
  if (current == NULL) return Fl_Graphics_Driver::default_driver().XRectangleRegion(x,y,w,h);
  CGRect r = Fl_Quartz_Graphics_Driver::fl_cgrectmake_cocoa(x, y, w, h);
  Fl_Region outr = (Fl_Region)malloc(sizeof(*outr));
  outr->count = current->count;
  outr->rects =(CGRect*)malloc(outr->count * sizeof(CGRect));
  int j = 0;
  for(int i = 0; i < current->count; i++) {
    CGRect test = CGRectIntersection(current->rects[i], r);
    if (!CGRectIsEmpty(test)) outr->rects[j++] = test;
  }
  if (j) {
    outr->count = j;
    outr->rects = (CGRect*)realloc(outr->rects, outr->count * sizeof(CGRect));
  }
  else {
    Fl_Graphics_Driver::default_driver().XDestroyRegion(outr);
    outr = Fl_Graphics_Driver::default_driver().XRectangleRegion(0,0,0,0);
  }
  return outr;
}


void Fl_Quartz_Graphics_Driver::push_clip(int x, int y, int w, int h) {
  Fl_Region r;
  if (w > 0 && h > 0) {
    r = XRectangleRegion(x,y,w,h);
    Fl_Region current = rstack[rstackptr];
    if (current) {
      XDestroyRegion(r);
      r = intersect_region_and_rect(current, x,y,w,h);
    }
  } else { // make empty clip region:
    r = XRectangleRegion(0,0,0,0);
  }
  if (rstackptr < region_stack_max) rstack[++rstackptr] = r;
  else Fl::warning("Fl_Quartz_Graphics_Driver::push_clip: clip stack overflow!\n");
  restore_clip();
}

int Fl_Quartz_Graphics_Driver::clip_box(int x, int y, int w, int h, int& X, int& Y, int& W, int& H){
  X = x; Y = y; W = w; H = h;
  Fl_Region r = rstack[rstackptr];
  if (!r) return 0;
  CGRect arg = fl_cgrectmake_cocoa(x, y, w, h);
  CGRect u = CGRectMake(0,0,0,0);
  CGRect test;
  for (int i = 0; i < r->count; i++) {
    test = CGRectIntersection(r->rects[i], arg);
    if ( !CGRectIsEmpty(test) ) {
      if(CGRectIsEmpty(u)) u = test;
      else u = CGRectUnion(u, test);
    }
  }
  X = int(u.origin.x + 0.5); // reverse offset introduced by fl_cgrectmake_cocoa()
  Y = int(u.origin.y + 0.5);
  W = int(u.size.width + 0.5); // round to nearest integer
  H = int(u.size.height + 0.5);
  if (CGRectIsEmpty(u)) W = H = 0;
  return !CGRectEqualToRect(arg, u);
}

int Fl_Quartz_Graphics_Driver::not_clipped(int x, int y, int w, int h) {
  if (x+w <= 0 || y+h <= 0) return 0;
  Fl_Region r = rstack[rstackptr];
  if (!r) return 1;
  CGRect arg = fl_cgrectmake_cocoa(x, y, w, h);
  for (int i = 0; i < r->count; i++) {
    CGRect test = CGRectIntersection(r->rects[i], arg);
    if (!CGRectIsEmpty(test)) return 1;
  }
  return 0;
}

// make there be no clip (used by fl_begin_offscreen() only!)
void Fl_Quartz_Graphics_Driver::push_no_clip() {
  if (rstackptr < region_stack_max) rstack[++rstackptr] = 0;
  else Fl::warning("Fl_Quartz_Graphics_Driver::push_no_clip: clip stack overflow!\n");
  restore_clip();
}

// pop back to previous clip:
void Fl_Quartz_Graphics_Driver::pop_clip() {
  if (rstackptr > 0) {
    Fl_Region oldr = rstack[rstackptr--];
    if (oldr) XDestroyRegion(oldr);
  } else Fl::warning("Fl_Quartz_Graphics_Driver::pop_clip: clip stack underflow!\n");
  restore_clip();
}

void Fl_Quartz_Graphics_Driver::restore_clip() {
  fl_clip_state_number++;
  Fl_Region r = rstack[rstackptr];
  if ( fl_window || gc_ ) { // clipping for a true window or an offscreen buffer
    if (gc_) {
      CGContextRestoreGState(gc_);
      CGContextSaveGState(gc_);
    }
    color(color());
    quartz_restore_line_style();
    if (r) { //apply program clip
      CGContextClipToRects(gc_, r->rects, r->count);
    }
  }
}


#endif // FL_CFG_GFX_QUARTZ

//
// End of "$Id: Fl_Quartz_Graphics_Driver_rect.cxx 12622 2018-01-05 16:01:39Z manolo $".
//
