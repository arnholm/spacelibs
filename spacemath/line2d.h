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

#ifndef LINE2D_H
#define LINE2D_H

#include "spacemath_config.h"
#include "pos2d.h"
#include <utility>

namespace spacemath {

   // line segment in 2d space
   class SPACEMATH_PUBLIC line2d {
   public:
      line2d();
      line2d(const pos2d& end1, const pos2d& end2);
      line2d(const line2d& line);
      virtual ~line2d();

      line2d& operator =(const line2d& line);

      const pos2d& end1() const;
      const pos2d& end2() const;

      double length() const;

      /// parameter (0.0 at end1 and 1.0 at end2), i.e. relative distance from end1
      /// This function also allows extrapolation beyond line ends
      pos2d  interpolate(double par) const;

      /// project point onto line and return parameter value (0.0 at end1 and 1.0 at end2)
      /// The function may return parameter outside the range [0,1]
      double project(const pos2d& point) const;

      /// compute line/line intersection and return intersection point.
      /// Observe that intersection point may lie anywhere beyond the line endpoints.
      /// The function returns false if the lines are parallel
      bool   intersect(const line2d& other_line, pos2d& pos) const;

      /// compute line/line intersection and return intersection parameters in range
      /// If both parameters are in the range [0,1], there is an actual intersection of finite length segments.
      /// The function returns false if the lines are parallel
      bool   intersect(const line2d& other_line, pos2d& pos,double& this_param,double& other_param) const;

      // construct rotated line, relative to this line
      // par      : Parameter on this line where new line starts (end1)
      // ccw_angle: Relative angle in radians, counterclockwise from this line. Defines direction of new line
      // length   : Length of new line
      line2d rotated_line(double par, double ccw_angle, double length) const;

   private:
      pos2d m_end1;
      pos2d m_end2;
   };

}

#endif

