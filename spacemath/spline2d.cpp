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

#include "spline2d.h"
#include "ap/spline3.h"

namespace spacemath {

   spline2d::spline2d()
   {}

   spline2d::spline2d(const vector<pos2d>& points)
   {
      compute_spline(points);
   }

   spline2d::~spline2d()
   {}

   bool spline2d::compute_spline(const vector<pos2d>& points)
   {
      // estimate parameter values for each of the points as the distance travelled along the curve
      int n = (int) points.size();

      // allocate the arrays
      ap::real_1d_array t,x,y;
      t.setbounds(0,n-1);
      x.setbounds(0,n-1);
      y.setbounds(0,n-1);

      // assign all array values
      double tsum = 0.0;
      t(0) = tsum;
      x(0) = points[0].x();
      y(0) = points[0].y();
      for(int i=1; i<n; i++) {
         tsum += points[i].dist(points[i-1]);
         t(i) = tsum;
         x(i) = points[i].x();
         y(i) = points[i].y();
      }

      // normalise the parameters [0,1]
      double scale = 1.0/tsum;
      for(int i=1; i<n; i++) t(i) *= scale;

      buildcubicspline(t,x,n,0,0.0,0,0.0,m_cx);
      buildcubicspline(t,y,n,0,0.0,0,0.0,m_cy);
      return true;
   }

   bool spline2d::compute_spline(const vector<pos2d>& points
                               , const vector<int>&    btx     // boundary types  x, [btx1,btx2]
                               , const vector<double>& bvx     // boundary values x, [bvx1,bvx2]
                               , const vector<int>&    bty     // boundary types  y, [bty1,bty2]
                               , const vector<double>& bvy     // boundary values y, [bvy1,bvy2]
                                  )
   {
      // estimate parameter values for each of the points as the distance travelled along the curve
      int n = (int) points.size();

      // allocate the arrays
      ap::real_1d_array t,x,y;
      t.setbounds(0,n-1);
      x.setbounds(0,n-1);
      y.setbounds(0,n-1);

      // assign all array values
      double tsum = 0.0;
      t(0) = tsum;
      x(0) = points[0].x();
      y(0) = points[0].y();
      for(int i=1; i<n; i++) {
         tsum += points[i].dist(points[i-1]);
         t(i) = tsum;
         x(i) = points[i].x();
         y(i) = points[i].y();
      }

      // normalise the parameters [0,1]
      double scale = 1.0/tsum;
      for(int i=1; i<n; i++) t(i) *= scale;

      buildcubicspline(t,x,n,btx[0],bvx[0],btx[1],bvx[1],m_cx);
      buildcubicspline(t,y,n,bty[0],bvy[0],bty[1],bvy[1],m_cy);

      return true;
   }

   pos2d spline2d::pos(double t) const
   {
      double x = splineinterpolation(m_cx,t);
      double y = splineinterpolation(m_cy,t);
      return pos2d(x,y);
   }

   vec2d spline2d::deriv1(double t) const
   {
      double x,dx,d2x;
      double y,dy,d2y;
      splinedifferentiation(m_cx,t,x,dx,d2x);
      splinedifferentiation(m_cy,t,y,dy,d2y);
      return vec2d(dx,dy);
   }

}
