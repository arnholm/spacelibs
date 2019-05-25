// BeginLicense:
// Part of: spacelibs - reusable libraries for 3d space calculations
// Copyright (C) 2017 Carsten Arnholm
// All rights reserved
//
// This file may be used under the terms of either the GNU General
// Public License version 2 or 3 (at your option) as published by the
// Free Software Foundation and appearing in the files LICENSE.GPL2
// and LICENSE.GPL3 included in the packaging of this file.
//
// This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE.
// EndLicense:
#ifndef POLYHEALER_CONFIG_H_INCLUDED
#define POLYHEALER_CONFIG_H_INCLUDED

#ifdef _MSC_VER

   /// MSVC compiler

   #ifdef POLYHEALER_IMPLEMENTATION
      #define POLYHEALER_PUBLIC __declspec(dllexport)
      #define POLYHEALER_EXTERN
   #else
      #define POLYHEALER_PUBLIC __declspec(dllimport)
      #define POLYHEALER_EXTERN extern
   #endif
   #define POLYHEALER_PRIVATE

#elif __GNUC__

   /// gnu gcc

   #define POLYHEALER_PUBLIC  __attribute__ ((visibility("default")))
   #define POLYHEALER_PRIVATE __attribute__ ((visibility("hidden")))
   #define POLYHEALER_EXTERN
	
#else

  #error "Unknown compiler"
	
#endif


#endif // POLYHEALER_CONFIG_H_INCLUDED

