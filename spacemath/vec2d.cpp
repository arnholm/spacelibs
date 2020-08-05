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

#include "vec2d.h"
#include "pos2d.h"
#include <math.h>

namespace spacemath {

   vec2d::vec2d(double x,double y)
   : m_x(x),m_y(y)
   {}

   vec2d::vec2d()
   : m_x(0.0),m_y(0.0)
   {}

   vec2d::vec2d(const vec2d& vec)
   : m_x(vec.m_x),m_y(vec.m_y)
   {}

   vec2d::vec2d(const pos2d& start,const pos2d& end)
   : m_x(end.x()-start.x()),m_y(end.y()-start.y())
   {}

   vec2d::~vec2d()
   {
   }

   double vec2d::x() const
   {
      return m_x;
   }

   double vec2d::y() const
   {
      return m_y;
   }


   vec2d& vec2d::operator=(const vec2d& vec)
   {
      m_x = vec.m_x;
      m_y = vec.m_y;
      return *this;
   }

   int vec2d::operator<(const vec2d& vec) const
   {
      double mydist2    = m_x*m_x + m_y*m_y;
      double otherdist2 = vec.m_x*vec.m_x + vec.m_y*vec.m_y;
      return (mydist2<otherdist2);
   }

   vec2d& vec2d::normalise()
   {
      double len = length();
      if(len > 0.0) {
         m_x = m_x/len;
         m_y = m_y/len;
      }
      return *this;
   }

   vec2d& vec2d::scale(double factor)
   {
      m_x = m_x * factor;
      m_y = m_y * factor;
      return *this;
   }

   vec2d& vec2d::add(const vec2d& vec2)
   {
      m_x = m_x + vec2.m_x;
      m_y = m_y + vec2.m_y;
      return *this;
   }

   vec2d& vec2d::subtract(const vec2d& vec2)
   {
      m_x = m_x - vec2.m_x;
      m_y = m_y - vec2.m_y;
      return *this;
   }

   vec2d& vec2d::reverse()
   {
      m_x = - m_x;
      m_y = - m_y;
      return *this;
   }


   double vec2d::dot(const vec2d& vec2) const
   {
      return m_x*vec2.m_x + m_y*vec2.m_y;
   }

   double vec2d::cross(const vec2d& vec2) const
   {
      return m_x*vec2.y()-vec2.x()*m_y;
   }

   double vec2d::angle(const vec2d& vec2) const
   {
      vec2d vector1(m_x,m_y);
      vec2d vector2(vec2.m_x,vec2.m_y);
      vector1.normalise();
      vector2.normalise();
      double dotVal = vector1.dot( vector2 );
      if (dotVal>1.0) dotVal = 1.0;
      if (dotVal<-1.0) dotVal = -1.0;
      return acos( dotVal );
   }

   double vec2d::length() const
   {
      return sqrt(m_x*m_x + m_y*m_y);
   }

   vec2d  vec2d::normal(int sign) const
   {
      if(sign<0) return vec2d(-m_y,m_x);
      else return vec2d(m_y,-m_x);
   }

   vec2d& vec2d::operator+=(const vec2d& vec)
   {
      m_x+=vec.x();
      m_y+=vec.y();
      return *this;
   }

   vec2d& vec2d::operator-=(const vec2d& vec)
   {
      m_x-=vec.x();
      m_y-=vec.y();
      return *this;
   }

   double vec2d::operator%=(const vec2d& vec)
   {
      return m_x*vec.x()+m_y*vec.y();
   }

   vec2d& vec2d::operator*=(double factor)
   {
      m_x*=factor;
      m_y*=factor;
      return *this;
   }

   vec2d vec2d::operator -() const
   {
      return vec2d(-m_x,-m_y);
   }

   vec2d vec2d::operator+(const vec2d& vec)
   {
      return vec2d(m_x+vec.m_x,m_y+vec.m_y);
   }

   vec2d vec2d::operator-(const vec2d& vec)
   {
      return vec2d(m_x-vec.m_x,m_y-vec.m_y);
   }

}
