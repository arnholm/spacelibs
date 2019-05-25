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
#ifndef LUMP_FINDER_H
#define LUMP_FINDER_H

#include "spacemath/polyhedron3d.h"
#include <memory>
#include <utility>  // std::pair
#include <unordered_map>
#include <unordered_set>
#include <set>

#include "mutable_polyhedron3d.h"

// lump_finder takes a healed polyhedron and analyses whether it consists of separate, disconnected lumps

class POLYHEALER_PUBLIC lump_finder {
public:

   // various topological identifiers
   typedef size_t  id_lump;

   typedef std::set<id_vertex>                    vertex_set;     // a sorted set of vertices
   typedef std::unordered_map<id_lump, face_set>  lump_map;       // faces in a lump

   lump_finder(const std::shared_ptr<polyhedron3d>  poly);
   virtual ~lump_finder();

   // return the polyhedron split into lumps
   std::shared_ptr<ph3d_vector> find_lumps();

private:
   mutable_polyhedron3d  m_poly;
};

#endif // LUMP_FINDER_H
