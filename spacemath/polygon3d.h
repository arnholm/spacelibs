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

#ifndef POLYGON3D_H
#define POLYGON3D_H

#include "spacemath_config.h"
#include <vector>
using std::vector;
#include "pos3d.h"
#include <cstddef>

namespace spacemath {

   // polygon in 2d space, it is assumed flat, but this is not checked
   class SPACEMATH_PUBLIC polygon3d {
   public:
      typedef vector<pos3d>::const_iterator const_iterator;

      polygon3d();
      polygon3d(const vector<pos3d>& vert);
      virtual ~polygon3d();

      /// add a position to the back of the boundary loop. Don't overuse this
      void push_back(const pos3d& pos);

      /// a polygon2d is valid if it has at least 3 vertices
      bool is_valid() const;

      /// return the number of points in the boundary loop
      size_t size() const;

      /// return position ipos. Range [0,size()-1]
      const pos3d& position(size_t ipos) const;

      // iteration over the boundary loop
      const_iterator begin() const;
      const_iterator end() const;

      // return polygon area and unit normal vector
      double area(vec3d& normal) const;

      // return area only
      double area() const;

   private:
      vector<pos3d> m_vert;
   };

}

#endif // POLYGON3D_H
