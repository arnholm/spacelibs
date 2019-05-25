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
#ifndef POLYREMESH_H
#define POLYREMESH_H

#include "mutable_polyhedron3d.h"

// polyremesh performs surface remeshing of input polyhedron
// The main purpose is to prepare for 3d FEM meshing

class POLYHEALER_PUBLIC polyremesh {
public:
   polyremesh(const std::shared_ptr<polyhedron3d> poly, double dtol, double edge_len);
   virtual ~polyremesh();

   // remesh by flip-split algorithm
   void flip_split();

   // adjust mesh flipping to improve aspect ratios
   void aspect_ratio_flip();

protected:
   typedef std::multimap<double,id_edge> edge_length_map;

   edge_length_map compute_edge_lengths();
   edge_length_map compute_face_aspect_ratio_edges();
   size_t flip_split(id_edge iedge);
   size_t flip_edge(id_edge iedge);

   id_face add_face(const vec3d& normal, id_vertex iv1, id_vertex iv2, id_vertex iv3);
   inline void  remove_face(id_face iface, id_edge iedge) { m_poly.remove_face(iface,iedge); }

private:
   mutable_polyhedron3d  m_poly;
   double                m_dtol;             // distance tolerance
   double                m_edge_len;         // target edge lengths
   double                m_min_aspect_ratio; // minimum aspect ratio
};

#endif // POLYREMESH_H
