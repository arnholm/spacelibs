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
   
#ifndef SPACEIO_CONFIG_H_INCLUDED
#define SPACEIO_CONFIG_H_INCLUDED

#ifdef _MSC_VER

   /// MSVC compiler
   // disabling warning 4251 is the only practical way to deal with dll-export issues according to M$
   #pragma warning (disable:4251) // class "XX" needs to have dll-interface to be used by clients of class "YY"

   #ifdef SPACEIO_IMPLEMENTATION
      #define SPACEIO_PUBLIC __declspec(dllexport)
      #define SPACEIO_EXTERN
   #else
      #define SPACEIO_PUBLIC __declspec(dllimport)
      #define SPACEIO_EXTERN extern
   #endif
   #define SPACEIO_PRIVATE

#elif __GNUC__

   /// gnu gcc

   #define SPACEIO_PUBLIC  __attribute__ ((visibility("default")))
   #define SPACEIO_PRIVATE __attribute__ ((visibility("hidden")))
   #define SPACEIO_EXTERN

#else

  #error "Unknown compiler"

#endif


#endif // SPACEIO_CONFIG_H_INCLUDED

