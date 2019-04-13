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
   
#include "bbox3d.h"
#include <algorithm>
#include <utility>
#include <cmath>
using namespace std;

namespace spacemath {

   bbox3d::bbox3d(bool initialised)
   : m_p1(0,0,0),m_p2(0,0,0),m_initialised(initialised)
   {}

   bbox3d::bbox3d(const pos3d& pos1, const pos3d& pos2)
   :m_initialised(true)
   {
      setBox(pos1,pos2);
   }

   bbox3d::~bbox3d()
   {
   }

   bbox3d& bbox3d::operator =(const bbox3d& box)
   {
      if(this!=&box) {
         m_p1 = box.m_p1;
         m_p2 = box.m_p2;
         m_initialised = box.m_initialised;
      }
      return *this;
   }


   void bbox3d::setBox(const pos3d& pos1, const pos3d& pos2)
   {
      if (!m_initialised) {
         m_p1 = pos1;
         m_p2 = m_p1;
         m_initialised = true;
      }

      double lminx = min(pos1.x(),pos2.x());
      double lmaxx = max(pos1.x(),pos2.x());
      double lminy = min(pos1.y(),pos2.y());
      double lmaxy = max(pos1.y(),pos2.y());
      double lminz = min(pos1.z(),pos2.z());
      double lmaxz = max(pos1.z(),pos2.z());

      m_p1 = pos3d(lminx,lminy,lminz);
      m_p2 = pos3d(lmaxx,lmaxy,lmaxz);
   }

   void  bbox3d::enclose(const pos3d& pos1,double tolerance)
   {
      if (!m_initialised) {
         m_p1 = pos1;
         m_p2 = m_p1;
         m_initialised = true;
      }
      // recalculate boundaries
      double lminx = min(m_p1.x(),pos1.x()-tolerance);
      double lmaxx = max(m_p2.x(),pos1.x()+tolerance);
      double lminy = min(m_p1.y(),pos1.y()-tolerance);
      double lmaxy = max(m_p2.y(),pos1.y()+tolerance);
      double lminz = min(m_p1.z(),pos1.z()-tolerance);
      double lmaxz = max(m_p2.z(),pos1.z()+tolerance);

      // reset points
      m_p1 = pos3d(lminx,lminy,lminz);
      m_p2 = pos3d(lmaxx,lmaxy,lmaxz);
   }

   const pos3d& bbox3d::p1() const
   {
      return m_p1;
   }

   const pos3d& bbox3d::p2() const
   {
      return m_p2;
   }

   bool bbox3d::initialised() const
   {
      return m_initialised;
   }


   bool bbox3d::is_enclosed( const pos3d& p) const
   {
      if(!m_initialised) return false;

      double xmin = m_p1.x();
      double ymin = m_p1.y();
      double zmin = m_p1.z();

      double xmax = m_p2.x();
      double ymax = m_p2.y();
      double zmax = m_p2.z();

      double x = p.x();
      double y = p.y();
      double z = p.z();

      bool okx = (xmin <= x) && (x <= xmax);
      bool oky = (ymin <= y) && (y <= ymax);
      bool okz = (zmin <= z) && (z <= zmax);

      return (okx && oky && okz);
   }

   static bool intersect(std::pair<double,double> a, std::pair<double,double> b, std::pair<double,double>& seg_inter)
   {
      if(b.first<=a.first &&  b.second<=a.first) {
         //              |---- a ----|
         //   |-- b --|
         return false;
      }
      else if(b.first>=a.second &&  b.second>=a.second) {
         //   |---- a ----|
         //                    |-- b --|
         return false;
      }
      else if(b.first>=a.first &&  b.second<=a.second) {
         //        |---- a ----|
         //          |-- b --|
         seg_inter = b;
         return true;
      }
      else if(b.first<=a.first &&  b.second>=a.second) {
         //        |---- a ----|
         //      |------ b ---------|
         seg_inter = a;
         return true;
      }
      else if(b.first>=a.first &&  b.second>=a.second) {
         //        |---- a ----|
         //             |------ b -------|
         seg_inter = std::make_pair(b.first,a.second);
         return true;
      }
      else if(b.first<=a.first &&  b.second<=a.second) {
         //              |---- a ----|
         //     |------ b -------|
         seg_inter = std::make_pair(a.first,b.second);
         return true;
      }
      return false;
   }


   bbox3d bbox3d::intersection(const bbox3d& b) const
   {
      std::pair<double,double> xa = std::make_pair(m_p1.x(),m_p2.x());
      std::pair<double,double> xb = std::make_pair(b.m_p1.x(),b.m_p2.x());
      std::pair<double,double> xi;
      if(!intersect(xa,xb,xi))return bbox3d();

      std::pair<double,double> ya = std::make_pair(m_p1.y(),m_p2.y());
      std::pair<double,double> yb = std::make_pair(b.m_p1.y(),b.m_p2.y());
      std::pair<double,double> yi;
      if(!intersect(ya,yb,yi))return bbox3d();

      std::pair<double,double> za = std::make_pair(m_p1.z(),m_p2.z());
      std::pair<double,double> zb = std::make_pair(b.m_p1.z(),b.m_p2.z());
      std::pair<double,double> zi;
      if(!intersect(za,zb,zi))return bbox3d();

      return bbox3d(pos3d(xi.first,yi.first,zi.first),pos3d(xi.second,yi.second,zi.second));
   }


   bbox3d bbox3d::difference(const bbox3d& b , double tolerance) const
   {
      // return copy of this box if no intersection
      std::pair<double,double> xa = std::make_pair(m_p1.x(),m_p2.x());
      std::pair<double,double> xb = std::make_pair(b.m_p1.x(),b.m_p2.x());
      std::pair<double,double> xi;
      if(!intersect(xa,xb,xi))return *this;

      std::pair<double,double> ya = std::make_pair(m_p1.y(),m_p2.y());
      std::pair<double,double> yb = std::make_pair(b.m_p1.y(),b.m_p2.y());
      std::pair<double,double> yi;
      if(!intersect(ya,yb,yi))return *this;

      std::pair<double,double> za = std::make_pair(m_p1.z(),m_p2.z());
      std::pair<double,double> zb = std::make_pair(b.m_p1.z(),b.m_p2.z());
      std::pair<double,double> zi;
      if(!intersect(za,zb,zi))return *this;

      // there was an intersection

      // extent of intersection
      double dxi = xi.second - xi.first;
      double dyi = yi.second - yi.first;
      double dzi = zi.second - zi.first;

      // extent of this box
      double dxa = xa.second - xa.first;
      double dya = ya.second - ya.first;
      double dza = za.second - za.first;

      // check if we have full overlap in x,y,z
      bool x_overlap = (fabs(dxi-dxa) <= tolerance);
      bool y_overlap = (fabs(dyi-dya) <= tolerance);
      bool z_overlap = (fabs(dzi-dza) <= tolerance);

      if(x_overlap && y_overlap) {

         // check if we can limit the range in z
         bool split = (zi.first-za.first > tolerance) && (za.second-zi.second > tolerance);
         if(!split) {
            if(za.first < zi.first) {
               za = std::make_pair(za.first,zi.first);
            }
            else if(zi.second < za.second) {
               za = std::make_pair(zi.second,za.second);
            }
         }
      }

      if(x_overlap && z_overlap) {
         // check if we can limit the range in y
         bool split = (yi.first-ya.first > tolerance) && (ya.second-yi.second > tolerance);
         if(!split) {
            if(ya.first < yi.first) {
               ya = std::make_pair(ya.first,yi.first);
            }
            else if(yi.second < ya.second)  {
               ya = std::make_pair(yi.second,ya.second);
            }
         }
      }

      if(y_overlap && z_overlap) {
         // check if we can limit the range in x
         bool split = (xi.first-xa.first > tolerance) && (xa.second-xi.second > tolerance);
         if(!split) {
            if(xa.first < xi.first) {
               xa = std::make_pair(xa.first,xi.first);
            }
            else if(xi.second < xa.second) {
               xa = std::make_pair(xi.second,xa.second);
            }
         }
      }

      return bbox3d(pos3d(xa.first,ya.first,za.first),pos3d(xa.second,ya.second,za.second));
   }

   pos3d bbox3d::center() const
   {
      return 0.5*(m_p1+m_p2);
   }

   double bbox3d::dx() const
   {
      return (m_p2.x() - m_p1.x());
   }

   double bbox3d::dy() const
   {
      return (m_p2.y() - m_p1.y());
   }

   double bbox3d::dz() const
   {
      return (m_p2.z() - m_p1.z());
   }

   bbox3d operator*(const HTmatrix& T, const bbox3d& b)
   {
       pos3d p1 = b.p1();
       pos3d p2 = b.p2();

       double x[] = { p1.x(), p2.x() };
       double y[] = { p1.y(), p2.y() };
       double z[] = { p1.z(), p2.z() };

       bbox3d box;
       for(size_t iz=0; iz<2; iz++) {
          for(size_t iy=0; iy<2; iy++) {
             for(size_t ix=0; ix<2; ix++) {
                box.enclose(T*pos3d(x[ix],y[iy],z[iz]));
             }
          }
       }

       return box;
   }



}
