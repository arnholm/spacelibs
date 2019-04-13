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

#ifndef VEC3D_H
#define VEC3D_H

#include "spacemath_config.h"

namespace spacemath {

   class pos3d;
   class HTmatrix;

   // vector in 3d space
   class SPACEMATH_PUBLIC vec3d {
   public:
      vec3d();
      vec3d(double x,double y,double z);
      vec3d(const pos3d& start,const pos3d& end);
      vec3d(const vec3d& vec);
     ~vec3d();

      double x() const;
      double y() const;
      double z() const;

      //operators
      vec3d& operator =(const vec3d& vec);
      vec3d  operator -() const;

      // Check if vectors are parallell within given angle tolerance
      bool isParallel(const vec3d& vec, double angle_tol) const;

      // vector arithmetic
      vec3d& operator+=(const vec3d& vec);
      vec3d& operator-=(const vec3d& vec);

      vec3d& operator*=(double factor);       //Scaling

      // in-place manipulation of vector
      vec3d& normalise();
      vec3d& scale(double factor);
      vec3d& add(const vec3d& vec2);
      vec3d& subtract(const vec3d& vec2);
      vec3d& reverse();

      // cross product
      vec3d  cross(const vec3d& vec2) const;

      // dot product
      double dot(const vec3d& vec2) const;

      // angle between vectors
      double angle(const vec3d& vec2) const;

      // length of vector
      double length() const;

      // square length of vector
      double squareLength() const;

      //Cross product
      friend vec3d operator*(const vec3d& vec1,const vec3d& vec2);

      //Vector sum
      friend vec3d operator+(const vec3d& vec1,const vec3d& vec2);
      friend vec3d operator-(const vec3d& vec1,const vec3d& vec2);

      //Scalar product
      friend double operator%(const vec3d& vec1,const vec3d& vec2);

      //Scaling
      friend vec3d operator*(const vec3d& vec,double factor);
      friend vec3d operator*(double factor,const vec3d& vec);

      // normalisation of vector
      friend vec3d normalise(const vec3d& vec);

      // vector transformation
      SPACEMATH_PUBLIC friend vec3d operator*(const HTmatrix& T, const vec3d& vec);

   private:
      double m_x;
      double m_y;
      double m_z;
   };

   //Construction from 3 components
   inline vec3d::vec3d(double x,double y,double z)
   : m_x(x),m_y(y),m_z(z)
   {}

   inline vec3d::vec3d()
   : m_x(0.0),m_y(0.0),m_z(0.0)
   {}

   inline vec3d::vec3d(const vec3d& vec)
   : m_x(vec.m_x),m_y(vec.m_y),m_z(vec.m_z)
   {}

   inline vec3d::~vec3d()
   {}

   // read access to components
   inline double vec3d::x() const {return m_x;}
   inline double vec3d::y() const {return m_y;}
   inline double vec3d::z() const {return m_z;}

   // vector arithmetic
   inline vec3d& vec3d::operator+=(const vec3d& vec) {return add(vec);}
   inline vec3d& vec3d::operator-=(const vec3d& vec) {return subtract(vec);}
   inline vec3d& vec3d::operator*=(double factor) {return scale(factor);}
   inline vec3d  vec3d::operator-() const {return vec3d(-m_x,-m_y,-m_z);}

   // vector arithmetic using friend operators
   inline vec3d operator*(const vec3d& vec1,const vec3d& vec2) {return vec1.cross(vec2);}
   inline vec3d operator+(const vec3d& vec1,const vec3d& vec2) {return vec3d(vec1)+=vec2;}
   inline vec3d operator-(const vec3d& vec1,const vec3d& vec2) {return vec3d(vec1)-=vec2;}
   inline vec3d operator*(const vec3d& vec,double factor) {return vec3d(vec)*=factor;}
   inline vec3d operator*(double factor,const vec3d& vec) {return vec3d(vec)*=factor;}
   inline vec3d normalise(const vec3d& vec) {return vec3d(vec).normalise();}

}

#endif

