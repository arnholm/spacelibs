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
#include "polyremesh.h"
#include "spacemath/line3d.h"
#include <limits>
#include <sstream>
#include <iomanip>
#include <cmath>

static const double pi = 4.0*atan(1.0);

polyremesh::polyremesh(const std::shared_ptr<polyhedron3d> poly, double dtol, double edge_len)
: m_poly(poly)
, m_dtol(dtol)
, m_edge_len(edge_len)
, m_min_aspect_ratio(0.1)
{}

polyremesh::~polyremesh()
{}

polyremesh::edge_length_map polyremesh::compute_face_aspect_ratio_edges()
{
   edge_length_map edge_lengths;

   std::unordered_set<id_edge> done_edges;

   // traverse all faces
   const face_edge_map& face_edges = m_poly.get_face_edges();
   for(auto p : face_edges) {
      id_face   iface = p.first;
      edge_set& edges = p.second;

      // check if the face has poor aspect ratio
      double aspect_ratio = m_poly.face_aspect_ratio(iface);
      if(aspect_ratio < m_min_aspect_ratio) {

         // find the longest face edge and add it to the set
         edge_length_map face_edge_lengths;
         for(id_edge iedge : edges) {
            face_edge_lengths.insert(std::make_pair(m_poly.edge_length(iedge),iedge));
         }

         // keep the edge if not seen before
         auto i =  face_edge_lengths.rbegin();
         if(done_edges.find(i->second) == done_edges.end()) {
            edge_lengths.insert(std::make_pair(i->first,i->second));
            done_edges.insert(i->second);
         }
      }
   }
   return std::move(edge_lengths);
}

polyremesh::edge_length_map polyremesh::compute_edge_lengths()
{
   edge_length_map edge_lengths;

   // traverse edges and sort on edge length
   const edge_face_map& edges = m_poly.get_edge_faces();
   for(auto& p : edges) {
      id_edge iedge = p.first;

      // edge length
      const vertex_pair& vp = m_poly.edge_vertices(iedge);
      const pos3d& p1 = m_poly.vertex(vp.first);
      const pos3d& p2 = m_poly.vertex(vp.second);
      double elen = p1.dist(p2);

      // TODO: replace this statement with an evaluation of edge elngth as function of xyz
      double tlen = m_edge_len;

      if(elen > tlen) {
         edge_lengths.insert(std::make_pair(elen,iedge));
      }
   }

   return std::move(edge_lengths);
}

void polyremesh::flip_split()
{
   size_t maxiter = 1000;
   size_t icount  = 0;

   // iterate until no more edges are flipped or split
   while( (icount<maxiter)) {

      // reset the counter in every iteration
      size_t nflip_split = 0;

      // obtain edges longer than limit and split them
      // note that edges will be added and removed as we traverse,
      // but it seems to be ok because if an edge no longer exists
      // it will return zero faces and therefore ignored.
      edge_length_map edge_lengths = compute_edge_lengths();
      for(auto i=edge_lengths.rbegin(); i!= edge_lengths.rend(); i++) {
         id_edge iedge = i->second;

         // check this edge
         nflip_split += flip_split(iedge);
      }

      // terminate iteration if nothing happened in this round
      if(nflip_split==0) break;
      icount++;
   }

   // remeshing completed
   // copy the mutable data to the static polyhedron
   m_poly.update_input();
}

void  polyremesh::aspect_ratio_flip()
{
   size_t maxiter = 100;
   size_t icount  = 0;
   // iterate until no more edges are flipped or split
   while( (icount<maxiter)) {

     // reset the counter in every iteration
      size_t nflip_split = 0;

      edge_length_map edge_lengths = compute_face_aspect_ratio_edges();
      for(auto p : edge_lengths) {
         id_edge iedge = p.second;
         nflip_split += flip_edge(iedge);
      }

      // terminate iteration if nothing happened in this round
      if(nflip_split==0) break;
      icount++;
   }

   // remeshing completed
   // copy the mutable data to the static polyhedron
   m_poly.update_input();
}

size_t polyremesh::flip_edge(id_edge iedge)
{
   size_t nflip_split = 0;
   auto& faces = m_poly.get_edge_faces(iedge);
   if(faces.size() == 2) {

      // this is a 2-manifold edge
      // get the relevant existing vertices
      const vertex_pair& vp = m_poly.edge_vertices(iedge);
      id_vertex iv1   = vp.first;
      id_vertex iv2   = vp.second;
      const pos3d& p1 = m_poly.vertex(iv1);
      const pos3d& p2 = m_poly.vertex(iv2);
      line3d edge_line(p1, p2);

      auto i = faces.begin();
      id_face face1   = *i++;
      id_face face2   = *i;

      // get opposite vertices
      id_vertex iv3   = m_poly.opposite_vertex(face1,iedge);
      id_vertex iv4   = m_poly.opposite_vertex(face2,iedge);
      const pos3d& p3 = m_poly.vertex(iv3);
      const pos3d& p4 = m_poly.vertex(iv4);
      line3d flip_line(p3, p4);

      double flip_dist = +std::numeric_limits<double>::max();
      double edge_par  = -std::numeric_limits<double>::max();
      double flip_par  = -std::numeric_limits<double>::max();
      bool on_edge     = false;
      if(edge_line.intersect(flip_line,edge_par,flip_par)) {

         // get corresponding positions on both lines
         pos3d pline = edge_line.interpolate(edge_par);
         pos3d pflip = flip_line.interpolate(flip_par);
         flip_dist   = pline.dist(pflip);

         // ok, we shall flip this edge.
         if(flip_dist < m_dtol) {

            if((0.0<flip_par) && (flip_par<1.0) &&   // must be on flip line
               (0.0<edge_par) && (edge_par<1.0)  ) { // must be on edge line
               // we are on the edge, but check distances
               if( (p1.dist(pline) >= m_dtol) && (p2.dist(pline) >= m_dtol)  && (p3.dist(pflip) >= m_dtol) && (p4.dist(pflip) >= m_dtol) ) {
                  // we have a real intersection within bounds of both edge and flip lines
                  on_edge = true;
               }
            }
            else  {
               // planar intersection outside parameter range(s)
               // looks like degenerate case, we skip this
               return 0;
            }

            if(on_edge) {

               // we flip only if the new aspect ratio is an improvement and above limit
               double aspect_old = std::min(m_poly.face_aspect_ratio(face1),m_poly.face_aspect_ratio(face2));
               double aspect_new = std::min(m_poly.face_aspect_ratio(iv1,iv3,iv4),m_poly.face_aspect_ratio(iv3,iv2,iv4));

               if((aspect_new > aspect_old) && (aspect_new > m_min_aspect_ratio)) {
                  // first get face normal from face1 to use as guide
                  vec3d normal1 = m_poly.face_normal(face1);

                  // then remove the 2 old faces
                  remove_face(face1,iedge);
                  remove_face(face2,iedge);

                  // add the replacement faces with check of normal vector
                  add_face(normal1,iv1,iv3,iv4);
                  add_face(normal1,iv3,iv2,iv4);

                  nflip_split++;
               }
            }

         }
      }
   }
   return nflip_split;
}

size_t polyremesh::flip_split(id_edge iedge)
{
   size_t nflip_split = 0;

   // get neighbour faces
   auto& faces = m_poly.get_edge_faces(iedge);
   if(faces.size() > 0) {

      // get the relevant existing vertices
      const vertex_pair& vp = m_poly.edge_vertices(iedge);
      id_vertex iv1   = vp.first;
      id_vertex iv2   = vp.second;

      // verify that the edge is longer than the limit
      const pos3d& p1 = m_poly.vertex(iv1);
      const pos3d& p2 = m_poly.vertex(iv2);
      line3d edge_line(p1,p2);
      double edge_len = edge_line.length();

      if(faces.size() == 1) {

         // Case 1
         // this is a free edge, we split it half way
         id_face face  = *faces.begin();
         id_vertex iv3 = m_poly.opposite_vertex(face,iedge);
         vec3d normal  = m_poly.face_normal(face);

         // add the new vertex
         pos3d pmid = edge_line.interpolate(0.5);
         id_vertex iv4 = m_poly.add_vertex(pmid);

         // remove the old face
         remove_face(face,iedge);

         // add 2 faces with check of normal vector
         add_face(normal,iv1,iv3,iv4);
         add_face(normal,iv3,iv2,iv4);

         // indicate we performed a flip_split
         nflip_split++;

      }
      else if(faces.size() == 2) {

         // Cases 2, 3 or 4
         // this is a 2-manifold edge

         // we may flip or split
         auto i = faces.begin();
         id_face face1 = *i++;
         id_face face2 = *i;

         // get opposite vertices
         id_vertex iv3 = m_poly.opposite_vertex(face1,iedge);
         id_vertex iv4 = m_poly.opposite_vertex(face2,iedge);
         const pos3d& p3 = m_poly.vertex(iv3);
         const pos3d& p4 = m_poly.vertex(iv4);
         line3d flip_line(p3,p4);

/*  FOR DEBUG USE
    when an exception is thrown in add_face, stop the debugger there, and move up the call stack to get the vertex numbers here.
    Uncomment the if statement below and set the right numbers. recompile and set unconditional breakpoint inside the if statement.

         if(  iv1==2310 &&
              iv2==4024 &&
              iv3==2089 &&
              iv4==4075 ) {
              bool breakpoint=true;
         }
*/

         // compute the flip distance, i.e. closest distance
         // between edge line and flip line

         double flip_dist = +std::numeric_limits<double>::max();
         double edge_par  = -std::numeric_limits<double>::max();
         double flip_par  = -std::numeric_limits<double>::max();
         bool on_edge     = false;
         if(edge_line.intersect(flip_line,edge_par,flip_par)) {

            // get corresponding positions on both lines
            pos3d pline = edge_line.interpolate(edge_par);
            pos3d pflip = flip_line.interpolate(flip_par);
            flip_dist   = pline.dist(pflip);

            // check if the intersection is on the original edge,
            // and well within the bounds of the ends (within tolerance)

            if(flip_dist < m_dtol) {
               // this is a planar intersection

               if((0.0<flip_par) && (flip_par<1.0) &&   // must be on flip line
                  (0.0<edge_par) && (edge_par<1.0)  ) { // must be on edge line
                  // we are on the edge, but check distances
                  if( (p1.dist(pline) >= m_dtol) && (p2.dist(pline) >= m_dtol)  && (p3.dist(pflip) >= m_dtol) && (p4.dist(pflip) >= m_dtol) ) {
                     // we have a real intersection within bounds of both edge and flip lines
                     on_edge = true;
                  }
               }
               else  {
                  // planar intersection outside parameter range(s)
                  // looks like degenerate case, we skip this
                  return 0;
               }
            }
         }
         else {
            throw std::logic_error("polyremesh::flip_split error: original and flip edges are paralell");
         }

         double flip_len = flip_line.length();
         // the edge must be genuinely longer than flip_len to make a flip
         // or else we might end up re-flipping later => eternal loop
         if((edge_len>flip_len) &&  (flip_dist <= m_dtol) && on_edge) {

            // Case 2
            // ok, we shall flip this edge.

            // first get face normal from face1 to use as guide
            vec3d normal = m_poly.face_normal(face1);

            // then remove the 2 old faces
            remove_face(face1,iedge);
            remove_face(face2,iedge);

            // add the replacement faces with check of normal vector
            add_face(normal,iv1,iv3,iv4);
            add_face(normal,iv3,iv2,iv4);

            // indicate we performed a flip_split
            nflip_split++;
         }
         else {

            // cases 3 or 4
            // if we end up here, it is because the input edge was shorter than the flip edge
            // or the vertical separation was greater than tolerance (non-parallell faces)
            // No flip was possible. we try a split instead

            if(flip_dist <= m_dtol) {

               // Case 3
               // the faces are close to parallell, so we can use a weighted
               // position of the vertex
               pos3d pmid = edge_line.interpolate(edge_par);

               // first get face normals from to use as guide
               vec3d normal1 = m_poly.face_normal(face1);
               vec3d normal2 = m_poly.face_normal(face2);

               // we treat the faces as parallel
               double area1 = m_poly.face_area(face1);
               double area2 = m_poly.face_area(face2);
               // relative weight of area1
               double par = area1/(area1+area2);

               // weighted position between v3 and v4, moving the pos in direction of largest area,
               // but also taking into account the original edge ends p1,p2.
               // The fact that p1,p2 is used, avoids collapsed faces when the edge is splitting 2 right angle triangles ("back to back")
               pmid  = p4 + par*(p3-p4);
               pmid  = 0.25*(p1+p2+2*pmid);

               // add the new vertex
               id_vertex iv5 = m_poly.add_vertex(pmid);

               // then remove the 2 old faces
               remove_face(face1,iedge);
               remove_face(face2,iedge);

               // add new faces

               // replacing face1
               add_face(normal1,iv1,iv3,iv5);
               add_face(normal1,iv3,iv2,iv5);

               // replacing face2
               add_face(normal2,iv2,iv4,iv5);
               add_face(normal2,iv4,iv1,iv5);

               // indicate we performed a flip_split
               nflip_split++;

            }
            else {

               // Case 4
               // the faces are not parallel

               // check that pmid is significant distance from p1,p2
               pos3d pmid = edge_line.interpolate(0.5);
               double d1 = pmid.dist(edge_line.end1());
               double d2 = pmid.dist(edge_line.end2());

               if( (d1 > m_dtol) && (d2 > m_dtol) ) {

                  // add the new vertex
                  id_vertex iv5 = m_poly.add_vertex(pmid);

                  // first get face normals to use as guides for the new faces
                  vec3d normal1 = m_poly.face_normal(face1);
                  vec3d normal2 = m_poly.face_normal(face2);

                  // then remove the 2 old faces
                  remove_face(face1,iedge);
                  remove_face(face2,iedge);

                  // add new faces

                  // replacing face1
                  add_face(normal1,iv1,iv3,iv5);
                  add_face(normal1,iv3,iv2,iv5);

                  // replacing face2
                  add_face(normal2,iv2,iv4,iv5);
                  add_face(normal2,iv4,iv1,iv5);

                  // indicate we performed a flip_split
                  nflip_split++;
               }
            }
         }
      }
      else {
         // this is an N-manifold edge, use count>2
         // Not supported here
      }
   }

   return nflip_split;
}


id_face  polyremesh::add_face(const vec3d& normal, id_vertex iv1, id_vertex iv2, id_vertex iv3)
{
   std::string message;
   double elen1 = m_poly.edge_length(iv1,iv2);
   double elen2 = m_poly.edge_length(iv2,iv3);
   double elen3 = m_poly.edge_length(iv3,iv1);
   if( (elen1<m_dtol) || (elen1<m_dtol) || (elen1<m_dtol) ) {
      std::stringstream out;
      out << "polyremesh::add_face, very short or collapsed face edge detected" <<  std::endl;
      message = out.str();
   }

   const pos3d& p1 = m_poly.vertex(iv1);
   const pos3d& p2 = m_poly.vertex(iv2);
   const pos3d& p3 = m_poly.vertex(iv3);
   line3d line(p1,p2);
   pos3d p = line.interpolate(line.project(p3));
   if(p.dist(p3) < m_dtol) {
      std::stringstream out;
      std::streamsize w=10;
      out << "polyremesh::add_face, co-linear face edges detected, dist_tol="<<m_dtol <<" proj_dist="<< p.dist(p3) << std::endl;
      out << "iv1="<< std::setw(5) <<iv1 << std::setw(w) << p1.x() << ' ' << std::setw(w) << p1.y() << ' ' << std::setw(w) << p1.z() << std::endl;
      out << "iv2="<< std::setw(5) <<iv2 << std::setw(w) << p2.x() << ' ' << std::setw(w) << p2.y() << ' ' << std::setw(w) << p2.z() << std::endl;
      out << "iv3="<< std::setw(5) <<iv3 << std::setw(w) << p3.x() << ' ' << std::setw(w) << p3.y() << ' ' << std::setw(w) << p3.z() << std::endl;
      out << "elen1,2,3="<< std::setw(w) << elen1 << ' ' << std::setw(w) << elen2 << ' ' << std::setw(w) <<  elen3  << std::endl;
      message += out.str();
   }

   if(message.length()>0) {
      throw std::logic_error(message);
   }


   vec3d v1 = m_poly.face_normal(iv1,iv2,iv3);
   if(v1.dot(normal) > 0) {
      return m_poly.add_face(iv1,iv2,iv3);
   }
   id_face iface = m_poly.add_face(iv3,iv2,iv1);
   return iface;
}
