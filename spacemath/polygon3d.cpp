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

#include "polygon3d.h"
#include "vec3d.h"


namespace spacemath {

   polygon3d::polygon3d()
   {}

   polygon3d::polygon3d(const vector<pos3d>& vert)
   : m_vert(vert)
   {}

   polygon3d::~polygon3d()
   {}

   bool polygon3d::is_valid() const
   {
      return (m_vert.size()>2);
   }

   void polygon3d::push_back(const pos3d& pos)
   {
      m_vert.push_back(pos);
   }

   polygon3d::const_iterator polygon3d::begin() const
   {
      return m_vert.begin();
   }

   polygon3d::const_iterator polygon3d::end() const
   {
      return m_vert.end();
   }

   const pos3d& polygon3d::position(size_t ipos) const
   {
      return m_vert[ipos];
   }

   size_t polygon3d::size() const
   {
      return m_vert.size();
   }

   double polygon3d::area(vec3d& normal) const
   {
      // http://thebuildingcoder.typepad.com/blog/2008/12/3d-polygon-areas.html

      // zero the normal vector
      normal = vec3d();

      size_t n = m_vert.size();
      pos3d b  = m_vert[n-2];
      pos3d c  = m_vert[n-1];
      for(size_t i=0; i<n; i++ ) {

         pos3d a = b;
         b       = c;
         c       = m_vert[i];

         double dx = b.y() * (c.z() - a.z());
         double dy = b.z() * (c.x() - a.x());
         double dz = b.x() * (c.y() - a.y());

         normal += vec3d(dx,dy,dz);
      }
      double length = normal.length();
      normal.normalise();

      double area = 0.5*length;
      return area;
   }

   double polygon3d::area() const
   {
      vec3d normal;
      return area(normal);
   }

}
