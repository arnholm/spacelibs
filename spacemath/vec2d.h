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

#ifndef VEC2D_H
#define VEC2D_H

#include "spacemath_config.h"

namespace spacemath {
   class pos2d;

   // vector in 2d space
   class SPACEMATH_PUBLIC vec2d {
   public:
      vec2d();
      vec2d(double x,double y);
      vec2d(const pos2d& start,const pos2d& end);
      vec2d(const vec2d& vec);
     ~vec2d();

      double x() const;
      double y() const;

      vec2d& operator =(const vec2d& vec);
      int    operator <(const vec2d& vec) const;
      vec2d  operator -() const;

      vec2d& normalise();
      vec2d& scale(double factor);
      vec2d& add(const vec2d& vec2);
      vec2d& subtract(const vec2d& vec2);
      vec2d& reverse();

      double dot(const vec2d& vec2) const;
      double cross(const vec2d& vec2) const;
      double angle(const vec2d& vec2) const;
      double length() const;

      vec2d  normal(int sign) const;

   //Vector arithmetic
      vec2d& operator+=(const vec2d& vec);
      vec2d& operator-=(const vec2d& vec);

      double operator%=(const vec2d& vec);    //Scalar product

      vec2d& operator*=(double factor);       //Scaling
      vec2d operator+(const vec2d& vec);
      vec2d operator-(const vec2d& vec);

      friend vec2d operator*(const vec2d& vec,double factor);
      friend vec2d operator*(double factor,const vec2d& vec);
      friend vec2d normalise(const vec2d& vec);

   private:
      double m_x;
      double m_y;
   };

   inline vec2d normalise(const vec2d& vec)                      { return vec2d(vec).normalise();                     }
   inline vec2d operator*(const vec2d& vec, double value)        { return vec2d(vec.x()*value,vec.y()*value);         }
   inline vec2d operator*(double value, const vec2d& vec)        { return vec2d(value*vec.x(),value*vec.y());         }
   inline vec2d operator+(const vec2d& vec1, const vec2d& vec2)  { return vec2d(vec1.x()+vec2.x(),vec1.y()+vec2.y()); }
   inline vec2d operator-(const vec2d& vec1, const vec2d& vec2)  { return vec2d(vec1.x()-vec2.x(),vec1.y()-vec2.y()); }

}

#endif

