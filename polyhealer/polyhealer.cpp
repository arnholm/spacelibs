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
#include "polyhealer.h"
#include "polyfix.h"
#include "polysplit.h"
#include "lump_finder.h"

#include "polyflip.h"

#include <sstream>
using namespace std;

polyhealer::polyhealer(const std::shared_ptr<polyhedron3d> poly,  double dtol, double atol, bool verbose)
: m_poly(poly)
, m_dtol(dtol)
, m_atol(atol)
, m_verbose(verbose)
, m_nchanges(0)
{}

polyhealer::~polyhealer()
{}

std::string polyhealer::size_status() const
{
   ostringstream out;
   out << "vertices=" << m_poly->vertex_size() << " faces="<<m_poly->face_size();
   return out.str();
}

std::list<std::string> polyhealer::warnings() const
{
   polyfix pre_fix(m_poly,m_dtol,m_atol,m_verbose);
   std::list<std::string> warnings = pre_fix.check();
   if(warnings.size() == 0)warnings.push_back("no warnings");
   return warnings;
}

void  polyhealer::remove_unused_vertices()
{
   polyfix  fix(m_poly,m_dtol,m_atol,m_verbose);
   size_t num_unused = fix.remove_unused_vertices();
   if(num_unused > 0) {
      ostringstream out;
      out << "removed " << num_unused << " unused " << ((num_unused==1)? "vertex":"vertices");
      m_messages.push_back(out.str());

      m_nchanges += num_unused;
   }
}

void polyhealer::merge_vertices()
{
   // merge vertices using clustering-technique

   polyfix  fix(m_poly,m_dtol,m_atol,m_verbose);
   std::pair<size_t,size_t> p = fix.merge_vertices();
   size_t num_removed_vertices = p.first;
   size_t num_removed_faces    = p.second;
   if( (num_removed_vertices+num_removed_faces) > 0) {
      if(num_removed_vertices>0) {
         ostringstream out;
         out << "merged " << num_removed_vertices << ((num_removed_vertices==1)? " vertex":" vertices");
         m_messages.push_back(out.str());
      }

      if(num_removed_faces>0) {
         ostringstream out;
         out << "removed " << num_removed_faces <<  " collapsed or zero area " << ((num_removed_faces==1)? "face":"faces");
         m_messages.push_back(out.str());
      }

      m_nchanges += num_removed_vertices+num_removed_faces;
   }
}

void  polyhealer::split_faces()
{
   // split any non-manifold edges & faces
   polysplit splitter(m_poly,m_dtol,m_atol);
   size_t nsplit = splitter.split_faces();
   if(nsplit > 0) {
      ostringstream out;
      out << "split "<< nsplit << ' ' << ((nsplit==1)? "face":"faces");
      m_messages.push_back(out.str());

      m_nchanges += nsplit;
   }
}

void polyhealer::remove_duplicate_faces()
{
   // remove duplicate faces, this can happen after merging vertices
   polyfix  fix(m_poly,m_dtol,m_atol,m_verbose);
   size_t num_dup_faces = fix.remove_duplicate_faces();
   if(num_dup_faces > 0) {
      ostringstream out;
      out << "removed " << num_dup_faces << " duplicate " << ((num_dup_faces==1)? "face":"faces");
      m_messages.push_back(out.str());

      m_nchanges += num_dup_faces;
   }
}

void polyhealer::remove_nonmanifold_or_zero_faces()
{
   // remove nonmanifiold faces, this can happen after removing duplicates
   polyfix  fix(m_poly,m_dtol,m_atol,m_verbose);
   std::pair<size_t,size_t> removed = fix.remove_nonmanifold_or_zero_faces();
   size_t num_nonmanifold = removed.first;
   size_t num_zero_area   = removed.second;
   if(num_nonmanifold > 0) {

      if(num_nonmanifold > 0) {
         ostringstream out;
         out << "removed " << num_nonmanifold << " nonmanifold " << ((num_nonmanifold==1)? "face":"faces");
         m_messages.push_back(out.str());
      }

      if(num_zero_area>0) {
         ostringstream out;
         out << "removed " << num_zero_area <<   " zero area "  << ((num_zero_area==1)? "face":"faces");
         m_messages.push_back(out.str());
      }
      m_nchanges += (num_nonmanifold+num_zero_area);
   }
}

size_t polyhealer::run_healing_step()
{
   m_nchanges=0;

   // add an initial status
   polyfix fix(m_poly,m_dtol,m_atol,m_verbose);
   m_messages = fix.check();

   // perform healing
   remove_unused_vertices();
   merge_vertices();
   split_faces();
   remove_duplicate_faces();
   remove_nonmanifold_or_zero_faces();

   ostringstream out;
   out << "total changes=" << m_nchanges;
   m_messages.push_back(out.str());

   return m_nchanges;
}


std::string  polyhealer::run_healing(size_t maxiter, std::ostream& out)
{
   std::string warning_summary = "";

   const string blanks = "             ";

   bool repeat = false;
   size_t iteration = 0;
   do {
      out << std::endl << "iteration "<< iteration << ": " <<  size_status() << std::endl;

      // perform the healing, set repeat flag if there were changes
      repeat = run_healing_step() > 0;

      // display messages from the healing process
      for(auto msg : *this) {
         out << blanks << msg << std::endl;
      }

      // display any current warnings
      std::list<std::string> warnings = this->warnings();
      for(auto msg : warnings) {
         out << blanks << msg << std::endl;
         warning_summary = msg;
      }
   }
   while(repeat && (++iteration < maxiter));

   return warning_summary;
}

std::shared_ptr<ph3d_vector>  polyhealer::find_lumps(bool flip_faces)
{
   m_messages.clear();
   ostringstream out;
   lump_finder lfinder(m_poly);
   std::shared_ptr<ph3d_vector> lumps = lfinder.find_lumps();

   if(lumps->size() == 0) {
      // use the original polyhedron as the one and only lump
      lumps->push_back(m_poly);
   }

   out << "lumps detected=" << lumps->size();
   m_messages.push_back(out.str());

   if(flip_faces) {
      size_t ilump=0;
      for(auto lump : *lumps) {
         ostringstream out;
         polyflip flipper(lump,m_dtol*0.01);
         size_t nflip = flipper.flip_faces();
         out << "lump " << ilump << " flipped "<< nflip << ((nflip==1)? " face":" faces");
         m_messages.push_back(out.str());
         ilump++;
      }
   }

   return lumps;
}
