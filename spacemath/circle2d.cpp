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

#include "circle2d.h"
#include "vec2d.h"
#include "line2d.h"
#include <cmath>

static const double pi = 4.0*atan(1.0);

namespace spacemath {

   circle2d::circle2d()
   : m_radius(0.0)
   {}

   circle2d::circle2d(const pos2d& center, double radius)
   : m_center(center)
   , m_radius(radius)
   { }

   circle2d::circle2d(const pos2d& p1, const pos2d& p2, const pos2d& p3)
   {
      circle_3p(p1,p2,p3,m_center,m_radius);
   }


   circle2d::~circle2d()
   {}

   void circle2d::circle_3p(const pos2d& p1, const pos2d& p2,const pos2d& p3, pos2d& center, double& radius)
   {
      // https://en.wikipedia.org/wiki/Circumscribed_circle#Cartesian_coordinates_from_cross-_and_dot-products

      vec2d p1p2(p2,p1);
      vec2d p2p3(p3,p2);
      vec2d p3p1(p1,p3);

      vec2d p1p3(p3,p1);
      vec2d p2p1(p1,p2);
      vec2d p3p2(p2,p3);

      double dp1p2 = p1p2.length();
      double dp2p3 = p2p3.length();
      double dp3p1 = p3p1.length();
      double dp1p3 = dp3p1;

      radius = fabs(0.5*(dp1p2*dp2p3*dp3p1)/p1p2.cross(p2p3));

      double cross = p1p2.cross(p2p3);
      double denom = 2.0* cross*cross;

      double alpha = dp2p3*dp2p3*p1p2.dot(p1p3)/denom;
      double beta  = dp1p3*dp1p3*p2p1.dot(p2p3)/denom;
      double gamma = dp1p2*dp1p2*p3p1.dot(p3p2)/denom;

      double x = alpha*p1.x() + beta*p2.x() + gamma*p3.x();
      double y = alpha*p1.y() + beta*p2.y() + gamma*p3.y();

      center = pos2d(x,y);
   }

   bool circle2d::intersect(const line2d& line, bool infinite_line) const
   {
      vector<pos2d> xsect = intersect_ex(line,infinite_line);
      return (xsect.size()>0);
   }

   vector<pos2d> circle2d::intersect_ex(const line2d& line, bool infinite_line) const
   {
      // http://mathworld.wolfram.com/Circle-LineIntersection.html
      vector<pos2d>  result;

      const pos2d& p1 = line.end1();
      const pos2d& p2 = line.end2();

      // the wolfram formulae assume circle is at (0,0)
      // so we must transform the line into a local circle coordinate system
      // before proceeding.
      // We also have to adjust the computed intersection points back to the
      // original system with circle offset from origin

      double x1  = p1.x() - m_center.x();
      double y1  = p1.y() - m_center.y();
      double x2  = p2.x() - m_center.x();
      double y2  = p2.y() - m_center.y();

      double dx  = x2 - x1;
      double dy  = y2 - y1;
      double dr2 = dx*dx + dy*dy;   //  double dr  = sqrt(dr2);
      double D   = x1*y2 - x2*y1;

      double delta  = m_radius*m_radius*dr2 - D*D;

      if(delta >= 0.0) {
         // tangent or 2-point intersection

         double sgn_dy   = (dy < 0.0)? -1 : 1;
         double sq_delta = sqrt(delta);

         // first point
         double xr1 = (+D*dy + sgn_dy*dx*sq_delta)/dr2 + m_center.x();
         double yr1 = (-D*dx + fabs(dy)*sq_delta)/dr2  + m_center.y();

         pos2d pr1(xr1,yr1);
         bool on_line = true;
         if(!infinite_line) {
            double par = line.project(pr1);
            on_line = (par >= 0.0) && (par <= 1.0);
         }
         if(on_line) result.push_back(pr1);

         if( delta > 0.0) {
            //  2-point intersection (secant)
            double xr2 = (+D*dy - sgn_dy*dx*sq_delta)/dr2 + m_center.x();
            double yr2 = (-D*dx - fabs(dy)*sq_delta)/dr2  + m_center.y();
            pos2d pr2(xr2,yr2);
            on_line = true;
            if(!infinite_line) {
               double par = line.project(pr2);
               on_line = (par >= 0.0) && (par <= 1.0);
            }
            if(on_line) result.push_back(pr2);
         }
      }
      return std::move(result);
   }


   vector<pos2d> circle2d::tangent_points(const pos2d& p) const
   {
      // https://stackoverflow.com/questions/1351746/find-a-tangent-point-on-circle
      // http://jsfiddle.net/zxqCw/1/

      vector<pos2d>  result;

      // check that point is outside circle
      double dist = p.dist(m_center);
      if(dist > m_radius) {

         vec2d  dv = m_center - p;
         double  a = asin(m_radius/dv.length());
         double  b = atan2(dv.y(),dv.x());

         double t1 = b - a;
         vec2d  v1(m_radius*sin(t1),-m_radius*cos(t1));

         double t2 = b + a;
         vec2d  v2(-m_radius*sin(t2),m_radius*cos(t2));

         result.push_back(m_center+v1);
         result.push_back(m_center+v2);

      }
      return std::move(result);
   }

   vector<line2d> circle2d::external_tangents(const circle2d& c2) const
   {
      /*

         https://gieseanw.wordpress.com/2012/09/12/finding-external-tangent-points-for-two-circles/

         given 2 circles c1 and c2, we want to find the external tangent lines.

         a) for non-congruent circles (c1.radius != c2.radius) determine the circle ca with largest radius
            ca = (c1.radius > c2.radius)? c1 : c2:
            cb = (c1.radius > c2.radius)? c2 : c1:

            then compute r3 = ca.radius - cb.radius;

            Construct new circle c3(ca.center,r3);
            Construct c3 tangent points using cb.center as external points.
            Compute ca intersection points using c3 center and ca tangent points. These are the ca tangent points.
            Compute cb tangent points using ca tangent points.
            Select cb tangent points with greates distance from ca.center() to obtain external tangents

         b) If the circles are congruent (c1.radius == c2.radius) we compute perpendicular
            lines through circle centers and obtain the intersection points. Finished.

         c) if circles overlap 100% or one is contained in the other, there are no external tangents
            and an empty vector is returned

      */


      vector<line2d> tangents;

      // line between centers
      line2d lcen(m_center,c2.m_center);
      double dist = lcen.length();
      if(dist > 0.0) {
         // circle centers in different positions

         double dr = fabs(m_radius - c2.m_radius);
         if(dr > 0.0) {
            // case a: the circles have different size (radius)
            const circle2d& ca = (m_radius > c2.m_radius)?  *this :  c2;    // large circle
            const circle2d& cb = (m_radius > c2.m_radius)?   c2   : *this;  // small circle

            // the small circle cb must have some part outside the large circle ca
            if(dist + cb.radius() > ca.radius()) {
               // tangents exist

               // helper circle provides directions to ca tangent points
               double r3 = ca.radius() - cb.radius();
               circle2d c3(ca.center(),r3);
               vector<pos2d> tp3 = c3.tangent_points(cb.center());
               if(tp3.size() > 1) {

                  // extrapolate the lines to the ca radius
                  line2d l3a(c3.center(),tp3[0]);
                  line2d l3b(c3.center(),tp3[1]);
                  double par = ca.radius()/r3;

                  // tangent points on circle ca
                  vector<pos2d> tpa = { l3a.interpolate(par), l3b.interpolate(par) };

                  // tangent points on circle cb
                  vector<pos2d> tpb0 = cb.tangent_points(tpa[0]);
                  vector<pos2d> tpb1 = cb.tangent_points(tpa[1]);

                  // filter out the external tangent lines
                  tangents.push_back(external_tangent(ca,tpa[0],tpb0));
                  tangents.push_back(external_tangent(ca,tpa[1],tpb1));
               }
            }
         }
         else {
            // case b: same radii, different center positions
            // in this case we are certain the tangents exist
            double ccw_angle = 0.5*pi;
            line2d l1 = lcen.rotated_line(0.0,ccw_angle,m_radius);
            line2d l2 = lcen.rotated_line(1.0,ccw_angle,m_radius);
            tangents.push_back(line2d(l1.interpolate(+1.0),l2.interpolate(+1.0)));
            tangents.push_back(line2d(l1.interpolate(-1.0),l2.interpolate(-1.0)));
         }
      }

      return tangents;
   }

   line2d circle2d::external_tangent(const circle2d& ca, const pos2d& tpa, const vector<pos2d>& tpb) const
   {
      // ca    = largest circle
      // tpa   = tangent point on circle ca
      // tpb[] = vector with 2 candidate tangent points on circle cb (smallest circle)

      // we are interested in the line from tpa to either tpb[0] or tpb[0]
      // depending on which has greater distance from ca.center()

      const pos2d& pa = ca.center();

      return (pa.dist(tpb[0]) > pa.dist(tpb[1]))? line2d(tpa,tpb[0]) : line2d(tpa,tpb[1]) ;
   }


}
