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
   
#include "line2d.h"
#include <math.h>
#include <limits>

namespace spacemath {

   line2d::line2d()
   : m_end1(pos2d(0.0,0.0)),m_end2(pos2d(0.0,0.0))
   {}

   line2d::line2d(const pos2d& end1, const pos2d& end2)
   : m_end1(end1),m_end2(end2)
   {}

   line2d::line2d(const line2d& line)
   : m_end1(line.m_end1),m_end2(line.m_end2)
   {}

   line2d::~line2d()
   {}

   line2d& line2d::operator =(const line2d& line)
   {
      m_end1 = line.m_end1;
      m_end2 = line.m_end2;
      return *this;
   }

   const pos2d& line2d::end1() const
   {
      return m_end1;
   }

   const pos2d& line2d::end2() const
   {
      return m_end2;
   }

   double line2d::length() const
   {
      return m_end1.dist(m_end2);
   }

   pos2d line2d::interpolate(double par) const
   {
      // parameter in interval [0.0, 1.0], i.e. relative distance from end1
      double x = m_end1.x() + par * ( m_end2.x() - m_end1.x() );
      double y = m_end1.y() + par * ( m_end2.y() - m_end1.y() );
      return pos2d(x,y);
   }

   double det(const vec2d& ab,const vec2d& cd) {
      return ab.x()*cd.y()-ab.y()*cd.x();
   }

   double det(const pos2d& ab,const pos2d& cd) {
      return ab.x()*cd.y()-ab.y()*cd.x();
   }

   bool line2d::intersect(const line2d& other_line, pos2d& pos) const
   {
      double below=det(vec2d(end2(),end1()),vec2d(other_line.end2(),other_line.end1()));

      if(fabs(below) > 1.0e-6) {
        //Lines are not parallel
         double line1det=det(end1(),end2());
         double line2det=det(other_line.end1(),other_line.end2());
         double above_x=det(vec2d(line1det,end1().x()-end2().x()),vec2d(line2det,other_line.end1().x()-other_line.end2().x()));
         double above_y=det(vec2d(line1det,end1().y()-end2().y()),vec2d(line2det,other_line.end1().y()-other_line.end2().y()));

         pos=pos2d(above_x/below,above_y/below);
         return true;
      }
      return false;
   }

   bool line2d::intersect(const line2d& other_line, pos2d& pos,double& this_param,double& other_param) const
   {
      if(!intersect(other_line,pos)) return false;
      this_param  = project(pos);
      other_param = other_line.project(pos);
      return true;
   }

   double line2d::project(const pos2d& point) const
   {
      vec2d line_vec(m_end1,m_end2);
      vec2d point_vec(m_end1,point);

      double qlen=line_vec.dot(line_vec);
      if(qlen<=0) return 0.0;

      double prd=line_vec.dot(point_vec);
      return prd/qlen;
   }

   line2d line2d::rotated_line(double par, double ccw_angle, double length) const
   {
      // direction of current line
      vec2d dir(m_end1,m_end2);
      dir.normalise();

      double dx = dir.x();
      double dy = dir.y();

      // direction of rotated line
      double ca = cos(ccw_angle);
      double sa = sin(ccw_angle);
      double x =  ca*dx - sa*dy;
      double y =  sa*dx + ca*dy;
      vec2d dir2(x,y);

      // compute start point
      pos2d p1 = interpolate(par);

      // compute end point
      pos2d p2 = p1 + dir2*length;

      return line2d(p1,p2);
   }


}
