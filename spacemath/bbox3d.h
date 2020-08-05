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

#ifndef BBOX3D_H
#define BBOX3D_H

#include "spacemath_config.h"
#include "pos3d.h"

namespace spacemath {
   class HTmatrix;

   // bbox3d - 3d bounding box
   class SPACEMATH_PUBLIC bbox3d {
   public:
      bbox3d(bool initialised = false);
      bbox3d(const pos3d& pos1, const pos3d& pos2);
      virtual ~bbox3d();

      bool initialised() const;
      bbox3d& operator =(const bbox3d& box);

      void  enclose(const pos3d& pos1, double tolerance=0.0);

      const pos3d& p1() const;
      const pos3d& p2() const;

      // return center of bounding box
      pos3d center() const ;

      // return extent in x
      double dx() const;

      // return extent in y
      double dy() const;

      // return extent in z
      double dz() const;

      // return diagonal distance, i.e. distance between p1 and p2
      double diagonal() const;

      // returns true if p is enclosed in bounding box
      bool  is_enclosed( const pos3d& p) const;

      // returns intersection box between this box and 'b'.
      // For empty intersection, initialised() will return false on the returned box
      bbox3d intersection(const bbox3d& b) const;

      // returns difference box between this box and box 'b'
      // For empty difference, initialised() will return false on the returned box
      bbox3d difference(const bbox3d& b, double tolerance=1E-6) const;

      // transforms the bounding box. Note that the result is still xyz aligned
      SPACEMATH_PUBLIC friend bbox3d operator*(const HTmatrix& T, const bbox3d& b);

   private:
      void setBox(const pos3d& pos1, const pos3d& pos2);

   private:
      pos3d  m_p1;
      pos3d  m_p2;
      bool   m_initialised;
   };

}

#endif // BBOX3D_H
