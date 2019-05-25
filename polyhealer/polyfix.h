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
#ifndef POLYFIX_H
#define POLYFIX_H

#include "polyhealer_config.h"

#include "spacemath/polyhedron3d.h"
#include <memory>
#include <utility>  // std::pair
#include <string>
#include <list>

using namespace spacemath;

// this class modifies the input polyhedron in stages

class POLYHEALER_PUBLIC polyfix {
public:

   // the constructor takes a copy of the input polyhedron
   polyfix(const std::shared_ptr<polyhedron3d> poly, double dtol, double atol, bool verbose);
   virtual ~polyfix();

   // find unused vertices and remove them,
   // return number of vertices removed
   size_t remove_unused_vertices();

   // merge matching vertices and remove collapsed faces
   // return number of vertices and faces removed
   std::pair<size_t,size_t> merge_vertices();

   // find duplicate faces and remove them,
   // return number of faces removed
   size_t remove_duplicate_faces();

   // find nonmanifold faces and remove them,
   // return number of faces removed
   std::pair<size_t,size_t> remove_nonmanifold_or_zero_faces();

   // return repaired polyhedron copy
   std::shared_ptr<polyhedron3d> poly() { return m_poly; }

   // check current polyhedron and return warnings, if any
   std::list<std::string> check();

   // debugging, compute face area
   double area(size_t iface) const;

private:
   std::shared_ptr<polyhedron3d> m_poly;

   double m_dtol;     // distance tolerance
   double m_atol;     // area tolerance
   bool   m_verbose;  // if true, produce verbose messages
};

#endif // POLYFIX_H
