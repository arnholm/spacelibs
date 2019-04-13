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
   
#ifndef A_SPLINE2D_H
#define A_SPLINE2D_H

#include "ap/ap.h"

#include <vector>
using namespace std;
#include "pos2d.h"

namespace spacemath {

   using namespace std;

   // spline2d expresses a spline curve in 2d space
   // so that (x,y) = pos(t), where t=[0,1]
   // t=0 corresponds to first point given
   // t=1 corresponds to last point given

   class SPACEMATH_PUBLIC spline2d {
   public:
      spline2d();
      spline2d(const vector<pos2d>& points);
      virtual ~spline2d();

      // compute the spline from a vector of points
      bool compute_spline(const vector<pos2d>& points);

      // return position using parameter [0,1]
      pos2d pos(double t) const;

      // return 1st derivative vector using parameter [0,1]
      vec2d deriv1(double t) const;

   private:
      ap::real_1d_array m_cx;
      ap::real_1d_array m_cy;
   };

}

#endif // SPLINE2D_H
