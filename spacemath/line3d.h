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

#ifndef LINE3D_H
#define LINE3D_H

#include "spacemath_config.h"
#include "pos3d.h"

namespace spacemath {

   // line segment in 3d space
   class SPACEMATH_PUBLIC line3d {
   public:
      line3d();
      line3d(const pos3d& end1, const pos3d& end2);
      line3d(const line3d& line);
      virtual ~line3d();

      line3d& operator =(const line3d& line);

      const pos3d& end1() const;
      const pos3d& end2() const;
      double length() const;

       // parameter in interval [0.0, 1.0], i.e. relative distance from end1
      pos3d  interpolate(double par) const;

      // project point onto line and return parameter value (0.0 at end1 and 1.0 at end2)
      double project(const pos3d& point) const;

      // Find closest points on two non-paralell lines, returns true if not parallel (i.e. result is possible)
      bool   intersect(const line3d& other_line, double& this_param, double& other_param) const;

   private:
      pos3d m_end1;
      pos3d m_end2;
   };

}

#endif

