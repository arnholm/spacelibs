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
#include "lump_finder.h"
#include <iostream>

lump_finder::lump_finder(const std::shared_ptr<polyhedron3d> poly)
: m_poly(poly)
{}

lump_finder::~lump_finder()
{}

std::shared_ptr<ph3d_vector> lump_finder::find_lumps()
{
   // create a set of all faces
   face_set all_faces;
   for(auto i=m_poly.face_begin(); i!=m_poly.face_end(); i++) all_faces.insert(i->first);

   // the actual lumps to be created
   lump_map lumps;

   size_t id_lump = 0;
   while(all_faces.size() > 0) {

      // start a new lump
      face_set& lump_faces = lumps[id_lump++];

      // our set of faces not yet processed
      face_set  todo_faces;

      // pick a face to start from, and repeat as long as more faces to do
      todo_faces.insert(*all_faces.begin());

      while(todo_faces.size() > 0) {

         // transfer next face from todo_faces to lump_faces
         id_face face_id = *todo_faces.begin();
         todo_faces.erase(face_id);
         lump_faces.insert(face_id);

         // look up face neigbours to this face (face_id)
         face_set neighbour_faces = m_poly.get_face_neighbour_faces(face_id);

         // collect only previously unseen neighbour faces in todo_faces
         for(id_face neighbour_face : neighbour_faces) {
            if(lump_faces.find(neighbour_face) == lump_faces.end()) {
               todo_faces.insert(neighbour_face);
            }
         }
      }

      // we now have all faces in this lump, remove them from all_faces.
      // The remaining faces in all_faces (if any) belong to other lumps
      for(id_face iface : lump_faces) {
         all_faces.erase(iface);
      }
   }

   // we have sorted all faces in lumps, create a new polyset with a polyhedron for each lump
   std::shared_ptr<ph3d_vector> polyset(new ph3d_vector);
   polyset->reserve(lumps.size());

   for(auto& p : lumps) {

      // the set of faces in the lump
      face_set& faces = p.second;

      // first collect the vertices in use by this lump
      vertex_set vtx_lump;
      for(id_face iface : faces) {
         const pface& face = m_poly.face(iface);
         for(id_vertex iv=0;iv<face.size();iv++) vtx_lump.insert(face[iv]);
      }

      // create a vector: iv_new = new_vert[iv_old]
      // where iv_new=-1 for unused vertices

      size_t nv=m_poly.vertex_size();
      std::vector<int> new_vert(nv,-1);
      int iv_new = 0;
      for(id_vertex iv_old : vtx_lump) {
         new_vert[iv_old] = iv_new++;
      }

      // create the new and smaller vertex vector with only used vertices
      vtx_vec vert(vtx_lump.size());
      for(id_vertex iv_old=0; iv_old<nv; iv_old++) {
          int iv_new = new_vert[iv_old];
          if(iv_new >= 0) {
             vert[iv_new] = m_poly.vertex(iv_old);
          }
      }

      // create polyhedron faces with renumbered indices
      // there is no need to check for unused vertices here

      // number of faces in this lump
      size_t nface = faces.size();
      pface_vec new_faces(nface);

      // face_counter counts the new faces
      size_t face_counter = 0;
      for(id_face iface : faces) {

         // iface is the old face number
         const pface& face_old = m_poly.face(iface);
         pface& face_new = new_faces[face_counter++];
         face_new.reserve(face_old.size());
         size_t nv = face_old.size();
         for(size_t iv=0; iv<nv; iv++) {
            size_t iv_old = face_old[iv];      // old vertex
            size_t iv_new = new_vert[iv_old];  // corresponding new vertex
            face_new.push_back(iv_new);
         }
      }

      // create the polyhedron for this lump and push to polyset vector
      polyset->push_back(std::shared_ptr<polyhedron3d>(new polyhedron3d(vert,new_faces)));
   }

   return polyset;
}
