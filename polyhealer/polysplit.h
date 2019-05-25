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
#ifndef POLYSPLIT_H
#define POLYSPLIT_H
#include "polyhealer_config.h"

#include "spacemath/polyhedron3d.h"
#include <memory>
#include <algorithm>
#include <utility>  // std::pair
#include <string>
#include <map>
#include <set>

using namespace spacemath;

// this class modifies the input polyhedron by
// splitting edges where required to eliminate non-manifold faces

class POLYHEALER_PUBLIC polysplit {
public:
   typedef size_t id_vertex; // original vertex id from polyhedron
   typedef size_t id_edge;   // computed from 2 vertex ids
   typedef size_t id_face;   // computed by incrementing

   typedef std::map<id_edge,std::pair<id_vertex,id_vertex>>   edge_map;
   typedef std::map<id_edge,std::set<id_face>>                edge_faces;
   typedef std::map<id_edge,std::map<double,id_vertex>>       edge_splits;
   typedef std::map<id_face,pface>                            pface_map;

   polysplit(const std::shared_ptr<polyhedron3d> poly, double dtol, double atol);
   virtual ~polysplit();

   // perform the actual face splitting, this modifies polyhedron
   size_t split_faces();

   // return repaired polyhedron
   std::shared_ptr<polyhedron3d> poly() { return m_poly; }

protected:

   void build_faces();         // computes m_faces & m_face_counter
   void build_free_edges();    // computes m_free_edges
   void build_edge_splits();   // computes m_edge_splits
   void build_edge_faces();    // computes m_edge_faces

   void split_face(id_face face, id_edge edge, std::map<double,id_vertex>& verts);

private:
   std::shared_ptr<polyhedron3d>  m_poly;  // original polyhedron

private:
   edge_map    m_free_edges;    // contains edges with use-count=1
   edge_faces  m_edge_faces;    // m_edge_faces[edge]  = faces referencing id_edge
   edge_splits m_edge_splits;   // m_edge_splits[edge] = vertices causing edge split
   pface_map   m_faces;         // all polyhedron faces, some will be erased, some new created
   id_face     m_face_counter;  // ever increasing face counter during processing, used when inserting in m_faces

   double m_dtol;  // distance tolerance
   double m_atol;  // area tolerance
};

#endif // POLYSPLIT_H
