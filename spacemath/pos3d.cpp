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
   
#include "pos3d.h"
#include "vec3d.h"
#include "HTmatrix.h"
#include <math.h>

namespace spacemath {

   pos3d& pos3d::operator=(const pos3d& pos)
   {
      if(this!=&pos) {
         m_x = pos.m_x;
         m_y = pos.m_y;
         m_z = pos.m_z;
      }
      return *this;
   }

   pos3d& pos3d::operator+=(const vec3d& vec)
   {
       m_x+=vec.x();
       m_y+=vec.y();
       m_z+=vec.z();
       return *this;
   }

   pos3d& pos3d::operator-=(const vec3d& vec)
   {
       m_x-=vec.x();
       m_y-=vec.y();
       m_z-=vec.z();
       return *this;
   }

   pos3d& pos3d::move(const vec3d& vec)
   {
      m_x += vec.x();
      m_y += vec.y();
      m_z += vec.z();
      return *this;
   }


   double pos3d::dist(const pos3d& pos) const
   {
      double dx = pos.m_x - m_x;
      double dy = pos.m_y - m_y;
      double dz = pos.m_z - m_z;
      return sqrt(dx*dx + dy*dy + dz*dz);
   }

   double pos3d::dist_squared(const pos3d& pos) const
   {
      double dx = pos.m_x - m_x;
      double dy = pos.m_y - m_y;
      double dz = pos.m_z - m_z;
      return (dx*dx + dy*dy + dz*dz);
   }

   pos3d operator*(const HTmatrix& T, const pos3d& pos)
   {
      const vmath::mat4<double>& t = T.detail();
      vmath::vec3<double> r=transform_point(t,vmath::vec3<double>(pos.m_x,pos.m_y,pos.m_z ) );
      return pos3d(r[0],r[1],r[2]);
   }

}
