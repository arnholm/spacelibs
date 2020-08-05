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

#ifndef HTMATRIX_H
#define HTMATRIX_H

#include "vmath/vector_math.h"
#include <algorithm>

namespace spacemath {

   // HTmatrix is a 4x4 homogenous transformation matrix (can be used for other purposes as well)
   // it is primarily designed for convenient notation as a wrapper around vmath

   //     X'   =  T * X
   //
   //   | x' | = | T(0,0) T(1,0) T(2,0) T(3,0) | * | x |
   //   | y' |   | T(0,1) T(1,1) T(2,1) T(3,1) |   | y |
   //   | z' |   | T(0,2) T(1,2) T(2,2) T(3,2) |   | z |
   //   | 1  |   |   0      0      0      1    |   | 1 |


   class HTmatrix {
   public:

      // default constructor creates identity matrix
      HTmatrix() : m_vmat(vmath::identity4<double>())              { }

      // construct from 4 vmath column vectors
      HTmatrix(const vmath::vec4<double>& v0, vmath::vec4<double>& v1, vmath::vec4<double>& v2, vmath::vec4<double>& v3)  : m_vmat(v0,v1,v2,v3) {}

      // move constructor
      HTmatrix(HTmatrix&& other) : m_vmat(std::move(other.m_vmat)) { }

      // copy constructor
      HTmatrix(const HTmatrix& other) : m_vmat(other.m_vmat)       { }

      // assignment operator
      HTmatrix& operator=(const HTmatrix& other)                   { m_vmat = other.m_vmat; return *this; }

      // destructor
      virtual ~HTmatrix()                                          { }

      // write access to the matrix elements using (i,j) notation
      inline double& operator()(size_t i, size_t j)                { return m_vmat.elem[i][j]; }

      // read access to the matrix elements using (i,j) notation
      inline const double& operator()(size_t i, size_t j) const    { return m_vmat.elem[i][j]; }

      // operators
      // =========

      // accumulating matrix post-multiplication T *= t
      inline HTmatrix& operator*=(const HTmatrix& t)               { m_vmat *= t.m_vmat; return *this; }

      // matrix post-multiplication: HTmatrix T1 = T*t
      inline HTmatrix operator*(const HTmatrix& t) const           { vmath::mat4<double> tmp(m_vmat); tmp *= t.m_vmat; return tmp; }

      // general matrix-vector multiplication v' = T*v
      inline vmath::vec4<double> operator*(const vmath::vec4<double>& v) const { return m_vmat*v; }

      // friend functions
      // ================

      // fast inverse matrix, assuming the matrix is a homogenous transfarmation matrix
      inline friend HTmatrix fast_inverse(const HTmatrix& m)       { return vmath::fast_inverse(m.m_vmat); }

      // inverse general 4x4 matrix
      inline friend HTmatrix inverse(const HTmatrix& m)            { return vmath::inverse(m.m_vmat); }

      // transposed matrix
      inline friend HTmatrix transpose(const HTmatrix& m)          { return vmath::transpose(m.m_vmat); }

      //  matrix determinant
      inline friend double determinant(const HTmatrix& m)          { return vmath::det(m.m_vmat); }

      // special
      // =======

      // access the matrix implementation detail
      const vmath::mat4<double>& detail() const { return m_vmat; }

      // direct access to the raw matrix data elements, as a row major order array of 4x4 elements. Use with care....
      double* raw() { return &m_vmat.elem[0][0]; }

   protected:
      // construct from native data
      HTmatrix(const vmath::mat4<double>& m) : m_vmat(m)           { }

   private:
      vmath::mat4<double> m_vmat;
   };

}

#endif // HTMATRIX_H
