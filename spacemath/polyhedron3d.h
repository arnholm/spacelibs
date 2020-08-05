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

#ifndef POLYHEDRON3D_H
#define POLYHEDRON3D_H

#include "spacemath_config.h"

// polyhedron3d contains the basic definition of a polyhedron in 3d space
// The faces can be N-sided (where N>= 3). All faces must be flat.

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include "vec3d.h"
#include "pos3d.h"

// some typedefs declared outside the spacemath namespace for simplicity

typedef size_t    id_vertex; // vertex index, 0 or greater
typedef long long id_edge;   // edge identifier.   Always positive, but compatible with id_coedge
typedef long long id_coedge; // coedge identifier. Same absolute value as id_edge, but may be negative. abs(coedge)==edge
typedef size_t    id_face;   // face index, 0 or greater

typedef std::pair<id_vertex,id_vertex>       vertex_pair;     // ordered pair of vertices, typically for an edge
typedef std::vector<spacemath::pos3d>        vtx_vec;         // vertex vector
typedef std::vector<id_vertex>               pface;           // polyhedron face (= vector of vertex indices)
typedef std::vector<pface>                   pface_vec;       // face vector (=v ector of faces);
typedef std::vector<id_coedge>               coedge_vector;   // coedges in a polyhedron face

typedef std::unordered_map<id_edge, size_t>  edge_count_map;  // use count for edges

namespace spacemath {
   class polyhedron3d;
}

typedef std::vector<std::shared_ptr<spacemath::polyhedron3d>> ph3d_vector;  // vector of pointers to polyhedron3d

namespace spacemath {

   // polyhedron in 3d space
   class SPACEMATH_PUBLIC polyhedron3d {
   public:

      polyhedron3d();
      polyhedron3d(const vtx_vec& vert);
      polyhedron3d(const vtx_vec& vert, const pface_vec& faces);
      virtual ~polyhedron3d();

      // assign new data to polyhedron
      void assign(const vtx_vec& vert, const pface_vec& faces);

      // assign faces only, keep vertices
      void assign(const pface_vec& faces);

      // clear all
      void clear();

      // vertex traversal
      inline size_t vertex_size() const          { return m_vert.size(); }
      inline const pos3d& vertex(size_t i) const { return m_vert[i]; }

      // non-const vertex overload allows modifying coordinate of existing vertex
      inline pos3d& vertex(size_t i)             { return m_vert[i]; }

      // face traversal
      inline size_t face_size() const            { return m_face.size(); }
      inline const pface& face(size_t i) const   { return m_face[i]; }

      // return face normal vector, observe the returned vector is not normalised.
      // The face area can be assessed by evaluating 0.5*normal.length()
      vec3d  face_normal(id_face iface) const;

      // compute volume of polyhedron, throws exception for other than triangular+quadrilateral faces
      double volume() const;

      // flip face index order
      void flip_face(id_face iface);

      // verify face vertex indices, throw exception on error
      void verify_face_vertex_indices(id_face iface) const;

      // verify polyhedron for common mistakes, throw exception on error
      void verify_polyhedron() const;

   public:
      // compute edge use count
      edge_count_map construct_edge_use_count() const;

      static const size_t vertex_shift = 100000000000; // constant used for encoding 2 id_vertex into one id_edge or id_coedge

      // compute edge identifier, vertex order not significant, result always positive.
      // Lowest vertex number always encoded first ("shifted left")
      static inline id_edge      EDGE(id_vertex iv0, id_vertex iv1)        { return std::min(iv0,iv1)*vertex_shift + std::max(iv0,iv1); }

      // compute coedge identifier for coedge from iv0->iv1, absolute value is the same as for EDGE,
      // but COEDGE may be negative to indicate opposite direction relative to EDGE
      static        id_coedge    COEDGE(id_vertex iv0, id_vertex iv1);

      // compute vertex pair according to EDGE rules
      static inline vertex_pair  VERTEX_PAIR(id_vertex iv0, id_vertex iv1) { return std::make_pair(std::min(iv0,iv1), std::max(iv0,iv1)); }

   private:
      vtx_vec    m_vert;    // polyhedron vertex vector
      pface_vec  m_face;    // polyhedron face vector, faces refer to m_vert
   };

}

#endif // POLYHEDRON3D_H
