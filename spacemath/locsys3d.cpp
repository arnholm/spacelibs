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

#include "locsys3d.h"
#include <math.h>

namespace spacemath {

   locsys3d::locsys3d()
    : m_xvec( vec3d(1.,0.,0.) ),
      m_yvec( vec3d(0.,1.,0.) ),
      m_zvec( vec3d(0.,0.,1.) )
   {
   }

   locsys3d::locsys3d(const vec3d& xvec, const vec3d& zvec)
   : m_xvec(xvec),m_zvec(zvec)
   {
      m_xvec.normalise();
      m_zvec.normalise();
      m_yvec = m_zvec.cross( m_xvec );
      m_yvec.normalise();
      m_zvec = m_xvec.cross( m_yvec ); // ensure to make z axis orthogonal to x and y
      m_zvec.normalise();
   }

   locsys3d::locsys3d(const locsys3d& sys)
    : m_xvec(sys.m_xvec),m_yvec(sys.m_yvec),m_zvec(sys.m_zvec)
   {
   }

   locsys3d::~locsys3d()
   {
   }

   locsys3d& locsys3d::operator =(const locsys3d& sys)
   {
      m_xvec = sys.m_xvec;
      m_yvec = sys.m_yvec;
      m_zvec = sys.m_zvec;
      return *this;
   }

   HTmatrix locsys3d::matrix(const pos3d& p) const
   {
      HTmatrix M; // identity matrix

      M(0,0) = m_xvec.x();
      M(1,0) = m_xvec.y();
      M(2,0) = m_xvec.z();

      M(0,1) = m_yvec.x();
      M(1,1) = m_yvec.y();
      M(2,1) = m_yvec.z();

      M(0,2) = m_zvec.x();
      M(1,2) = m_zvec.y();
      M(2,2) = m_zvec.z();

      M(0,2) = p.x();
      M(1,2) = p.y();
      M(2,2) = p.z();

      return std::move(M);
   }

}
