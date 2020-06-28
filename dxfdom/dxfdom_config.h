#ifndef DXFDOM_CONFIG_H_INCLUDED
#define DXFDOM_CONFIG_H_INCLUDED

#ifdef _MSC_VER

   /// MSVC compiler
   // disabling warning 4251 is the only practical way to deal with dll-export issues according to M$
   #pragma warning (disable:4251) // class "XX" needs to have dll-interface to be used by clients of class "YY"

   #ifdef DXFDOM_IMPLEMENTATION
      #define DXFDOM_PUBLIC __declspec(dllexport)
      #define DXFDOM_EXTERN
   #else
      #define DXFDOM_PUBLIC __declspec(dllimport)
      #define DXFDOM_EXTERN extern
   #endif
   #define DXFDOM_PRIVATE

#elif __GNUC__

   /// gnu gcc

   #define DXFDOM_PUBLIC  __attribute__ ((visibility("default")))
   #define DXFDOM_PRIVATE __attribute__ ((visibility("hidden")))
   #define DXFDOM_EXTERN

#else

  #error "Unknown compiler"

#endif


#endif // DXFDOM_CONFIG_H_INCLUDED

