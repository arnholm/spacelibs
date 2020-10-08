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
#ifndef POLYFLIP_H
#define POLYFLIP_H

#include "mutable_polyhedron3d.h"
#include <memory>
#include <utility>  // std::pair
#include <string>
#include <list>

// this class checks for wrong face orientations and flips such faces.
// the distance tolerance is used only for comparing intersections, and should
//  be smaller than ordinary coordinate tolerances

class POLYHEALER_PUBLIC polyflip {
public:

   polyflip(const std::shared_ptr<polyhedron3d> poly, double dtol, double m_atol);
   virtual ~polyflip();

   // perform the actual face flipping, this modifies the polyhedron
   size_t flip_faces();

protected:
   // check if pont p is inside triangle a,b,c
   bool p_in_triangle(const pos3d& p, const pos3d& a, const pos3d& b, const pos3d& c);

   // count the intersections with other faces in the positive direction of the face normal
   size_t count_positive_intersections(id_face iface0);

   // having established iface0 and iface1 as neigbour faces, flip face1 if winding order so dictates
   size_t check_neighbour_flip(id_face iface0, id_coedge coedge0, id_face iface1);

private:
   mutable_polyhedron3d  m_poly;
   double m_dtol;     // distance tolerance
   double m_atol;     // area tolerance
};

#endif // POLYFLIP_H
