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

#ifndef POS2D_H
#define POS2D_H

#include "spacemath_config.h"
#include "vec2d.h"

namespace spacemath {

   // position in 2d space
   class SPACEMATH_PUBLIC pos2d {
   public:
      pos2d();
      pos2d(double x,double y);
      pos2d(const pos2d& pos);
     ~pos2d();

      double x() const;
      double y() const;

      void x( double x );
      void y( double y );

      pos2d& operator =(const pos2d& pos);
      pos2d& operator+=(const vec2d& vec);
      pos2d& operator-=(const vec2d& vec);

      pos2d& operator+=(const pos2d& pos);
      pos2d& operator-=(const pos2d& pos);

      double dist(const pos2d& pos) const;
      double dist_squared(const pos2d& pos) const;

      friend pos2d operator+(const pos2d& pos, const vec2d& vec);
      friend pos2d operator+(const vec2d& vec, const pos2d& pos);

      friend pos2d operator-(const pos2d& pos, const vec2d& vec);
      friend pos2d operator-(const vec2d& vec, const pos2d& pos);

      friend pos2d operator*(const pos2d& pos, double value);
      friend pos2d operator*(double value, const pos2d& pos);

      friend vec2d operator-(const pos2d& pos1, const pos2d& pos2);

   private:
      double m_x;
      double m_y;
   };


   //Operators
   inline pos2d operator+(const pos2d& pos, const vec2d& vec)   { return pos2d(pos.m_x + vec.x(), pos.m_y + vec.y());   }
   inline pos2d operator+(const vec2d& vec, const pos2d& pos)   { return pos2d(pos.m_x + vec.x(), pos.m_y + vec.y());   }
   inline pos2d operator-(const pos2d& pos, const vec2d& vec)   { return pos2d(pos.m_x - vec.x(), pos.m_y - vec.y());   }
   inline pos2d operator-(const vec2d& vec, const pos2d& pos)   { return pos2d(-pos.m_x + vec.x(), -pos.m_y + vec.y()); }
   inline pos2d operator*(const pos2d& pos, double value)       { return pos2d(pos.x()*value,pos.y()*value); }
   inline pos2d operator*(double value, const pos2d& pos)       { return pos2d(value*pos.x(),value*pos.y()); }
   inline vec2d operator-(const pos2d& pos1, const pos2d& pos2) { return vec2d(pos2,pos1); }

   inline pos2d operator+(const pos2d& pos1, const pos2d& pos2) {return pos2d(pos1)+=pos2;}

}

#endif
