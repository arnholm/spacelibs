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

#ifndef CIRCLE2D_H
#define CIRCLE2D_H

#include "spacemath_config.h"
#include "pos2d.h"
#include <vector>
using std::vector;

namespace spacemath {
   class line2d;

   // circle in 2d space
   class SPACEMATH_PUBLIC circle2d {
   public:
      circle2d();
      circle2d(const pos2d& center, double radius);
      circle2d(const pos2d& p1, const pos2d& p2, const pos2d& p3);
      virtual ~circle2d();

      // compute the circle from 3 points, return center and radius
      static void circle_3p(const pos2d& p1, const pos2d& p2,const pos2d& p3, pos2d& center, double& radius);

      // return center and radius of circle
      const pos2d& center() const    { return m_center; }
      double radius() const          { return m_radius; }

      // check if position is inside circle
      bool pos_inside(const pos2d& p, double epspnt) const { return (m_center.dist(p) <= (m_radius+epspnt) ); }

      // simplified intersection returning only true/false
      bool intersect(const line2d& line, bool infinite_line) const;

      // full intersection with intersection vector returned
      // size() == 0 : no intersection
      // size() == 1 : tangent
      // size() == 2 : intersection
      vector<pos2d> intersect_ex(const line2d& line, bool infinite_line) const;

      // find tangent points on circle, given a point outside the circle.
      // if p is not outside the circle, an empty vector is returned
      vector<pos2d> tangent_points(const pos2d& p) const;

      // find external tangent lines between this circle and c2
      // if no external tangents exist, an empty vector is returned.
      // The returned lines do not have any guaranteed directions
      vector<line2d> external_tangents(const circle2d& c2) const;

   protected:
      line2d external_tangent(const circle2d& ca, const pos2d& tpa, const vector<pos2d>& tpb) const;

   private:
      pos2d  m_center;
      double m_radius;
   };

}

#endif // CIRCLE2D_H
