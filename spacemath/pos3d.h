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

#ifndef POS3D_H
#define POS3D_H

#include "spacemath_config.h"

namespace spacemath {

   class vec3d;
   class HTmatrix;

   // position in 3d space
   class SPACEMATH_PUBLIC pos3d {
   public:
      pos3d();
      pos3d(double x,double y,double z);
      pos3d(const pos3d& pos);
      ~pos3d();

      const double& x() const;
      const double& y() const;
      const double& z() const;

      void x( double x );
      void y( double y );
      void z( double z );

      pos3d& move(const vec3d& vec);

   //Operators
      pos3d& operator =(const pos3d& pos);
      pos3d& operator+=(const vec3d& vec);
      pos3d& operator-=(const vec3d& vec);
      pos3d& operator*=(double value);
      pos3d& operator/=(double value);
      pos3d& operator+=(const pos3d& pos);
      pos3d& operator-=(const pos3d& pos);
      pos3d  operator-() const;

      bool   operator <(const pos3d& pos) const;
      double dist(const pos3d& pos) const;
      double dist_squared(const pos3d& pos) const;

      friend pos3d operator+(const pos3d& pos1, const pos3d& pos2);
      friend pos3d operator-(const pos3d& pos1, const pos3d& pos2);

      friend pos3d operator*(const pos3d& pos, double value);
      friend pos3d operator/(const pos3d& pos, double value);
      friend pos3d operator*(double value, const pos3d& pos);

      friend pos3d operator+(const pos3d& pos, const vec3d& vec);
      friend pos3d operator+(const vec3d& vec, const pos3d& pos);

      friend pos3d operator-(const pos3d& pos, const vec3d& vec);
      friend pos3d operator-(const vec3d& vec, const pos3d& pos);

      SPACEMATH_PUBLIC friend pos3d operator*(const HTmatrix& T, const pos3d& pos);

   private:
      double m_x;
      double m_y;
      double m_z;
   };


   //
   //inline definition of functions
   //

   //Constructions

   inline pos3d::pos3d(double x,double y,double z)
   : m_x(x),m_y(y),m_z(z)
   {}

   inline pos3d::pos3d()
   : m_x(0.0),m_y(0.0),m_z(0.0)
   {}

   inline pos3d::pos3d(const pos3d& pos)
   : m_x(pos.m_x),m_y(pos.m_y),m_z(pos.m_z)
   {}

   inline pos3d::~pos3d()
   {}

   //Operators

   inline pos3d& pos3d::operator*=(double value) {m_x*=value;m_y*=value;m_z*=value;return *this;}
   inline pos3d& pos3d::operator/=(double value) {m_x/=value;m_y/=value;m_z/=value;return *this;}
   inline pos3d& pos3d::operator+=(const pos3d& pos) {m_x+=pos.m_x;m_y+=pos.m_y;m_z+=pos.m_z;return *this;}
   inline pos3d& pos3d::operator-=(const pos3d& pos) {m_x-=pos.m_x;m_y-=pos.m_y;m_z-=pos.m_z;return *this;}

   inline pos3d pos3d::operator-() const {return pos3d(-m_x,-m_y,-m_z);}

   // Access

   inline const double& pos3d::x() const {return m_x;}
   inline const double& pos3d::y() const {return m_y;}
   inline const double& pos3d::z() const {return m_z;}

   inline void pos3d::x( double x ) {m_x=x;}
   inline void pos3d::y( double y ) {m_y=y;}
   inline void pos3d::z( double z ) {m_z=z;}

   // Operators

   inline pos3d operator+(const pos3d& pos1, const pos3d& pos2) {return pos3d(pos1)+=pos2;}
   inline pos3d operator-(const pos3d& pos1, const pos3d& pos2) {return pos3d(pos1)-=pos2;}
   inline pos3d operator*(const pos3d& pos, double value) {return pos3d(pos)*=value;}
   inline pos3d operator*(double value, const pos3d& pos) {return pos3d(pos)*=value;}
   inline pos3d operator/(const pos3d& pos, double value) {return pos3d(pos)/=value;}

   inline pos3d operator+(const pos3d& pos, const vec3d& vec) { return pos3d(pos)+=vec; }
   inline pos3d operator+(const vec3d& vec, const pos3d& pos) { return pos3d(pos)+=vec; }
   inline pos3d operator-(const pos3d& pos, const vec3d& vec) { return pos3d(pos)-=vec; }
   inline pos3d operator-(const vec3d& vec, const pos3d& pos) { return pos3d(-pos)+=vec; }

}

#endif
