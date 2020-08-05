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

// plane3d.h: interface for the plane3d class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PLANE3D_H
#define PLANE3D_H

#include "spacemath_config.h"
#include "pos3d.h"
#include "vec3d.h"
#include "line3d.h"

// plane3d represents an infinite plane in 3d space
// plane equation parameters: A*x + B*y + C*z = D

namespace spacemath {

   class SPACEMATH_PUBLIC plane3d  {
   public:
      plane3d();
      plane3d(const pos3d& p1,    const pos3d& p2, const pos3d& p3); // 3 points in plane
      plane3d(const pos3d& point, const vec3d& normal);              // point in plane + normal vector
      plane3d(const double abcd[4], bool is_plane);
      virtual ~plane3d();

      // returns true if plane equation is ok
      bool is_ok() const {return m_is_plane;}

      // distance from point to plane
      double  dist(const pos3d& point) const;

      // project point onto plane
      pos3d   project(const pos3d& point) const;

      // intersect line/plane
      bool    intersect(const line3d& line, double& line_param) const;

      // intersect plane/plane
      bool    intersect(const plane3d& plane, line3d& line) const;

      // return plane normal
      vec3d   normal() const;

      // mirror pont around plane
      pos3d   mirror(const pos3d& in) const;

      // return plane equation ABCD
      const double* equation() const {return m_ABCD;}

      //Returns 0 if point on plane
      //Returns 1 if point on positive side of plane (direction of normal vector)
      //Returns -1 if otherwise.
      int side(const pos3d& pos) const;
   private:
      // plane equation parameters: A*x + B*y + C*z = D
      double m_ABCD[4];
      bool   m_is_plane;
   };

}

#endif
