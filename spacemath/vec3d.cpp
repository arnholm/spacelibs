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

#include "vec3d.h"
#include "pos3d.h"
#include <math.h>
#include "HTmatrix.h"

namespace spacemath {

   vec3d::vec3d(const pos3d& start,const pos3d& end)
   : m_x(end.x()-start.x()),m_y(end.y()-start.y()),m_z(end.z()-start.z())
   {}

   vec3d& vec3d::operator=(const vec3d& vec)
   {
      m_x = vec.m_x;
      m_y = vec.m_y;
      m_z = vec.m_z;
      return *this;
   }

   vec3d& vec3d::normalise()
   {
      double len = length();
      if(len > 0.0) {
         m_x = m_x/len;
         m_y = m_y/len;
         m_z = m_z/len;
      }
      return *this;
   }

   vec3d& vec3d::scale(double factor)
   {
      m_x = m_x * factor;
      m_y = m_y * factor;
      m_z = m_z * factor;
      return *this;
   }

   vec3d& vec3d::add(const vec3d& vec2)
   {
      m_x = m_x + vec2.m_x;
      m_y = m_y + vec2.m_y;
      m_z = m_z + vec2.m_z;
      return *this;
   }

   vec3d& vec3d::subtract(const vec3d& vec2)
   {
      m_x = m_x - vec2.m_x;
      m_y = m_y - vec2.m_y;
      m_z = m_z - vec2.m_z;
      return *this;
   }

   vec3d& vec3d::reverse()
   {
      m_x = - m_x;
      m_y = - m_y;
      m_z = - m_z;
      return *this;
   }

   vec3d vec3d::cross(const vec3d& vec2) const
   {
      double x = m_y*vec2.m_z - m_z*vec2.m_y;
      double y = m_z*vec2.m_x - m_x*vec2.m_z;
      double z = m_x*vec2.m_y - m_y*vec2.m_x;
      return vec3d(x,y,z);
   }

   double vec3d::dot(const vec3d& vec2) const
   {
      return m_x*vec2.m_x + m_y*vec2.m_y + m_z*vec2.m_z;
   }

   double vec3d::angle(const vec3d& vec2) const
   {
      vec3d vector1(m_x,m_y,m_z);
      vec3d vector2(vec2.m_x,vec2.m_y,vec2.m_z);
      vector1.normalise();
      vector2.normalise();
      double dotVal = vector1.dot( vector2 );
      if (dotVal>1.0) dotVal = 1.0;
      if (dotVal<-1.0) dotVal = -1.0;
      return acos( dotVal );
   }

   double vec3d::length() const
   {
      return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
   }

   double vec3d::squareLength() const
   {
      return m_x*m_x+m_y*m_y+m_z*m_z;
   }

   double operator%(const vec3d& vec1,const vec3d& vec2)
   {
       return vec1.x()*vec2.x() + vec1.y()*vec2.y() + vec1.z()*vec2.z();
   }

   bool vec3d::isParallel(const vec3d& vec, double angle_tol) const
   {
      if(fabs(angle(vec)) <= angle_tol)return true;
      if(fabs(angle(-vec)) <= angle_tol)return true;

      return false;
   }

   vec3d operator*(const HTmatrix& T, const vec3d& vec)
   {
      vmath::vec3<double> r=transform_vector_transpose(T.detail(),vmath::vec3<double>(vec.m_x,vec.m_y,vec.m_z) );
      return vec3d(r[0],r[1],r[2]);
   }

}
