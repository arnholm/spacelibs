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

#include "polyhedron3d.h"
#include <algorithm>    // std::reverse
#include <string> // std::to_string
#include <stdexcept>
#include "vmath/vector_math.h"

namespace spacemath {

   polyhedron3d::polyhedron3d()
   {}

   polyhedron3d::polyhedron3d(const vtx_vec& vert)
   : m_vert(vert)
   {}

   polyhedron3d::polyhedron3d(const vtx_vec& vert, const pface_vec& faces)
   : m_vert(vert)
   , m_face(faces)
   {}

   polyhedron3d::~polyhedron3d()
   {}

   void polyhedron3d::clear()
   {
      m_vert.clear();
      m_face.clear();
   }

   void polyhedron3d::assign(const vtx_vec& vert, const pface_vec& faces)
   {
      m_vert = vert;
      m_face = faces;
   }

   void polyhedron3d::assign(const pface_vec& faces)
   {
      m_face = faces;
   }

   id_coedge polyhedron3d::COEDGE(id_vertex iv0, id_vertex iv1)
   {
      id_edge   iedge        = EDGE(iv0,iv1);
      id_coedge icoedge_test = iv0*vertex_shift + iv1;
      id_coedge icoedge      = (icoedge_test == iedge)? iedge : -iedge;
      return icoedge;
   }


   edge_count_map polyhedron3d::construct_edge_use_count() const
   {
      // perform edge use count
      edge_count_map edge_count;
      size_t nface = this->face_size();
      for(size_t iface=0; iface<nface; iface++) {

         const pface& face = this->face(iface);

         // count edges
         // number of edges == number of vertices
         size_t nedge     = face.size();
         size_t last_edge = nedge-1;
         for(size_t iedge=0; iedge<nedge; iedge++) {
            id_vertex   iv0 = face[iedge];
            id_vertex   iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
            id_edge edge_id = EDGE(iv0,iv1);
            edge_count[edge_id]++;
         }
      }

      return std::move(edge_count);
   }

   vec3d  polyhedron3d::face_normal(id_face iface) const
   {
      size_t nface = this->face_size();
      if(iface >= nface)throw std::logic_error("polyhedron3d::face_normal() face index out of range");

      // zero the normal vector
      vec3d normal;

      const pface& face = m_face[iface];
      size_t n = face.size();

      if(n == 3) {

         // for triangles we simply compute a cross product
         const pos3d& p0 = vertex(face[0]);
         const pos3d& p1 = vertex(face[1]);
         const pos3d& p2 = vertex(face[2]);
         vec3d v1(p0,p1);
         vec3d v2(p0,p2);
         normal = v1.cross(v2);
      }
      else if(n > 3) {

         // N-sided polygon face, see http://thebuildingcoder.typepad.com/blog/2008/12/3d-polygon-areas.html
         pos3d b = vertex(face[n-2]);
         pos3d c = vertex(face[n-1]);
         for(size_t i=0; i<n; i++ ) {
            pos3d a = b;
            b       = c;
            c       = vertex(face[i]);
            double dx = b.y() * (c.z() - a.z());
            double dy = b.z() * (c.x() - a.x());
            double dz = b.x() * (c.y() - a.y());
            normal += vec3d(dx,dy,dz);
         }
      }

      return normal;
   }

   double  polyhedron3d::volume() const
   {
      // http://stackoverflow.com/questions/1838401/general-formula-to-calculate-polyhedron-volume

      size_t nface = this->face_size();
      if(nface==0)throw std::logic_error("polyhedron3d::volume() not implemented for polyhedron with no faces");

      using namespace vmath;
      double vol = 0.0;
      vec4<double> v0(0.0,0.0,0.0,1.0);

      for(size_t iface=0; iface<nface; iface++) {
         const pface& face = this->face(iface);
         if(face.size() == 3) {

            // 4x4 matrix of tetrahedron from triangle + origin(v4)
            const pos3d& p1 = vertex(face[0]);
            const pos3d& p2 = vertex(face[1]);
            const pos3d& p3 = vertex(face[2]);
            vec4<double> v1(p1.x(),p1.y(),p1.z(),1.0);
            vec4<double> v2(p2.x(),p2.y(),p2.z(),1.0);
            vec4<double> v3(p3.x(),p3.y(),p3.z(),1.0);
            mat4<double> mat(v1,v2,v3,v0);

            // determinant of matrix
            vol += det(mat);
         }
         else if(face.size() == 4) {

            // Note: convex quadrilateral assumed here
            const pos3d& p1 = vertex(face[0]);
            const pos3d& p2 = vertex(face[1]);
            const pos3d& p3 = vertex(face[2]);
            const pos3d& p4 = vertex(face[3]);
            vec4<double> v1(p1.x(),p1.y(),p1.z(),1.0);
            vec4<double> v2(p2.x(),p2.y(),p2.z(),1.0);
            vec4<double> v3(p3.x(),p3.y(),p3.z(),1.0);
            vec4<double> v4(p4.x(),p4.y(),p4.z(),1.0);

            // determinant of 2 matrices
            mat4<double> mat1(v1,v2,v4,v0);
            mat4<double> mat2(v4,v2,v3,v0);
            vol += det(mat1);
            vol += det(mat2);

         }
         else {
            throw std::logic_error("polyhedron3d::volume() requires triangular or quadrilateral faces only.");
         }
      }

      // The signed volume of each tetrahedron is equal to 1/6 the determinant
      return vol/6.0;
   }

   void polyhedron3d::flip_face(id_face iface)
   {
      size_t nface = this->face_size();
      if(iface >= nface)throw std::logic_error("polyhedron3d::flip_face(), face index out of range: " + std::to_string(iface));

      pface& face = m_face[iface];
      std::reverse(face.begin(),face.end());
   }


   void polyhedron3d::verify_face_vertex_indices(id_face iface) const
   {
      size_t nface = this->face_size();
      if(iface >= nface)throw std::logic_error("polyhedron3d::check_face_vertices(), face index out of range: " + std::to_string(iface));

      size_t nvert = m_vert.size();
      const pface& face = m_face[iface];
      for(auto iv : face) {
         if(iv >= nvert) {
            throw std::logic_error("polyhedron3d::check_face_vertices(), face " + std::to_string(iface) + ", vertex index out of range: " + std::to_string(iv) );
         }
      }
   }

   void polyhedron3d::verify_polyhedron() const
   {
      for(id_face iface=0; iface<m_face.size(); iface++) {
         verify_face_vertex_indices(iface);
         bool area_ok = (face_normal(iface).length()*0.5 > 0.0);
         if(!area_ok) {
            throw std::logic_error("polyhedron3d::verify_polyhedron(), face " + std::to_string(iface) + " has zero area." );
         }
      }

      double volume = this->volume();
      if(volume <= 0.0) {
         throw std::logic_error("polyhedron3d::verify_polyhedron(), volume is zero or negative: " +  std::to_string(volume) );
      }
   }

   void polyhedron3d::transform_inplace(const HTmatrix& T)
   {
      for(auto& v : m_vert) {
         v = T*v;
      }
   }

   std::shared_ptr<polyhedron3d> polyhedron3d::transform_copy(const HTmatrix& T) const
   {
      vtx_vec vert;
      vert.reserve(m_vert.size());
      for(auto& v : m_vert) {
         vert.push_back(T*v);
      }
      return std::make_shared<polyhedron3d>(vert,m_face);
   }

}
