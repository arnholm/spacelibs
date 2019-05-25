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
#ifndef MUTABLE_POLYHEDRON3D_H
#define MUTABLE_POLYHEDRON3D_H

#include "polyhealer_config.h"

#include "spacemath/polyhedron3d.h"
#include "spacemath/vec3d.h"
using namespace spacemath;

#include <map>
#include <unordered_map>
#include <unordered_set>

typedef std::unordered_map<id_edge, vertex_pair>   edge_vertex_map;  // edge end vertices
typedef std::unordered_set<id_edge>                edge_set;         // a set of edges
typedef std::unordered_set<id_face>                face_set;         // a set of faces
typedef std::unordered_map<id_edge, face_set>      edge_face_map;    // faces connected to an edge
typedef std::unordered_map<id_face, edge_set>      face_edge_map;    // edges connected to a face


// mutable_polyhedron3d is similar to a polyhedron3d, but supports additional
// datastructures to support operations to modify it, including remeshing operations.
// In addition, it is assumed that mutable_polyhedron3d contains only triangle faces

class POLYHEALER_PUBLIC mutable_polyhedron3d {
public:
   typedef std::map<id_vertex,pos3d>               vtx_map;
   typedef vtx_map::iterator                       vertex_iterator;

   typedef std::unordered_map<id_face, pface>      pface_map;
   typedef pface_map::iterator                     pface_iterator;

public:
   mutable_polyhedron3d(const std::shared_ptr<polyhedron3d> poly);
   virtual ~mutable_polyhedron3d();

   // update the input polyhedron to match the current mutable polyhedron
   std::shared_ptr<polyhedron3d> update_input();

   // clear all mutable polyhedron data, except input polyhedron
   void clear();


   // === BASIC VERTEX/FACE TRAVERSAL
   // -------------------------------

   // vertex traversal
   inline size_t vertex_size() const             { return m_vert.size(); }
   inline const pos3d& vertex(id_vertex i) const { return m_vert.find(i)->second; }
   inline vertex_iterator vertex_begin()         { return m_vert.begin(); }
   inline vertex_iterator vertex_end()           { return m_vert.end(); }

   // face traversal
   inline pface_iterator face_begin()            { return m_face.begin(); }
   inline pface_iterator face_end()              { return m_face.end(); }

   // get face by face_id. Note that face id's are NOT contiguous
   inline const pface& face(id_face i) const  { return m_face.find(i)->second; }


   // === ACCESS TO COMPUTED DATA STRUCTURES
   // --------------------------------------

   // return vertex pair for edge
   inline const vertex_pair& edge_vertices(id_edge iedge) const { return m_edge_vert.find(iedge)->second; }

   // return faces connected to given edge
   inline const face_set& get_edge_faces(id_edge iedge) const   { return m_edge_faces.find(iedge)->second; }

   // return access to all the edges and their connected faces
   inline const edge_face_map& get_edge_faces() const           { return m_edge_faces; }


   // return access to all faces and their connected edges
   inline const face_edge_map& get_face_edges() const           { return m_face_edges; }

   // return neighbour faces connected to given face
   face_set get_face_neighbour_faces(id_face iface) const;

   // get coedges of given face
   coedge_vector get_face_coedges(id_face iface);

   // for given face and edge, return vertex opposite to edge
   id_vertex  opposite_vertex(id_face iface, id_edge iedge);


   // === MODIFY MUTABLE POLYHEDRON
   // -----------------------------

   // add vertex to polyhedron
   id_vertex add_vertex(const pos3d& pos);

   // add face to polyhedron
   id_face add_face(id_vertex iv1, id_vertex iv2, id_vertex iv3);

   // remove face and edge from polyhedron
   void remove_face(id_face iface, id_edge iedge);

   // flip selected face
   void face_flip(id_face iface);

   // remove vertex
   // Note: it is assumed that the vertex being removed is unreferenced, no checking is done
   void remove_vertex(id_vertex iv);

   // ==== COMPUTE PROPERTIES
   // -----------------------

   // compute edge length from 2 vertices in assumed edge order
   double edge_length(id_vertex iv1, id_vertex iv2) const;

   // compute edge length
   double edge_length(id_edge iedge) const;

   // compute normal from 3 vertices in assumed face order
   double face_area(id_vertex iv1, id_vertex iv2, id_vertex iv3);

   // compute face area
   double face_area(id_face iface);

   // compute face aspect ratio from 3 vertices
   double face_aspect_ratio(id_vertex iv1, id_vertex iv2, id_vertex iv3);

   // compute face aspect ratio
   double face_aspect_ratio(id_face iface);

   // compute normal from 3 vertices in assumed face order
   vec3d  face_normal(id_vertex iv1, id_vertex iv2, id_vertex iv3);

   // compute face normal of existing face
   vec3d  face_normal(id_face iface);

public:
   // helper functions
   edge_count_map  construct_edge_use_count();
   edge_face_map   construct_edge_faces();
   edge_vertex_map construct_edge_vertices();

protected:

   // initialize all data structures belonging to
   // mutable_polyhedron3d, based on input polyhedron3d
   void construct();

private:
   std::shared_ptr<polyhedron3d>  m_poly;  // original polyhedron

   // Additional data structures
   vtx_map                        m_vert;          //  <id_vertex,pos3d>  vertices as a map, can grow more easily.
   pface_map                      m_face;          //  <id_face, pface>   faces as as a map, can add and remove faces more easily
   id_face                        m_face_counter;  //  ever growing face counter. Note: we never re-use an id when removing a face

   edge_face_map                  m_edge_faces;    //  <id_edge, face_set>     edge faces
   face_edge_map                  m_face_edges;    //  <id_face, edge_set>     face edges
   edge_vertex_map                m_edge_vert;     //  <id_edge, vertex_pair>  edge vertices
};

#endif // MUTABLE_POLYHEDRON3D_H
