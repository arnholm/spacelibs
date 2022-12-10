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
#include "polyfix.h"
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <sstream>

#include "multimap_pos3d.h"
#include "spacemath/polygon3d.h"

#include "mutable_polyhedron3d.h"

polyfix::polyfix(const std::shared_ptr<polyhedron3d> poly, double dtol, double atol, bool verbose)
: m_poly(poly)
, m_dtol(dtol)
, m_atol(atol)
, m_verbose(verbose)
{}

polyfix::~polyfix()
{}

size_t polyfix::remove_unused_vertices()
{
   mutable_polyhedron3d poly(m_poly);

   // we use a map to maintain the original vertex order
   typedef std::map<size_t,int> lookup_map;

   // vtx_use : <vertex,usecount>
   lookup_map vtx_use;

   // execute vertex use count, count faces too
   size_t nface = 0;
   for(auto i=poly.face_begin(); i!=poly.face_end(); i++) {
      nface++;
      const pface& face = i->second;
      size_t nv = face.size();
      for(size_t iv=0; iv<nv; iv++) {
         vtx_use[face[iv]]++;
      }
   }

   // the size of the use_count buffer will be less or equal to number of original vertices
   size_t num_unused = poly.vertex_size() - vtx_use.size();

   if(num_unused > 0) {

      // build the set of unreferenced vertices to be removed
      set<id_vertex> vtx_rem;
      for(auto itv=poly.vertex_begin(); itv!=poly.vertex_end(); itv++) {
         id_vertex iv = itv->first;
         if(vtx_use.find(iv) == vtx_use.end()) vtx_rem.insert(iv);
      }

      for(auto iv : vtx_rem) {
         poly.remove_vertex(iv);
      }

      m_poly = poly.update_input();
   }

   return num_unused;
}


std::pair<size_t,size_t> polyfix::merge_vertices()
{
   size_t num_removed_vertices = 0;
   size_t num_removed_faces = 0;

   typedef std::set<size_t>                            vtx_cluster;       // cluster of matching vertices (contains sorted vertex indices)
   typedef std::unordered_map<std::string,vtx_cluster> vtx_cluster_map;   // map of vertex clusters
   typedef std::unordered_map<std::string,pos3d>       vtx_cluster_pos;   // map of vertex cluster positions
   typedef std::vector<string>                         vtx_cluster_keys;  // vector of cluster keys, vector index is original vertex index
   typedef std::unordered_map<std::string,size_t>      vtx_cluster_idx;   // assigned new vertex index for each cluster
   typedef std::map<size_t,size_t>                     vtx_idx_map;       // <index_old,index_new> ,  index_new = vtx_idx_map[index_old]


   // build the vertex multimap, keyed by coordinates
   multimap_pos3d<size_t>  mmpos;
   mmpos.setTolerance(m_dtol);
   size_t nv=m_poly->vertex_size();
   for(size_t i=0;i<nv; i++) {
      mmpos.insert(make_pair(m_poly->vertex(i),i));
   }

   // map of all vertex clusters found
   vtx_cluster_map   cluster_map;
   vtx_cluster_keys  cluster_keys(nv);  // set to blank keys for all

   // post-process the vertices and get duplicates from mmpos to create clusters
   for(size_t i=0;i<nv; i++) {

      // get the vertex position
      const pos3d& pos = m_poly->vertex(i);

      // vertex cluster of matching vertices (zero based indices)
      vtx_cluster cluster;
      cluster.insert(i);

      // get the range of candidate matches
      auto range = mmpos.equal_key_range(pos);

      // traverse the candidates, so far these are only in the same shell
      for(auto it=range.begin(); it !=range.end(); it = range.next_match(it)) {

         // get the candidate vertex index, process only if this is not a "self-match"
         size_t iv_other = it->second.second;
         if(i != iv_other) {

            // get the distance to the candidate, it must be less or equal to tolerance
            // to be classified as a match
            double dist = pos.dist( m_poly->vertex(iv_other));
            if(dist <= range.tol())  {

               // ok, a match within tolerance
               // add to cluster
               cluster.insert(iv_other);
            }
         }
      }

      if(cluster.size() > 1) {

         // this vertex was part of a cluster of matching vertices
         // create the cluster lookup key
         ostringstream out;
         for(auto iv : cluster) out << '_' << iv;
         string key = out.str();

         // add to cluster map if not already there
         if(cluster_map.find(key) == cluster_map.end()) {
            cluster_map.insert(make_pair(key,cluster));
            // set the key for the vertices in the cluster
            for(auto iv : cluster) cluster_keys[iv] = key;
         }
      }

   }

   // all clusters have been computed
   // compute cluster coordinates
   vtx_cluster_pos cluster_pos;
   for(auto& p : cluster_map) {
      const string& key          = p.first;
      const vtx_cluster& cluster = p.second;
      pos3d cpos;
      for(size_t iv : cluster) {
         cpos += m_poly->vertex(iv);
      }
      cpos /= double(cluster.size());
      cluster_pos.insert(std::make_pair(key,cpos));
   }

   // create the new and smaller vertex vector with only unique vertices plus cluster vertices
   vtx_vec vert;
   vert.reserve(nv);

   // this map records which clusters have been processed and their cooresponding vertex index
   vtx_cluster_idx cluster_idx;
   vtx_idx_map  new_vert;

   // traverse original vertices
   for(size_t iv=0; iv<nv; iv++) {

       // first assign the original vertex position
       pos3d pos = m_poly->vertex(iv);

       // iv_new is the index in the new, reduced vertex vector
       size_t iv_new = std::numeric_limits<size_t>::max();

       // ckeck if part of cluster
       const string& key = cluster_keys[iv];
       if(key.length() > 0) {

          // cluster vertex
          auto it = cluster_idx.find(key);
          if(it == cluster_idx.end()) {

             // new cluster vertex, record the new vertex position
             iv_new = vert.size();
             vert.push_back(cluster_pos[key]);

             // mark the cluster as processed
             cluster_idx.insert(std::make_pair(key,iv_new));
          }
          else {
             // previously seen cluster vertex
             iv_new = it->second;
          }
       }
       else {
          // not a cluster vertex
          iv_new = vert.size();
          vert.push_back(pos);
       }

       // keep the mapping from old to new vertex
       new_vert[iv] = iv_new;
   }

   // compute the number of removed vertices
   num_removed_vertices = nv - vert.size();

   // create polyhedron faces with renumbered indices
   // and check for collapsed faces
   size_t nface = m_poly->face_size();
   pface_vec faces;
   faces.reserve(nface);

   int num_zero = 0;

   for(size_t iface=0; iface<nface; iface++) {

      const pface& face_old = m_poly->face(iface);
      pface face_new;
      face_new.reserve(face_old.size());

      set<size_t> face_verts;
      size_t nv = face_old.size();
      for(size_t iv=0; iv<nv; iv++) {
         size_t iv_old = face_old[iv];
         size_t iv_new = new_vert[iv_old];
         face_new.push_back(iv_new);
         face_verts.insert(iv_new);
      }

      // if the sizes below are not the same, it means that a vertex was repeated, i.e. collapsed face
      // We simply skip collapsed faces
      if(face_new.size() == face_verts.size()) {

         // check for sliver face (vertices on a straight line)
         vector<pos3d> face_pos;
         for(size_t iv=0; iv<face_new.size(); iv++) {
            face_pos.push_back(vert[face_new[iv]]);
         }
         polygon3d poly_face(face_pos);
         if(poly_face.area() > m_atol) {
            faces.push_back(face_new);
         }
         else {
            num_zero++; // for debugging, count number of zero area faces ignored
         }
      }
   }

   // compute the number of removed faces
   num_removed_faces = nface - faces.size();

   // finally assign the polyhedron
   m_poly->assign(vert,faces);

   return std::make_pair(num_removed_vertices,num_removed_faces);
}

size_t  polyfix::remove_duplicate_faces()
{
   // execute face duplicate count
   unordered_set<string> face_keys;
   size_t nface = m_poly->face_size();

   // the new faces
   pface_vec faces;
   faces.reserve(nface);

   for(size_t iface=0; iface<nface; iface++) {

      const pface& face = m_poly->face(iface);
      size_t nv = face.size();

      // sort the vertex indices in increasing order
      set<size_t> sorted_face;
      for(size_t iv=0; iv<nv; iv++) {
        sorted_face.insert(face[iv]);
      }

      ostringstream out;
      for(auto iv : sorted_face) out << '_' << iv;
      string key = out.str();
      if(face_keys.find(key) == face_keys.end()) {

         // face not seen before, so keep it
         face_keys.insert(key);
         faces.push_back(face);
      }
   }

   // if any duplicates were found, faces.size() will be less than the original
   size_t num_removed_faces =  nface - faces.size();

   if(num_removed_faces>0) {
      // assign the reduced set of faces to the polyhedron
      m_poly->assign(faces);
   }

   return num_removed_faces;
}

std::pair<size_t,size_t> polyfix::remove_nonmanifold_or_zero_faces()
{
   // perform edge use count
   edge_count_map edge_count = m_poly->construct_edge_use_count();

   // the new faces
   size_t nface = m_poly->face_size();
   pface_vec faces;
   faces.reserve(nface);

   size_t num_zero_area = 0;

   // in a second pass over faces, detect faces with edge use count different from 2
   for(size_t iface=0; iface<nface; iface++) {

      const pface& face = m_poly->face(iface);

      // number of edges == number of vertices
      size_t nedge        = face.size();
      size_t nedge_nonman = 0;
      size_t last_edge = nedge-1;
      for(size_t iedge=0; iedge<nedge; iedge++) {
         size_t iv0 = face[iedge];
         size_t iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
         size_t edge_id = polyhedron3d::EDGE(iv0,iv1);
         size_t edge_used = edge_count[edge_id];
         if(edge_used != 2) {
            nedge_nonman++;
         }
      }

      // if there is at least one manifold edge, we keep the face,
      // but only if it has a non-zero area
      if(nedge_nonman < nedge) {

         // check face area
         vector<pos3d> face_pos;
         for(size_t iv=0; iv<face.size(); iv++) {
            face_pos.push_back( m_poly->vertex(face[iv]));
         }

         // check face area error
         polygon3d poly_face(face_pos);
         if(poly_face.area()>m_atol) {
            faces.push_back(face);
         }
         else {
            num_zero_area++;
         }
      }

   }

   // if any faces removed, faces.size() will be less than the original
   size_t num_removed_faces =  nface - faces.size();
   size_t num_nonmanifold = num_removed_faces - num_zero_area;

   if(num_removed_faces>0) {
      // assign the reduced set of faces to the polyhedron
      m_poly->assign(faces);
   }

   return std::make_pair(num_nonmanifold,num_zero_area);
}

double polyfix::area(size_t iface) const
{
   const pface& face = m_poly->face(iface);
   vector<pos3d> face_pos;
   for(size_t iv=0; iv<face.size(); iv++) {
      face_pos.push_back( m_poly->vertex(face[iv]));
   }
   polygon3d poly_face(face_pos);
   return poly_face.area();
}

list<string>  polyfix::check()
{
   edge_count_map edge_count;
   std::unordered_multimap<size_t,size_t> edge_faces;
   size_t face_error=0;

   list<string> warnings;

   size_t nface = m_poly->face_size();
   if(nface==0) warnings.push_back("warning: no faces");


   for(size_t iface=0; iface<nface; iface++) {

      const pface& face = m_poly->face(iface);

      // check face area
      vector<pos3d> face_pos;
      for(size_t iv=0; iv<face.size(); iv++) {
         face_pos.push_back( m_poly->vertex(face[iv]));
      }

      // check face area error
      polygon3d poly_face(face_pos);
      if(!(poly_face.area()>m_atol))face_error++;

      // count edges
      // number of edges == number of vertices
      size_t nedge     = face.size();
      size_t last_edge = nedge-1;
      for(size_t iedge=0; iedge<nedge; iedge++) {
         size_t iv0 = face[iedge];
         size_t iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
         size_t edge_id = polyhedron3d::EDGE(iv0,iv1);
         edge_count[edge_id]++;
         edge_faces.insert(make_pair(edge_id,iface));
      }
   }

   // count edge errors
   map<size_t,size_t> uc_error;
   for(auto i=edge_count.begin(); i!=edge_count.end(); i++) {
      if(i->second != 2) {
         uc_error[i->second]++;
      }
      else {
         edge_faces.erase(i->first);
      }
   }

   if(face_error > 0) {
      ostringstream out;
      out << "warning: "<< face_error << " zero area faces.";
      warnings.push_back(out.str());
   }

   if( uc_error.size() > 0) {
      ostringstream out;
      out << "warning: nonmanifold edges: ";
      for(auto p : uc_error) { out << "uc("<<p.first <<")"<< '='<< p.second << ' '; }
      warnings.push_back(out.str());

      if(m_verbose) {

         // more detailed messages
         size_t edge_counter=0;
         for(auto& p  : edge_faces) {

            size_t iface = p.second;
            const pface& face = m_poly->face(iface);

            ostringstream out;
            out << "    edge=" << p.first << " uc=" << edge_count[p.first] << " face=" << iface << ':';
            for(size_t iv=0; iv<face.size(); iv++) out << ' ' << face[iv];
            out << " area=" << area(iface);
            warnings.push_back(out.str());
            if(edge_counter > 5)break;
            edge_counter++;
         }
         size_t more_edges = edge_faces.size() - edge_counter;
         if(more_edges > 0) {
            ostringstream out;
            out << "    ... and " << more_edges << " more edges";
            warnings.push_back(out.str());
         }
      }

   }


   return std::move(warnings);
}
