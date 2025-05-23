//
// "$Id: Fl_WinAPI_Screen_Driver.H 12628 2018-01-09 07:26:49Z manolo $"
//
// Definition of MSWindows Win32/64 Screen interface
// for the Fast Light Tool Kit (FLTK).
//
// Copyright 2010-2016 by Bill Spitzak and others.
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

/**
 \file Fl_WinAPI_Screen_Driver.H
 \brief Definition of MSWindows Win32/64 Screen interface.
 */

#ifndef FL_WINAPI_SCREEN_DRIVER_H
#define FL_WINAPI_SCREEN_DRIVER_H

#include <FL/Fl_Screen_Driver.H>
#include <windows.h>

class Fl_Window;


class FL_EXPORT Fl_WinAPI_Screen_Driver : public Fl_Screen_Driver 
{
protected:
  RECT screens[MAX_SCREENS];
  RECT work_area[MAX_SCREENS];
  float dpi[MAX_SCREENS][2];
  float scale_of_screen[MAX_SCREENS];

  static BOOL CALLBACK screen_cb(HMONITOR mon, HDC, LPRECT r, LPARAM);
  BOOL screen_cb(HMONITOR mon, HDC, LPRECT r);
  int get_mouse_unscaled(int &mx, int &my);
#ifdef FLTK_HIDPI_SUPPORT
  void init_screen_scale_factors();
#endif

public:
  Fl_WinAPI_Screen_Driver() : Fl_Screen_Driver() {
    for (int i = 0; i < MAX_SCREENS; i++) scale_of_screen[i] = 1;
  }
  // --- display management
  virtual int visual(int flags);
  // --- screen configuration
  virtual void init();
  virtual int x();
  virtual int y();
  virtual int w();
  virtual int h();
  virtual void screen_xywh(int &X, int &Y, int &W, int &H, int n);
  virtual void screen_dpi(float &h, float &v, int n=0);
  int screen_num_unscaled(int x, int y);
  virtual void screen_work_area(int &X, int &Y, int &W, int &H, int n);
  // --- audible output
  virtual void beep(int type);
  // --- global events
  virtual void flush();
  virtual double wait(double time_to_wait);
  virtual int ready();
  virtual void grab(Fl_Window* win);
  // --- global colors
  virtual void get_system_colors();
  virtual const char *get_system_scheme();
  // --- global timers
  virtual void add_timeout(double time, Fl_Timeout_Handler cb, void *argp);
  virtual void repeat_timeout(double time, Fl_Timeout_Handler cb, void *argp);
  virtual int has_timeout(Fl_Timeout_Handler cb, void *argp);
  virtual void remove_timeout(Fl_Timeout_Handler cb, void *argp);
  virtual int dnd(int unused);
  virtual int compose(int &del);
  virtual Fl_RGB_Image *read_win_rectangle(uchar *p, int X, int Y, int w, int h, int alpha);
  Fl_RGB_Image *read_win_rectangle_unscaled(uchar *p, int X, int Y, int w, int h, int alpha);
  virtual int get_mouse(int &x, int &y);
  virtual void enable_im();
  virtual void disable_im();
  virtual void open_display_platform();
  virtual void offscreen_size(Fl_Offscreen off, int &width, int &height);
#if defined(FLTK_HIDPI_SUPPORT)  
  virtual APP_SCALING_CAPABILITY rescalable() {
    return PER_SCREEN_APP_SCALING;
  }
  virtual float scale(int n) {
    return scale_of_screen[n];
  }
  virtual void scale(int n, float f) {
    scale_of_screen[n] = f;
  }
#else
  float DWM_scaling_factor();
#endif
  virtual float desktop_scale_factor();
};


#endif // FL_WINAPI_SCREEN_DRIVER_H

//
// End of "$Id: Fl_WinAPI_Screen_Driver.H 12628 2018-01-09 07:26:49Z manolo $".
//
