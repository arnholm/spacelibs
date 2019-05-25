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
#ifndef POLYHEALER_H
#define POLYHEALER_H

#include "polyhealer_config.h"

#include "spacemath/polyhedron3d.h"
#include <list>
#include <string>
#include <memory>
#include <ostream>

using namespace spacemath;

class POLYHEALER_PUBLIC polyhealer {
public:
   typedef std::list<std::string> message_list;
   typedef message_list::iterator iterator;

   polyhealer(const std::shared_ptr<polyhedron3d> poly, double dist_tol, double area_tol, bool verbose);
   virtual ~polyhealer();

   // return a string with a status message on the polyhedron sizes
   std::string size_status() const;

   // return a list of warning messages for the polyhedron, if any
   std::list<std::string> warnings() const;

   // perform healing iterations until no more changes, messages to given ostream
   std::string  run_healing(size_t maxiter, std::ostream& out);

   // perform one healing iteration, return number of changes
   size_t run_healing_step();

   // traversal over messages from last iteration
   iterator begin() { return m_messages.begin(); }
   iterator end()   { return m_messages.end(); }

   // findlumps from the input polyhedron
   std::shared_ptr<ph3d_vector>  find_lumps(bool flip_faces);

protected:
   void remove_unused_vertices();
   void merge_vertices();
   void split_faces();
   void remove_duplicate_faces();
   void remove_nonmanifold_or_zero_faces();

private:
   std::shared_ptr<polyhedron3d>  m_poly;     // polyhedron being processed
   double                         m_dtol;     // coordinate distance tolerance
   double                         m_atol;     // area tolerance
   bool                           m_verbose;  // if true, produce verbose messages
   size_t                         m_nchanges; // number of changes in iteration
   std::list<std::string>         m_messages; // messages in this iteration
};

#endif // POLYHEALER_H
