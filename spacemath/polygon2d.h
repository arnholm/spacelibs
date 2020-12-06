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

#ifndef POLYGON2D_H
#define POLYGON2D_H

#include "spacemath_config.h"
#include <list>
#include <vector>
#include <map>
using std::vector;
using std::list;
using std::multimap;
#include "pos2d.h"
#include <cstddef>
/*
** Author: Carsten Arnholm, 2008
*/

namespace spacemath {
   class line2d;
   class circle2d;

   // polygon in 2d space
   class SPACEMATH_PUBLIC polygon2d {
   public:
      polygon2d();

      // construct from list or vector of points, should normally be in CCW order
      polygon2d(const list<pos2d>& vert);
      polygon2d(const vector<pos2d>& vert);
      polygon2d(const polygon2d& other);
      virtual ~polygon2d();

      typedef vector<pos2d>::const_iterator const_iterator;
      typedef vector<pos2d>::iterator iterator;

      polygon2d& operator =(const polygon2d& other);

      /// a polygon2d is valid if it has at least 3 vertices
      bool is_valid() const;

      /// add a position to the back of the polyline. Don't overuse this
      void push_back(const pos2d& pos);

      /// return the number of points in polyline
      size_t size() const;

      /// return position ipos. Range [0,size()-1]
      const pos2d& position(size_t ipos) const;

      // compute the signed area of the polygon
      // The signed area is positive if the edges are in CCW order and negative if not
      double signed_area() const;

      // return the absolute area, regardless edge order
      double area() const;

      // reverse the orientation of the polygon by reversing the vertex order
      void reverse();

      // check for polygon self-intersection by naive edge-edge comparison, this is O2
      // returns true if at least one self intersection is detected
      // an intersection happens when the intersection point is within <0,1> and distance to endpoints are > dtol
      bool is_self_interesecting(double epspnt, double epspar) const;


      // iteration over the boundary points
      const_iterator begin() const;
      const_iterator end() const;

      iterator begin() { return m_vert.begin(); }
      iterator end()   { return m_vert.end(); }

      // compute shortest connection line between point and polygon
      // If no projections found, shortest point-to-point line is returned instead
      line2d project(const pos2d& p) const;

      // compute shortest connection line between polygons, using 2 way vertex to edge projections.
      // If no projections found, shortest point-to-point line is returned instead
      // Note: there is no check for edge/edge intersection here
      line2d project(const polygon2d& b) const;

      // compute edge/edge intersections, returning zero or more polygon intersection points (limited by nmax)
      list<pos2d> intersect(const polygon2d& b, size_t nmax) const;

      // compute edge/circle intersections, returning zero or more intersection points (limited by nmax)
      list<pos2d> intersect(const circle2d& b, size_t nmax) const;

   protected:
      // project vertices onto edges of b
      void project(const polygon2d& b, multimap<double,line2d>& pmap, bool this_first) const;
      void project(const pos2d& b, multimap<double,line2d>& pmap) const;

      vector<line2d> get_edges() const;

   private:
      static bool point_online(const pos2d& p, const line2d& l, double epspnt, double epspar);
      static bool near_parallel_overlap(const line2d& line0, const line2d& line1, double epspnt, double epspar);

   private:
      vector<pos2d> m_vert;
   };
}

#endif // POLYGON2D_H
