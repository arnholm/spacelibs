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
   
#include "pos2d.h"
#include <math.h>

namespace spacemath {

   pos2d::pos2d(double x,double y)
   : m_x(x),m_y(y)
   {}

   pos2d::pos2d()
   : m_x(0.0),m_y(0.0)
   {}

   pos2d::pos2d(const pos2d& pos)
   : m_x(pos.m_x),m_y(pos.m_y)
   {}

   pos2d::~pos2d()
   {
   }

   double pos2d::x() const
   {
      return m_x;
   }

   double pos2d::y() const
   {
      return m_y;
   }

   void pos2d::x(double x)
   {
      m_x = x;
   }

   void pos2d::y(double y)
   {
      m_y = y;
   }


   pos2d& pos2d::operator=(const pos2d& pos)
   {
      m_x = pos.m_x;
      m_y = pos.m_y;
      return *this;
   }


   double pos2d::dist(const pos2d& pos) const
   {
      double dx = pos.m_x - m_x;
      double dy = pos.m_y - m_y;
      return sqrt(dx*dx + dy*dy);
   }

   double pos2d::dist_squared(const pos2d& pos) const
   {
      double dx = pos.m_x - m_x;
      double dy = pos.m_y - m_y;
      return (dx*dx + dy*dy);
   }

   pos2d& pos2d::operator+=(const vec2d& vec)
   {
       m_x+=vec.x();
       m_y+=vec.y();
       return *this;
   }

   pos2d& pos2d::operator-=(const vec2d& vec)
   {
       m_x-=vec.x();
       m_y-=vec.y();
       return *this;
   }

   pos2d& pos2d::operator+=(const pos2d& pos)
   {
       m_x+=pos.x();
       m_y+=pos.y();
       return *this;
   }

   pos2d& pos2d::operator-=(const pos2d& pos)
   {
       m_x-=pos.x();
       m_y-=pos.y();
       return *this;
   }

}
