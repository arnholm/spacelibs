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


#ifndef SPACEMATH_EIGEN_QUATERNION_H
#define SPACEMATH_EIGEN_QUATERNION_H

#ifdef Success
#undef Success
#endif

// This is a header only implementation of quaternion for spacemath based on Eigen
// Using it requires Eigen  https://eigen.tuxfamily.org/

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <limits>

// spacemath::quaternion is compatible with spacemath::HTmatrix
#include "HTmatrix.h"

namespace spacemath {

   // Eigen matrices used
   using Matrix4x4 = Eigen::Matrix<double,4,4>;
   using Matrix3x3 = Eigen::Matrix<double,3,3>;

   // quaternions is used for interpolating 3d rotations
   class quaternion {
   public:
      // default construction => identity quaternion
      quaternion();

      // construct from basic quaternion parameters
      quaternion(double w, double x, double y, double z);

      // construct from rotation matrix
      quaternion(const Matrix3x3& rot, bool normalize=true);

      // construct from Homogeneous matrix (discarding column 4)
      quaternion(const Matrix4x4& Tm, bool normalize=true);

      // construct from spacemath Homogeneous matrix (discarding column 4)
      quaternion(const HTmatrix& Tm, bool normalize=true);

      virtual ~quaternion();

      // normalize this quaternion
      quaternion& normalize();

      // norm of quaternion
      double norm() const;

      // return quaternion parameters
      double x() const;
      double y() const;
      double z() const;
      double w() const;

      // return 3x3 rotation matrix
      Matrix3x3 to_rotation_matrix() const;

      // Convert to homogeneous 4x4 transformation matrix M
      // note that translational part will always be zero
      Matrix4x4 to_homogeneous_matrix() const;

      // Convert to homogeneous 4x4 transformation matrix M
      // note that translational part will always be zero
      HTmatrix to_homogeneous_HTmatrix() const;

      // compute difference from qa to qb. qdiff = qa(-1)*qb
      static quaternion difference(const quaternion& qa, const quaternion& qb);

      // Interpolate between two quaternions using "slerp" algorithm
      // p = interpolation parameter, range [0, 1], corresponding to [q1,q2]
      static quaternion interpolate(const quaternion& q1, const quaternion& q2, double p);

      // Interpolate between two quaternions using modified "slerp" algorithm from Wikipedia
      // https://en.wikipedia.org/wiki/Slerp
      // p = interpolation parameter, range [0, 1], corresponding to [q1,q2]
      static quaternion slerp_wiki(const quaternion& q1, const quaternion& q2, double p);

   protected:
      quaternion(const Eigen::Quaterniond& quat, bool normalize=true);

   private:
      Eigen::Quaterniond m_quat;
   };

   inline quaternion::quaternion(const Eigen::Quaterniond& quat, bool normalize)
   : m_quat(quat)
   {
      if(normalize)m_quat.normalize();
   }

   inline quaternion::quaternion(double w, double x, double y, double z)
   : m_quat(w,x,y,z)
   {}

   inline quaternion::quaternion()
   : m_quat(1.0,0.0,0.0,0.0)
   {}

   inline quaternion::quaternion(const Matrix3x3& rot, bool normalize)
   : m_quat(rot)
   {
      if(normalize)m_quat.normalize();
   }

   inline quaternion::quaternion(const Matrix4x4& Tm, bool normalize)
   : m_quat((const Matrix3x3&) Tm.block(0,0,3,3))
   {
      if(normalize)m_quat.normalize();
   }

   inline quaternion::quaternion(const HTmatrix& Tm, bool normalize)
   : m_quat(1.0,0.0,0.0,0.0)
   {
      Matrix3x3 rot;
      for(size_t i=0; i<3; i++) {
         for(size_t j=0; j<3; j++) {
            rot(i,j) = Tm(i,j);
         }
      }
      m_quat = rot;

      if(normalize)m_quat.normalize();
   }


   inline quaternion::~quaternion()
   {}

   inline double quaternion::x() const { return m_quat.x(); }
   inline double quaternion::y() const { return m_quat.y(); }
   inline double quaternion::z() const { return m_quat.z(); }
   inline double quaternion::w() const { return m_quat.w(); }

   inline Matrix3x3 quaternion::to_rotation_matrix () const
   {
      return m_quat.toRotationMatrix();
   }

   inline Matrix4x4  quaternion::to_homogeneous_matrix() const
   {
      Matrix4x4 m;
      m.setIdentity();
      m.block(0,0,3,3) = m_quat.toRotationMatrix();
      return std::move(m);
   }

   inline HTmatrix quaternion::to_homogeneous_HTmatrix() const
   {
      Matrix3x3 rot = m_quat.toRotationMatrix();
      HTmatrix m;
      for(size_t i=0; i<3; i++) {
         for(size_t j=0; j<3; j++) {
            m(i,j) = rot(i,j);
         }
      }
      return std::move(m);
   }

   inline quaternion& quaternion::normalize()
   {
      m_quat.normalize();
      return *this;
   }

   inline double quaternion::norm() const
   {
      return m_quat.norm();
   }

   inline quaternion quaternion::difference(const quaternion& qa, const quaternion& qb)
   {
      // ref https://www.youtube.com/watch?v=5NY3fP_0GVA

      // qa * qTransition = qb
      //  ==>   qa(-1) * qa * qTransition = qa(-1)* qb
      //  ==>   qTransition = qa(-1)* qb

      // Quaternion difference is the angular displacement from one orientation to another
      const Eigen::Quaterniond& quat_a = qa.m_quat;
      const Eigen::Quaterniond& quat_b = qb.m_quat;
      return quaternion(quat_a.inverse()*quat_b);
   }

   inline quaternion quaternion::interpolate(const quaternion& q1, const quaternion& q2, double p)
   {
      quaternion res = q1.m_quat.slerp(p,q2.m_quat);
      return std::move(res);
   }

}

#endif // SPACEMATH_EIGEN_QUATERNION_H
