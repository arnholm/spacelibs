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
#include "polysplit.h"
#include "spacemath/line3d.h"

#include <iostream>

polysplit::polysplit(std::shared_ptr<polyhedron3d> poly, double dtol, double atol)
: m_poly(poly)
, m_face_counter(0)
, m_dtol(dtol)
, m_atol(atol)
{
   // copy all faces
   build_faces();

   // compute free edges
   build_free_edges();

   // compute all edge splits
   build_edge_splits();

   // collect all faces connected to the free edges
   build_edge_faces();
}

polysplit::~polysplit()
{}

void polysplit::build_faces()
{
   m_faces.clear();

   // copy all the faces
   size_t nface   = m_poly->face_size();
   m_face_counter = nface;
   for(size_t iface=0; iface<nface; iface++) m_faces.insert(make_pair(iface,m_poly->face(iface)));
}

void polysplit::build_free_edges()
{
   m_free_edges.clear();

   // perform edge use count
   std::map<id_edge,size_t> edge_count;
   edge_map all_edges;
   size_t nface = m_poly->face_size();
   for(size_t iface=0; iface<nface; iface++) {

      const pface& face = m_poly->face(iface);

      // count edges
      // number of edges == number of vertices
      size_t nedge     = face.size();
      size_t last_edge = nedge-1;
      for(size_t iedge=0; iedge<nedge; iedge++) {
         id_vertex iv0 = face[iedge];
         id_vertex iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
         id_edge  edge =  polyhedron3d::EDGE(iv0,iv1);
         edge_count[edge]++;
         all_edges[edge] = std::make_pair(iv0,iv1);
      }
   }

   // build free edge set
   for(auto& p : edge_count) {
      if(p.second == 1) {
         // insert free edge and its vertices
         m_free_edges.insert(std::make_pair(p.first,all_edges[p.first]));
      }
   }
}


void polysplit::build_edge_splits()
{
   m_edge_splits.clear();

   // traverse vertices
   size_t nvert = m_poly->vertex_size();
   for(size_t ivert=0; ivert<nvert; ivert++) {

      // get vertex position
      const pos3d& pos = m_poly->vertex(ivert);

      // traverse free edges and check for splits
      for(auto& p : m_free_edges) {

         // get the edge and its vertices
         id_edge   edge = p.first;
         id_vertex iv0  = (p.second).first;
         id_vertex iv1  = (p.second).second;
         // skip if the current vertex is one of the end vertices of the edge
         if( (ivert!=iv0) && (ivert!=iv1) ) {

            // compute projection onto edge line
            line3d edge_line(m_poly->vertex(iv0),m_poly->vertex(iv1));
            double par = edge_line.project(pos);
            if( par>0.0 && par<1.0 ) {
               // the projection is on the edge, is the vertex actually on the edge?
               double dist = pos.dist(edge_line.interpolate(par));
               if(dist <= m_dtol) {
                  // yes, this vertex is splitting the edge
                  m_edge_splits[edge][par] = ivert;
               }
            }
         }

      // next free edge
      }

   // next vertex
   }
}

void polysplit::build_edge_faces()
{
   m_edge_faces.clear();

   // traverse faces and register the faces affected by edge splits
   size_t nface = m_poly->face_size();
   for(size_t iface=0; iface<nface; iface++) {
      const pface& face = m_poly->face(iface);

      // number of face edges == number of vertices
      size_t nedge     = face.size();
      size_t last_edge = nedge-1;
      for(size_t iedge=0; iedge<nedge; iedge++) {
         id_vertex iv0 = face[iedge];
         id_vertex iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
         id_edge  edge =  polyhedron3d::EDGE(iv0,iv1);
         if(m_free_edges.find(edge) != m_free_edges.end()) {
            // this is one of the free edges, therefore add this face to its reference
            m_edge_faces[edge].insert(iface);
         }
      }
   }
}


size_t polysplit::split_faces()
{
   std::set<id_face> faces_done;
   for(auto& p : m_edge_splits) {

      id_edge                      edge = p.first;   // the edge to be split
      std::map<double,id_vertex>& verts = p.second;  // the vertices splitting, in increasing parameter order

      // traverse the affected faces
      std::set<id_face>& faces = m_edge_faces[edge];
      for(auto& iface : faces) {

         // check that this face was not previously split
         if(faces_done.find(iface) == faces_done.end()) {
            split_face(iface,edge,verts);
            faces_done.insert(iface);
         }
      }
   }

   // all affected faces have been replaced,
   // rebuild the face vector and assign to polyhedron
   size_t nface = m_faces.size();
   pface_vec faces;
   faces.reserve(nface);
   for(auto& p : m_faces) {
      faces.push_back(p.second);
   }

   // finally assign the cleaned-up faces to polyhedron
   m_poly->assign(faces);

   return faces_done.size();
}

void polysplit::split_face(id_face iface, id_edge edge, std::map<double,id_vertex>& verts)
{
   // we need the two edge vertices
   auto& vp = m_free_edges[edge];
   id_vertex iv0 = vp.first;
   id_vertex iv1 = vp.second;
   id_vertex iv2 = std::numeric_limits<size_t>::max();

   // get the face vertices
   const pface& face = m_faces[iface];
   // get the 3rd vertex
   for(auto iv : face) {
      if( (iv!=iv0) && (iv!=iv1) ) {
         iv2 = iv;
         break;
      }
   }

   // ok, we have the 3 vertices iv0, iv1, iv2 defining the old vertex.
   // iv2 shall be kept in all new faces

   // add new faces before the splits
   id_vertex ivA = iv0;
   for(auto& split : verts) {
      id_vertex ivB = split.second;
      pface face_vert = { iv2, ivA, ivB };
      m_faces.insert(std::make_pair(++m_face_counter,face_vert));
      ivA = ivB;
   }

   // add final face
   pface face_vert = { iv2, ivA, iv1 };
   m_faces.insert(std::make_pair(++m_face_counter,face_vert));

   // erase the face that was split
   m_faces.erase(iface);
}
