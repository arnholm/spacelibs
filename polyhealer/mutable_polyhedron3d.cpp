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
#include "mutable_polyhedron3d.h"
#include "spacemath/line3d.h"
#include <stdexcept>

mutable_polyhedron3d::mutable_polyhedron3d(const std::shared_ptr<polyhedron3d> poly)
: m_poly(poly)
{
   clear();
   construct();
}

mutable_polyhedron3d::~mutable_polyhedron3d()
{
   //dtor
}

void mutable_polyhedron3d::clear()
{
   m_vert.clear();
   m_face.clear();
   m_face_counter = 0;

   m_edge_faces.clear();
   m_face_edges.clear();
   m_edge_vert.clear();
}

void mutable_polyhedron3d::construct()
{
   // basic data as maps
   size_t nvert = m_poly->vertex_size();
   for(size_t i=0;i<nvert; i++) m_vert[i] = m_poly->vertex(i);

   size_t nface = m_poly->face_size();
   for(size_t i=0;i<nface; i++) m_face[i] = m_poly->face(i);
   m_face_counter = m_face.size();

   // extended data
   m_edge_faces = construct_edge_faces();
   m_edge_vert  = construct_edge_vertices();

   // face_edges = inverse of edge_faces
   for(auto& p: m_edge_faces) {
      id_edge          edge = p.first;
      const face_set& faces = p.second;
      for(auto face : faces) {
         m_face_edges[face].insert(edge);
      }
   }
}


edge_face_map mutable_polyhedron3d::construct_edge_faces()
{
   edge_face_map edge_faces;

   size_t nface = m_poly->face_size();
   for(size_t iface=0; iface<nface; iface++) {

      const pface& face = m_poly->face(iface);

      // number of edges == number of vertices
      size_t nedge     = face.size();
      size_t last_edge = nedge-1;
      for(size_t iedge=0; iedge<nedge; iedge++) {
         id_vertex   iv0 = face[iedge];
         id_vertex   iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
         id_edge edge_id =  m_poly->EDGE(iv0,iv1);
         edge_faces[edge_id].insert(iface);
      }
   }

   return std::move(edge_faces);
}

edge_vertex_map mutable_polyhedron3d::construct_edge_vertices()
{
   edge_vertex_map edges;
   size_t nface =  m_poly->face_size();
   for(id_face iface=0; iface<nface; iface++) {

      const pface& face =  m_poly->face(iface);

      // number of edges == number of vertices
      size_t nedge     = face.size();
      size_t last_edge = nedge-1;
      for(size_t iedge=0; iedge<nedge; iedge++) {
         id_vertex iv0 = face[iedge];
         id_vertex iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
         id_edge  edge = m_poly->EDGE(iv0,iv1);
         edges[edge]   = m_poly->VERTEX_PAIR(iv0,iv1);
      }
   }

   return std::move(edges);
}

edge_count_map mutable_polyhedron3d::construct_edge_use_count()
{
   // perform edge use count
   edge_count_map edge_count;
   size_t nface = m_poly->face_size();
   for(size_t iface=0; iface<nface; iface++) {

      const pface& face = m_poly->face(iface);

      // count edges
      // number of edges == number of vertices
      size_t nedge     = face.size();
      size_t last_edge = nedge-1;
      for(size_t iedge=0; iedge<nedge; iedge++) {
         id_vertex   iv0 = face[iedge];
         id_vertex   iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
         id_edge edge_id = m_poly->EDGE(iv0,iv1);
         edge_count[edge_id]++;
      }
   }

   return std::move(edge_count);
}


id_vertex mutable_polyhedron3d::add_vertex(const pos3d& pos)
{
   id_vertex ivert = m_vert.size();
   m_vert[ivert] = pos;
   return ivert;
}

id_face mutable_polyhedron3d::add_face(id_vertex iv1,id_vertex iv2,id_vertex iv3)
{
   // new face id
   id_face iface = ++m_face_counter;

   // insert the face
   pface face = {iv1,iv2,iv3};
   m_face.insert(std::make_pair(iface,face));

   // 3 edge id's if this face
   id_edge edge1 = polyhedron3d::EDGE(iv1,iv2);
   id_edge edge2 = polyhedron3d::EDGE(iv2,iv3);
   id_edge edge3 = polyhedron3d::EDGE(iv3,iv1);

   // update edge->vertex relations
   m_edge_vert[edge1] = polyhedron3d::VERTEX_PAIR(iv1,iv2);
   m_edge_vert[edge2] = polyhedron3d::VERTEX_PAIR(iv2,iv3);
   m_edge_vert[edge3] = polyhedron3d::VERTEX_PAIR(iv3,iv1);

   // update edge to face relations
   m_edge_faces[edge1].insert(iface);
   m_edge_faces[edge2].insert(iface);
   m_edge_faces[edge3].insert(iface);

   // update face ro edge relations
   m_face_edges[iface].insert(edge1);
   m_face_edges[iface].insert(edge2);
   m_face_edges[iface].insert(edge3);

   return iface;
}

void mutable_polyhedron3d::remove_face(id_face iface, id_edge iedge)
{
   // get edges referenced and remove references to this face
   edge_set& edges = m_face_edges[iface];
   for(auto edge : edges) {
      m_edge_faces[edge].erase(iface);
   }

   // remove face from face edges and finally face map itself
   m_face_edges.erase(iface);
   m_face.erase(iface);

   m_edge_faces.erase(iedge);
   m_edge_vert.erase(iedge);
}

void mutable_polyhedron3d::remove_vertex(id_vertex iv)
{
   m_vert.erase(iv);
}


face_set mutable_polyhedron3d::get_face_neighbour_faces(id_face iface)  const
{
   const pface& face = this->face(iface);

   face_set neighbour_faces;

   // number of edges == number of vertices
   size_t nedge     = face.size();
   size_t last_edge = nedge-1;
   for(size_t iedge=0; iedge<nedge; iedge++) {
      id_vertex   iv0 = face[iedge];
      id_vertex   iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
      id_edge edge_id = polyhedron3d::EDGE(iv0,iv1);

      auto it         = m_edge_faces.find(edge_id);
      if(it == m_edge_faces.end()) throw std::logic_error("mutable_polyhedron3d::get_face_neighbour_faces(...) edge not found in edge_faces");

      const face_set& faces = it->second;
      for(id_face neighbour_face : faces) {
         if(neighbour_face != iface) neighbour_faces.insert(neighbour_face);
      }
   }

   return std::move(neighbour_faces);
}


id_vertex mutable_polyhedron3d::opposite_vertex(id_face iface, id_edge iedge)
{
   const pface& face = m_face[iface];
   std::unordered_set<id_vertex> vt;
   for(auto iv : face)vt.insert(iv);

   const vertex_pair& p = m_edge_vert[iedge];
   vt.erase(p.first);
   vt.erase(p.second);

   if(vt.size() != 1) throw std::logic_error("mutable_polyhedron3d::opposite_vertex, ambigous input");

   return *vt.begin();
}

std::shared_ptr<polyhedron3d> mutable_polyhedron3d::update_input()
{
   vtx_vec   vert;
   vert.reserve(m_vert.size());

   // build vertex vector and permutation vector: id_vertex iv_new = vtx[iv_old]
   std::unordered_map<id_vertex,id_vertex> vtx;

   for(auto& p : m_vert) {
      id_vertex iv_old = p.first;
      id_vertex iv_new = vert.size();
      vtx[iv_old]      = iv_new;
      vert.push_back(p.second);
   }

   pface_vec faces;
   faces.reserve(m_face.size());
   for(auto& p : m_face) {
      const pface& face_old = p.second;
      pface face_new;
      face_new.reserve(face_old.size());
      for(auto iv_old : face_old) face_new.push_back(vtx[iv_old]);
      faces.push_back(face_new);
   }

   // update the polyhedron
   m_poly->assign(vert,faces);

   return m_poly;
}

double  mutable_polyhedron3d::edge_length(id_vertex iv1, id_vertex iv2) const
{
   const pos3d& p1 = vertex(iv1);
   const pos3d& p2 = vertex(iv2);
   return p1.dist(p2);
}

double mutable_polyhedron3d::edge_length(id_edge iedge) const
{
   const vertex_pair& vp = edge_vertices(iedge);
   return edge_length(vp.first,vp.second);
}

vec3d mutable_polyhedron3d::face_normal(id_vertex iv1, id_vertex iv2, id_vertex iv3)
{
   const pos3d& p1 = vertex(iv1);
   const pos3d& p2 = vertex(iv2);
   const pos3d& p3 = vertex(iv3);

   vec3d v1(p1,p2);
   vec3d v2(p1,p3);
   vec3d norm = v1.cross(v2);
   norm.normalise();
   return norm;
}

double mutable_polyhedron3d::face_area(id_vertex iv1, id_vertex iv2, id_vertex iv3)
{
   const pos3d& p1 = vertex(iv1);
   const pos3d& p2 = vertex(iv2);
   const pos3d& p3 = vertex(iv3);
   vec3d v1(p1,p2);
   vec3d v2(p1,p3);
   double area = 0.5*v1.cross(v2).length();
   return area;
}

double  mutable_polyhedron3d::face_area(id_face iface)
{
   const pface& vind = face(iface);
   if(vind.size() != 3)throw std::logic_error("mutable_polyhedron3d::face_area, face is not a triangle");

   return face_area(vind[0],vind[1],vind[2]);
}

double mutable_polyhedron3d::face_aspect_ratio(id_vertex iv1, id_vertex iv2, id_vertex iv3)
{
   const pos3d& p1 = vertex(iv1);
   const pos3d& p2 = vertex(iv2);
   const pos3d& p3 = vertex(iv3);

   // we define aspect ratio to be defined from the
   // longest edge and the projection distance of opposite vertex down to the same edge

   line3d line1(p1,p2);
   line3d line2(p2,p3);
   line3d line3(p1,p3);

   // figure out which edge/vertex pair is most relevant
   typedef std::pair<line3d,pos3d> aspect_pair;
   std::map<double,aspect_pair> edges;
   edges[line1.length()] = std::make_pair(line1,p3);
   edges[line2.length()] = std::make_pair(line2,p1);
   edges[line3.length()] = std::make_pair(line3,p2);

   // use the longest edge
   auto i = edges.rbegin();
   const auto& p = *i;
   const line3d& line = (p.second).first;
   const pos3d& pos   = (p.second).second;
   pos3d ppos = line.interpolate(line.project(pos));
   double dmin = pos.dist(ppos);
   return dmin/line.length();
}

double mutable_polyhedron3d::face_aspect_ratio(id_face iface)
{
   const pface& vind = face(iface);
   if(vind.size() != 3)throw std::logic_error("mutable_polyhedron3d::face_aspect_ratio, face is not a triangle");
   return face_aspect_ratio(vind[0],vind[1],vind[2]);
}


vec3d  mutable_polyhedron3d::face_normal(id_face iface)
{
   const pface& vind = face(iface);
   if(vind.size() != 3)throw std::logic_error("mutable_polyhedron3d::face_area, face is not a triangle");
   return face_normal(vind[0],vind[1],vind[2]);
}

void  mutable_polyhedron3d::face_flip(id_face iface)
{
   pface& face = m_face[iface];
   std::reverse(face.begin(),face.end());
}

coedge_vector mutable_polyhedron3d::get_face_coedges(id_face iface)
{
   const pface& face = m_face[iface];
   size_t nedge      = face.size();

   // number of edges == number of vertices
   coedge_vector coedges;
   coedges.reserve(nedge);

   size_t last_edge = nedge-1;
   for(size_t iedge=0; iedge<nedge; iedge++) {
      id_vertex   iv0   = face[iedge];
      id_vertex   iv1   = (iedge<last_edge)? face[iedge+1] : face[0];
      id_coedge icoedge = polyhedron3d::COEDGE(iv0,iv1);
      coedges.push_back(icoedge);
   }
   return std::move(coedges);
}
