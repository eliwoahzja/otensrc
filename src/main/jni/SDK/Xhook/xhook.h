#ifndef XHOOK_H
#define XHOOK_H 1

// XHook — https://github.com/iQiyi/xHook
// Copyright (c) 2018-present, iQIYI, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER    // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    // OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// xHook public API — reference implementation:
//   https://github.com/iQiyi/xHook
//
// Main.cpp calls these directly after the xHook sources are compiled and
// linked into the native module by Android.mk.
//
extern void  xhook_enable_debug(int on);
extern void  xhook_clear(void);
extern int   xhook_refresh(int do_stack_scan);
extern void  xhook_register(const char *pattern,
                           const char *symbol,
                           void *replace,
                           void **result);

#endif


