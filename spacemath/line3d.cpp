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

#include "line3d.h"
#include "vec3d.h"
#include <cstddef>
#include <cmath>
#include <limits>

namespace spacemath {

   line3d::line3d()
   : m_end1(pos3d(0.0,0.0,0.0)),m_end2(pos3d(0.0,0.0,0.0))
   {}

   line3d::line3d(const pos3d& end1, const pos3d& end2)
   : m_end1(end1),m_end2(end2)
   {}

   line3d::line3d(const line3d& line)
   : m_end1(line.m_end1),m_end2(line.m_end2)
   {}

   line3d::~line3d()
   {}

   line3d& line3d::operator =(const line3d& line)
   {
      m_end1 = line.m_end1;
      m_end2 = line.m_end2;
      return *this;
   }

   const pos3d& line3d::end1() const
   {
      return m_end1;
   }

   const pos3d& line3d::end2() const
   {
      return m_end2;
   }

   double line3d::length() const
   {
      return m_end1.dist(m_end2);
   }

   double line3d::project(const pos3d& point) const
   {
   // method:
   //     let (m_end1,point) = par*(m_end1,m_end2). this is put into the equation
   //
   //        (m_end1,point)*(m_end1,m_end2) = (m_end1,point)*(m_end1,m_end2).
   //
   //     thus the parameter par along m_end1,m_end2 is: par = (m_end1,p)*(m_end1,m_end2)/(m_end1,m_end2)**2.
   //
   //     compute the distance between the line and the point point.
   //
   // restrictions:
   //     the length of (m_end2-m_end1)  must be greater than zero.
      vec3d line_vec(m_end1,m_end2);
      vec3d point_vec(m_end1,point);

      double qlen=line_vec.dot(line_vec);
      if(qlen<=0) return 0.0;

      double prd=line_vec.dot(point_vec);
      return prd/qlen;
   }

   pos3d line3d::interpolate(double par) const
   {
      // parameter in interval [0.0, 1.0], i.e. relative distance from end1
      double x = m_end1.x() + par * ( m_end2.x() - m_end1.x() );
      double y = m_end1.y() + par * ( m_end2.y() - m_end1.y() );
      double z = m_end1.z() + par * ( m_end2.z() - m_end1.z() );
      return pos3d(x,y,z);
   }


   template<int kd>
   static int line_line_intersect(double p1[kd], double p2[kd], double q1[kd], double q2[kd], double& s,double&  t)
   {
      double ppl = 0.0;
      double qql = 0.0;
      for(size_t i=0; i<kd; i++) {
         double pp = p2[i]-p1[i];
         double qq = q2[i]-q1[i];
         ppl += pp*pp;
         qql += qq*qq;
      }
      ppl = sqrt(ppl);
      qql = sqrt(qql);

      double ppqd=1.0;
      double qqqd=1.0;
      double ppqq=0.0;
      double pppq=0.0;
      double pqqq=0.0;

      for(size_t i=0; i<kd; i++) {
         double pp = (p2[i]-p1[i])/ppl;
         double qq = (q2[i]-q1[i])/qql;
         double pq = q1[i]-p1[i];
         ppqq += pp*qq;
         pppq += pp*pq;
         pqqq += pq*qq;
      }

      double den = ppqd*qqqd - ppqq*ppqq;
      s  = (pppq*qqqd - ppqq*pqqq)/(den*ppl);
      t  = (ppqq*pppq - pqqq*ppqd)/(den*qql);

      if(!std::isfinite(s)) return -1;
      if(!std::isfinite(t)) return -1;

      return 0;
   }

   bool line3d::intersect(const line3d& other_line, double& this_param, double& other_param) const
   {
      vec3d this_vec(m_end1,m_end2);
      vec3d other_vec(other_line.m_end1,other_line.m_end2);
      if(this_vec.length() == 0.0)return false;
      if(other_vec.length() == 0.0)return false;
      if(this_vec.angle(other_vec)== 0.0)return false;

      double p1[3] = {m_end1.x(),m_end1.y(),m_end1.z()};
      double p2[3] = {m_end2.x(),m_end2.y(),m_end2.z()};

      double q1[3] = {other_line.m_end1.x(),other_line.m_end1.y(),other_line.m_end1.z()};
      double q2[3] = {other_line.m_end2.x(),other_line.m_end2.y(),other_line.m_end2.z()};

      double s = -std::numeric_limits<double>::max();
      double t = -std::numeric_limits<double>::max();
      if(line_line_intersect<3>(p1,p2,q1,q2,s,t) == 0) {
         this_param  = s;
         other_param = t;
         return true;
      }
      return false;
   }

}
