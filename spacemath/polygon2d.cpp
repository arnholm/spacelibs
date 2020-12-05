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

#include "polygon2d.h"
#include "line2d.h"
#include "circle2d.h"
#include <set>
#include <algorithm>
#include <iterator>
#include <cmath>

/*
** Author: Carsten Arnholm, 2008
*/

namespace spacemath {

   polygon2d::polygon2d()
   {}

   polygon2d::polygon2d(const list<pos2d>& vert)
   {
      m_vert.reserve(vert.size());
      copy(vert.begin(),vert.end(),std::back_inserter(m_vert));
   }

   polygon2d::polygon2d(const vector<pos2d>& vert)
   : m_vert(vert)
   {}

   polygon2d::polygon2d(const polygon2d& other)
   : m_vert(other.m_vert)
   {}

   polygon2d::~polygon2d()
   {}

   polygon2d& polygon2d::operator =(const polygon2d& other)
   {
      m_vert = other.m_vert;
      return *this;
   }

   void polygon2d::push_back(const pos2d& pos)
   {
      m_vert.push_back(pos);
   }

   bool polygon2d::is_valid() const
   {
      return (m_vert.size()>2);
   }

   const pos2d& polygon2d::position(size_t ipos) const
   {
      return m_vert[ipos];
   }

   size_t polygon2d::size() const
   {
      return m_vert.size();
   }

   polygon2d::const_iterator polygon2d::begin() const
   {
      return m_vert.begin();
   }

   polygon2d::const_iterator polygon2d::end() const
   {
      return m_vert.end();
   }

   double polygon2d::signed_area() const
   {

      /*
        Method:

        ref http://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
        Some of the suggested methods will fail in the case of a non-convex polygon,
        such as a crescent. Here's a simple one that will work with non-convex polygons
        (it'll even work with a self-intersecting polygon like a figure-eight, telling you whether it's mostly clockwise).

        Sum over the edges, (x2-x1)(y2+y1). If the result is positive the curve is clockwise,
        if it's negative the curve is counter-clockwise. (The result is twice the enclosed area, with a +/- convention.)

        point[0] = (5,0)   edge[0]: (6-5)(4+0) =   4
        point[1] = (6,4)   edge[1]: (4-6)(5+4) = -18
        point[2] = (4,5)   edge[2]: (1-4)(5+5) = -30
        point[3] = (1,5)   edge[3]: (1-1)(0+5) =   0
        point[4] = (1,0)   edge[4]: (5-1)(0+0) =   0
                                                 ---
                                                 -44  counter-clockwise
      */

      size_t np  = m_vert.size();
      double sum = 0.0;

      for (size_t ip = 1; ip < np; ip++)    {
         const pos2d& prev = m_vert[ip-1];
         const pos2d& pcur = m_vert[ip];

         double x1 = prev.x();
         double y1 = prev.y();

         double x2 = pcur.x();
         double y2 = pcur.y();

         sum += (x2 - x1)*(y2 + y1);
      }

      // close the polygon: account for the final edge from last to first point
      const pos2d& prev = m_vert[np-1];
      const pos2d& pcur = m_vert[0];

      double x1 = prev.x();
      double y1 = prev.y();

      double x2 = pcur.x();
      double y2 = pcur.y();

      sum += (x2 - x1)*(y2 + y1);

      double s_area = 0.5*sum;
      return s_area;
   }

   double polygon2d::area() const
   {
      return fabs(signed_area());
   }

   void polygon2d::reverse()
   {
      std::reverse(m_vert.begin(),m_vert.end());
   }


   /*
   static std::ostream& operator<<(std::ostream& out, const line2d& l)
   {
      out << " (" << setprecision(5) << setw(7) << fixed << l.end1().x() << ", " << setprecision(5) << setw(7) << fixed << l.end1().y() << ") ";
      out << " (" << setprecision(5) << setw(7) << fixed << l.end2().x() << ", " << setprecision(5) << setw(7) << fixed << l.end2().y() << ") ";
      return out;
   }
   */

   bool polygon2d::is_self_interesecting(double epspnt, double epspar) const
   {
      bool retval = false;

      double epspnt2 = epspnt*epspnt;
      size_t np  = m_vert.size();

      // traverse the edges and check other edges
      // notice that the inner loop jp starts at ip+1, so we don't perform all tests twice

      for (size_t ip=0; ip<np; ip++) {

         const pos2d& pcur =            m_vert[ip];
         const pos2d& pnxt = (ip<np-1)? m_vert[ip+1] : m_vert[0];
         line2d line0(pcur,pnxt);

         for (size_t jp=ip+1; jp<np; jp++) {

            const pos2d& pcur =            m_vert[jp];
            const pos2d& pnxt = (jp<np-1)? m_vert[jp+1] : m_vert[0];
            line2d line1(pcur,pnxt);

            // we have both edge lines, first check for normal intersection of non-parallel lines

            pos2d p;
            double line0_param=-1.0;
            double line1_param=-1.0;
            if(line0.intersect(line1,p,line0_param,line1_param)){

               // non-parallell, the two line parameters must be within rage to have a proper intersection
               bool on_line0 = (line0_param>epspar) && (line0_param<(1.0-epspar));
               bool on_line1 = (line1_param>epspar) && (line1_param<(1.0-epspar));
               if(on_line0 && on_line1) {

                  // ok, looks like a real intersection, but if the intersection point is
                  // very close to the edge ends, it isn't a real intersection anyway
                  if( (p.dist_squared(line0.end1())<= epspnt2) || (p.dist_squared(line0.end2())<= epspnt2) )on_line0 = false;
                  if( (p.dist_squared(line1.end1())<= epspnt2) || (p.dist_squared(line1.end2())<= epspnt2) )on_line1 = false;

                  // if we pass all these tests, we have a real intersection
                  if(on_line0 && on_line1) retval = true;
               }
               else {
                  // if we get here, the lines have been classified as non-parallel in the intersect call.
                  // However, it could still be that they are *nearly* parallel and overlap

                  retval = near_parallel_overlap(line0,line1,epspnt,epspar);
                  if(!retval) retval = near_parallel_overlap(line1,line0,epspnt,epspar);
               }
            }
            else {

               // lines are parallel, check if they are partly or fully overlapping
               if(point_online(line0.end1(),line1,epspnt,epspar)
               || point_online(line0.end2(),line1,epspnt,epspar)
               || point_online(line1.end1(),line0,epspnt,epspar)
               || point_online(line1.end2(),line0,epspnt,epspar)) {
                  retval = true;
               }
            }

            if(retval) {
               return retval;
            }
         }
      }
      return false;
   }

   bool polygon2d::near_parallel_overlap(const line2d& line0, const line2d& line1, double epspnt, double epspar)
   {
      double epspnt2 = epspnt*epspnt;

      // near_parallel is true if both projection distances are within tolerance

      pos2d end1      = line1.end1();
      pos2d end2      = line1.end2();
      double end1_par = line0.project(end1);
      double end2_par = line0.project(end2);
      pos2d end1_proj = line0.interpolate(end1_par);
      pos2d end2_proj = line0.interpolate(end2_par);
      double dist1_sq = end1.dist_squared(end1_proj);
      double dist2_sq = end2.dist_squared(end2_proj);

      if( (dist1_sq <= epspnt2) && (dist2_sq <= epspnt2) ) {
         // lines are near parallel, we have to check parameter overlap
         // If the overlap is near total, we might miss, so we check the mid parameter as well
         double mid_par = 0.5*(end1_par+end2_par);
         bool overlap   =   (end1_par>=epspar) && (end1_par<=(1.0-epspar))
                        ||  (end2_par>=epspar) && (end2_par<=(1.0-epspar))
                        ||  (mid_par>=epspar)  && (mid_par<=(1.0-epspar));
         if(overlap) {

            // we have near-parallel lines with overlap
            return true;
         }
      }
      return false;
   }

   bool polygon2d::point_online(const pos2d& p, const line2d& l, double epspnt, double epspar)
   {
      double par     = l.project(p);

      bool on_line = (par>=epspar) && (par<=(1.0-epspar));
      if(on_line) {
         // projection was within parameter range, check projection distance
         double epspnt2 = epspnt*epspnt;
         double dist2   = p.dist_squared(l.interpolate(par));
         if(dist2 <= epspnt2)return true;
      }
      return false;
   }

   line2d polygon2d::project(const pos2d& p) const
   {
      multimap<double,line2d> pmap;
      project(p,pmap);

      // also check for shortest point-to-point distance
      for(auto& p1 : m_vert) {
         line2d l(p,p1);
         pmap.insert(std::make_pair(l.length(),l));
      }

      // return the shortest line found
      return pmap.begin()->second;
   }

   line2d polygon2d::project(const polygon2d& b) const
   {
      multimap<double,line2d> pmap;

      // perform 2-way projections and collect in pmap
      project(b,pmap,true);
      b.project(*this,pmap,false);

      // also check for shortest point-to-point distance
      for(auto& p1 : m_vert) {
         for(auto& p2 : b) {
            line2d l(p1,p2);
            pmap.insert(std::make_pair(l.length(),l));
         }
      }

      // return the shortest line found
      return pmap.begin()->second;
   }

   vector<line2d> polygon2d::get_edges() const
   {
      // establish a closed loop of edges
      vector<line2d> edges;
      edges.reserve(m_vert.size());
      for(size_t ivert=1; ivert<m_vert.size(); ivert++) {
         edges.push_back(line2d(m_vert[ivert-1],m_vert[ivert]));
      }

      // final edge from last to first vertex
      edges.push_back(line2d(m_vert[m_vert.size()-1],m_vert[0]));
      return std::move(edges);
   }

   void polygon2d::project(const polygon2d& b, multimap<double,line2d>& pmap, bool this_first) const
   {
      // project my vertices onto b edges
      vector<line2d> edges = b.get_edges();
      for(auto& p : m_vert) {
         for(auto& e : edges) {
            double par = e.project(p);
            if( (par>=0.0) && (par<=1.0) ) {
               line2d l =  (this_first)? line2d(p,e.interpolate(par)) : line2d(e.interpolate(par),p) ;
               pmap.insert(std::make_pair(l.length(),l));
            }
         }
      }
   }

   void polygon2d::project(const pos2d& p, multimap<double,line2d>& pmap) const
   {
      vector<line2d> edges = get_edges();
      for(auto& e : edges) {
         double par = e.project(p);
         if( (par>=0.0) && (par<=1.0) ) {
            line2d l(p,e.interpolate(par));
            pmap.insert(std::make_pair(l.length(),l));
         }
      }
   }

   list<pos2d> polygon2d::intersect(const polygon2d& b, size_t nmax) const
   {
      list<pos2d> xsect;
      vector<line2d> a_edges = get_edges();
      vector<line2d> b_edges = b.get_edges();

      // check all possible intersection combinations
      for(auto& a_edge : a_edges) {
        for(auto& b_edge : b_edges) {

           // do not compute more intersections than requested
           if(xsect.size() >= nmax)return xsect;

           pos2d xpos;
           double a_par = std::numeric_limits<double>::max();
           double b_par = std::numeric_limits<double>::max();
           if(a_edge.intersect(b_edge,xpos,a_par,b_par)) {
              bool on_a = (a_par >= 0.0) && (a_par <= 1.0);
              bool on_b = (b_par >= 0.0) && (b_par <= 1.0);
              if(on_a && on_b) {
                 xsect.push_back(xpos);
              }
           }
        }
      }

      return xsect;
   }

   list<pos2d> polygon2d::intersect(const circle2d& b, size_t nmax) const
   {
      list<pos2d> xsect;
      vector<line2d> a_edges = get_edges();

      // check all possible intersection combinations
      for(auto& a_edge : a_edges) {
         // do not compute more intersections than requested
         if(xsect.size() >= nmax)return xsect;

         bool infinite_line = false;
         vector<pos2d> xp = b.intersect_ex(a_edge,infinite_line);
         for(auto& p : xp) {
            xsect.push_back(p);
         }
      }

      return xsect;
   }



}
