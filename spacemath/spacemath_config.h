// BeginLicense:
// Part of: spacelibs - reusable libraries for 3d space calculations
// Copyright (C) 2017-2020 Carsten Arnholm
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

#ifndef SPACEMATH_CONFIG_H_INCLUDED
#define SPACEMATH_CONFIG_H_INCLUDED

#ifdef _MSC_VER

   /// MSVC compiler
   // disabling warning 4251 is the only practical way to deal with dll-export issues according to M$
   #pragma warning (disable:4251) // class "XX" needs to have dll-interface to be used by clients of class "YY"

   #ifdef SPACEMATH_IMPLEMENTATION
      #define SPACEMATH_PUBLIC __declspec(dllexport)
      #define SPACEMATH_EXTERN
   #else
      #define SPACEMATH_PUBLIC __declspec(dllimport)
      #define SPACEMATH_EXTERN extern
   #endif
   #define SPACEMATH_PRIVATE

#elif __GNUC__

   /// gnu gcc

   #define SPACEMATH_PUBLIC  __attribute__ ((visibility("default")))
   #define SPACEMATH_PRIVATE __attribute__ ((visibility("hidden")))
   #define SPACEMATH_EXTERN

#else

  #error "Unknown compiler"

#endif


#endif // SPACEMATH_CONFIG_H_INCLUDED

