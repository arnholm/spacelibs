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

#ifndef BSPLINE2D_H
#define BSPLINE2D_H

#include "spacemath_config.h"

#include "pos2d.h"
#include <vector>
#include <memory>
#include <utility>
using std::vector;

namespace tinyspline {
   class BSpline;
}

namespace spacemath {

   // bspline2d is a B-spline curve defined by control points and knot vector.
   // Such spline curves exist in e.g. DXF files
   class SPACEMATH_PUBLIC bspline2d {
   public:
      bspline2d(const std::vector<pos2d>&  ctrl_pnts,
                const std::vector<double>& knots,
                size_t degree);
      virtual ~bspline2d();

      // obtain the min and max parameter values (see evaluate below)
      std::pair<double,double> range() const;

      // evaluate the spline in parameter position
      // the parameter position must be within [min,max] range as returned by range()
      pos2d evaluate(double u) const;

      // return the points on the spline curve corresponding to the knot positions
      std::vector<pos2d> knot_points() const;

      // return the points on the spline curve corresponding to the knot positions
      // plus n positions between each knot pair
      std::vector<pos2d> interpolated_points(size_t n) const;

   protected:
      static std::shared_ptr<tinyspline::BSpline>  compute_spline(const std::vector<pos2d>&  ctrl_pnts,
                                                                  const std::vector<double>& knots,
                                                                  size_t degree);
   private:
      std::vector<pos2d>                   m_cp;     // control points
      std::vector<double>                  m_kn;     // knot vector
      size_t                               m_degree; // degreee of spline (typicaly 2 or 3)
      std::shared_ptr<tinyspline::BSpline> m_spline;
   };

}
#endif // BSPLINE2D_H

