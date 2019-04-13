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

#ifndef LOCSYS3D_H
#define LOCSYS3D_H

#include "spacemath_config.h"
#include "vec3d.h"

namespace spacemath {

   // local coordinate system in 3d space, represented as 3 orthogonal vectors
   class SPACEMATH_PUBLIC locsys3d {
   public:
      locsys3d();

      // construct from local x and z directions,
      // Y is computed as cross product zvec.cross(xvec), and final zvec=xvec.cross(yvec)
      locsys3d(const vec3d& xvec, const vec3d& zvec);
      locsys3d(const locsys3d& sys);
      ~locsys3d();

      locsys3d& operator =(const locsys3d& sys);

      const vec3d& x() const  { return m_xvec; }
      const vec3d& y() const  { return m_yvec; }
      const vec3d& z() const  { return m_zvec; }

   private:
      vec3d m_xvec;
      vec3d m_yvec;
      vec3d m_zvec;
   };
}

#endif
