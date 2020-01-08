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
#include "polyflip.h"
#include "spacemath/polygon3d.h"
#include "spacemath/vec3d.h"
#include "spacemath/line3d.h"
#include "spacemath/plane3d.h"

#include <set>
#include <limits>
#include <cmath>
#include <stdexcept>

polyflip::polyflip(const std::shared_ptr<polyhedron3d> poly, double dtol)
: m_poly(poly)
, m_dtol(dtol)
{}

polyflip::~polyflip()
{}

bool polyflip::p_in_triangle(const pos3d& p, const pos3d& a, const pos3d& b, const pos3d& c)
{
   // solution based on area coordinates
   // see figure at http://math.stackexchange.com/questions/4322/check-whether-a-point-is-within-a-3d-triangle
   // Note that the logic there is wrong

   // we simply check whether the subtriangle areas add up to more than the whole triangle
   // if yes, the point is outside

   // a,b,c are triangle corners. p is the point to check.
   // p must lie in the plane of abc

   vec3d  ab(a,b);
   vec3d  ac(a,c);
   vec3d  ap(a,p);
   vec3d  pb(p,b);
   vec3d  pc(p,c);

   // area of triangle abc
   double area_abc = 0.5*ab.cross(ac).length();

   // areas of subtriangles
   double area_abp = 0.5*ab.cross(ap).length();
   double area_pbc = 0.5*pb.cross(pc).length();
   double area_apc = 0.5*ap.cross(ac).length();

   // if the sum of subtriangle areas is greater than the abc triangle area, then the point is outside
   if((area_abp+area_pbc+area_apc) > area_abc) return false;

   // ok, point is inside triangle area
   return true;
}


size_t polyflip::count_positive_intersections(id_face iface0)
{
   // select the first face and check its orientation
   const pface& face = m_poly.face(iface0);

   // compute the face normal.  Returned vector is normalised
   std::vector<pos3d> face_pos;
   pos3d centroid;
   for(size_t iv=0; iv<face.size(); iv++) {
      const pos3d& p = m_poly.vertex(face[iv]);
      centroid += p;
      face_pos.push_back(p);
   }
   polygon3d poly_face(face_pos);
   vec3d normal;
   poly_face.area(normal);

   // face normal as a line. Must use centroid as start point to be sure about proper intersections
   centroid /= double(face.size());
   line3d normal_line(centroid,centroid+normal);

   // intersections contains all intersection distances found along the face normal
   std::set<double> intersections;

   // Count face intersections in forward direction
   for(auto i=m_poly.face_begin(); i!=m_poly.face_end(); i++) {

      id_face iface = i->first;

      // guard against self-intersection
      if(iface != iface0) {

         const pface& face = m_poly.face(iface);
         if(face.size() != 3) throw std::logic_error("polyflip::flip_faces(...) non-triangular face detected");

         // compute plane equation for the face triangle
         const pos3d& p1 = m_poly.vertex(face[0]);
         const pos3d& p2 = m_poly.vertex(face[1]);
         const pos3d& p3 = m_poly.vertex(face[2]);
         plane3d face_plane(p1,p2,p3);

         // compute intersection with normal_line and face plane.
         double line_par=-1.0;  // line_par [0.0,1.0] correspond to line [end1,end2]
         if(face_plane.intersect(normal_line,line_par)){

            // plane and line were non-parallel
            // we are only interested in intersections in the positive normal direction
            if((std::isfinite(line_par)) &&  (line_par>0.0) ) {

               // the plane intersection point is on the positive side of iface0,
               // but is it within the area of the current triangle face being checked?
               pos3d p = normal_line.interpolate(line_par);
               if(p_in_triangle(p,p1,p2,p3)) {

                  // yes, the intersection hit inside the triangle area or exactly on the border,
                  // therefore this is a real intersection.
                  // Record the distance from iface0, measured along the iface0 normal
                  intersections.insert(normal_line.end1().dist(p));
               }
            }
         }
      }

   } // next face in polyhedron

   // filter any intersections that may be too close to each other
   // this is to guard against small numerical differences in intersections
   // computed on 2 neighbouring faces (on border)
   std::set<double> tolerant_intersections;
   double dist_prev = -1.0;
   for(double dist : intersections) {
      if( (dist-dist_prev) >= m_dtol) {
         tolerant_intersections.insert(dist);
         dist_prev = dist;
      }
   }

   // finally we have the real number of intersections
   return tolerant_intersections.size();
}

size_t polyflip::flip_faces()
{
   size_t nflip = 0;
   // determine the normal orientation of the 1st polyhedron face
   // if the number of intersections is an even number (including zero), the normal is pointing outwards

   id_face iface0 = 0;
   size_t nint = count_positive_intersections(iface0);
   if(nint%2 != 0) {
      // normal is pointing inwards, flip this face at once
      m_poly.face_flip(iface0);
      nflip++;
   }

   // we have now established that iface0 is pointing out of the polyhedron
   // just follow the neighbours and make sure they are pointing the same way, using only edge winding order

   // collect the edge to face relations
   edge_face_map edge_faces = m_poly.get_edge_faces();

   // create a set of all faces, except iface0
   face_set all_faces;
 //  size_t nface = m_poly->face_size();
 //  for(id_face iface=0; iface<nface; iface++) all_faces.insert(iface);
   for(auto i=m_poly.face_begin(); i!=m_poly.face_end(); i++) all_faces.insert(i->first);
   all_faces.erase(iface0);

   // todo_faces represent those that have been seen but neigbours not processed yet
   // every face in this set will be guaranteed correctly oriented outward

   face_set todo_faces;
   todo_faces.insert(iface0);

   //done_faces = faces already checked for flipping
   face_set done_faces;

   while(todo_faces.size() > 0) {

      // we have a new face0
      id_face iface0 = *todo_faces.begin();
      todo_faces.erase(iface0);
      done_faces.insert(iface0);

      // traverse the iface0 coedges and look op neigbour faces
      coedge_vector coedges0 = m_poly.get_face_coedges(iface0);
      for(id_coedge coedge0 : coedges0) {

         // look up the neighbour faces to coedge0
         id_edge edge0 = std::abs(coedge0);
         const face_set& neighbour_faces = edge_faces[edge0];

         // check only previously unseen neighbour faces different from iface0
         // in reality neighbour_faces.size() should always be 2
         for(id_face iface1 : neighbour_faces) {
            if( (iface1!=iface0) && (done_faces.find(iface1) == done_faces.end()) )  {

               // check the face flip state of iface1 and put it into todo set
               // so we can traverse its neighbours
               nflip += check_neighbour_flip(iface0,coedge0,iface1);
               done_faces.insert(iface0);
               todo_faces.insert(iface1);
            }
         }
      }
   }

   // erase the faces done in this round, so we can check that all have been processed
   for(id_face iface : done_faces) {
      all_faces.erase(iface);
   }

   if(all_faces.size() != 0) {
      throw std::logic_error("polyflip::flip_faces(...) not all faces visited, is this polyhedron a single, connected body?");
   }

   return nflip;
}

size_t polyflip::check_neighbour_flip(id_face iface0, id_coedge coedge0, id_face iface1)
{
   coedge_vector coedges1 = m_poly.get_face_coedges(iface1);

   // find the matching coedge
   for(id_coedge coedge1 : coedges1) {
      if( std::abs(coedge0) ==  std::abs(coedge1) ) {
         // found it
         if((coedge0+coedge1) != 0) {

            // iface1 has the wrong winding order, so flip it
            m_poly.face_flip(iface1);
            return 1;
         }
         return 0;
      }
   }

   throw std::logic_error("polyflip::check_neighbour_flip(...) matching coedge not found");
}
