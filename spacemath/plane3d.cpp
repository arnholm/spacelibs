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

#include "plane3d.h"
#include <cmath>
#include <limits>
#include <set>

namespace spacemath {

   static inline double accuracy() {return 0.0;}
   static inline bool isEqual(double a,double b) {return (a-b)<accuracy() && (b-a)<accuracy();}
   static inline bool isZero(double a) {return isEqual(a,0.0);}

   plane3d::plane3d()
   : m_is_plane(false)
   {}

   plane3d::plane3d(const pos3d& p1, const pos3d& p2, const pos3d& p3)
   : m_is_plane(false)
   {
      m_ABCD[0] = 0.0;
      m_ABCD[1] = 0.0;
      m_ABCD[2] = 0.0;
      m_ABCD[3] = 0.0;
      vec3d v1(p1,p2);
      vec3d v2(p1,p3);
      v1.normalise();
      v2.normalise();
      vec3d n=v1*v2;
      if(isZero(n.length())) return;
      else {
         n.normalise();
         m_ABCD[0] = n.x();
         m_ABCD[1] = n.y();
         m_ABCD[2] = n.z();
         m_ABCD[3] = m_ABCD[0]*p1.x()+m_ABCD[1]*p1.y()+m_ABCD[2]*p1.z();
         m_is_plane = true;
      }
   }


   plane3d::plane3d(const pos3d& point, const vec3d& normal)
   : m_is_plane(false)
   {
      m_ABCD[0] = 0.0;
      m_ABCD[1] = 0.0;
      m_ABCD[2] = 0.0;
      m_ABCD[3] = 0.0;
      if(isZero(normal.length())) m_is_plane=false;
      else {
         vec3d n=normalise(normal);
         m_ABCD[0] = n.x();
         m_ABCD[1] = n.y();
         m_ABCD[2] = n.z();
         m_ABCD[3] = m_ABCD[0]*point.x()+m_ABCD[1]*point.y()+m_ABCD[2]*point.z();
         m_is_plane = true;
      }
   }

   plane3d::plane3d(const double abcd[4], bool is_plane)
   {
      for (int i=0; i<4; ++i)
         m_ABCD[i] = abcd[i];
      m_is_plane = is_plane;
   }

   double plane3d::dist(const pos3d& point) const
   {
      double delta=m_ABCD[0]*point.x()+m_ABCD[1]*point.y()+m_ABCD[2]*point.z();
      return fabs(m_ABCD[3]-delta);
   }

   int plane3d::side(const pos3d& point) const
   {
      double delta=m_ABCD[0]*point.x()+m_ABCD[1]*point.y()+m_ABCD[2]*point.z();
      double result=delta-m_ABCD[3];
      if(result==0) return 0;
      else if(result<0) return -1;
      else return 1;
   }

   pos3d plane3d::project(const pos3d& point) const
   {
      double delta=m_ABCD[0]*point.x()+m_ABCD[1]*point.y()+m_ABCD[2]*point.z();
      vec3d ABC(m_ABCD[0],m_ABCD[1],m_ABCD[2]);
      double par=m_ABCD[3]-delta;
      return point+par*ABC;
   }

   bool plane3d::intersect(const line3d& line, double& line_param) const
   {
      if(!m_is_plane) return false;
      double denominator = m_ABCD[0]*(line.end1().x() - line.end2().x()) +
                           m_ABCD[1]*(line.end1().y() - line.end2().y()) +
                           m_ABCD[2]*(line.end1().z() - line.end2().z());

      //Lines are parallel, no intersection exists.
      if(isZero(denominator)) return false;

      line_param = m_ABCD[0]*line.end1().x() +
                   m_ABCD[1]*line.end1().y() +
                   m_ABCD[2]*line.end1().z() -
                   m_ABCD[3];
      line_param/= denominator;

      if(!std::isfinite(line_param)) return false;

      return true;
   }

   plane3d::~plane3d()
   {
      m_is_plane = false;
   }

   vec3d plane3d::normal() const
   {
      return vec3d(m_ABCD[0],m_ABCD[1],m_ABCD[2]);
   }

   pos3d plane3d::mirror(const pos3d& in) const
   {
      if (!m_is_plane)
         return in;

      pos3d projection = project(in);
      return in + (projection-in)*2;
   }


   bool plane3d::intersect(const plane3d& plane, line3d& line) const
   {
      vec3d line_normal =normal().cross(plane.normal());
      if(isZero(line_normal.length())) return false;

      double x[3];

      std::set<int> indices;
      indices.insert(0);indices.insert(1);indices.insert(2);
      int index1=0;
      int index2=0;
      int index3=0;
      for(;index1<3;++index1) {
         if(!isZero(equation()[index1])) {
            for(;index2<3;++index2) {
               if(index1==index2) continue;
               if(!isZero(equation()[index1]*plane.equation()[index2]-plane.equation()[index1]*equation()[index2])) break;
            }
            break;
         }
      }

      if(index1==3 || index2==3) return false;

      indices.erase(index1);indices.erase(index2);
      index3=*indices.begin();

      x[index3]=0.0;
      x[index2]=(equation()[index1]*plane.equation()[3]-equation()[3]*plane.equation()[index1])/
                (equation()[index1]*plane.equation()[index2]-equation()[index2]*plane.equation()[index1]);
      x[index1]=(equation()[3]-equation()[index2]*x[index2])/equation()[index1];
      line_normal.normalise();
      pos3d pos(x[0],x[1],x[2]);
      line=line3d(pos,pos+line_normal);
      return true;
   }

}
