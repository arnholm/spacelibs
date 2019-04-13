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
   
#include "spline3d.h"
#include "ap/spline3.h"

namespace spacemath {

   spline3d::spline3d()
   {}

   spline3d::spline3d(const vector<pos3d>& points)
   {
      compute_spline(points);
   }

   spline3d::~spline3d()
   {}

   bool spline3d::compute_spline(const vector<pos3d>& points)
   {
      // estimate parameter values for each of the points as the distance travelled along the curve
      int n = (int) points.size();

      // allocate the arrays
      ap::real_1d_array t,x,y,z;
      t.setbounds(0,n-1);
      x.setbounds(0,n-1);
      y.setbounds(0,n-1);
      z.setbounds(0,n-1);

      // assign all array values
      double tsum = 0.0;
      t(0) = tsum;
      x(0) = points[0].x();
      y(0) = points[0].y();
      z(0) = points[0].z();
      for(int i=1; i<n; i++) {
         tsum += points[i].dist(points[i-1]);
         t(i) = tsum;
         x(i) = points[i].x();
         y(i) = points[i].y();
         z(i) = points[i].z();
      }

      // normalise the parameters [0,1]
      double scale = 1.0/tsum;
      for(int i=1; i<n; i++) t(i) *= scale;

      buildcubicspline(t,x,n,0,0.0,0,0.0,m_cx);
      buildcubicspline(t,y,n,0,0.0,0,0.0,m_cy);
      buildcubicspline(t,z,n,0,0.0,0,0.0,m_cz);
      return true;
   }

   pos3d spline3d::pos(double t) const
   {
      double x = splineinterpolation(m_cx,t);
      double y = splineinterpolation(m_cy,t);
      double z = splineinterpolation(m_cz,t);
      return pos3d(x,y,z);
   }

   vec3d spline3d::deriv1(double t) const
   {
      double x,dx,d2x;
      double y,dy,d2y;
      double z,dz,d2z;
      splinedifferentiation(m_cx,t,x,dx,d2x);
      splinedifferentiation(m_cy,t,y,dy,d2y);
      splinedifferentiation(m_cz,t,z,dz,d2z);
      return vec3d(dx,dy,dz);
   }

}
