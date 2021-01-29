#include "quaternion.h"
#include <utility>

extern "C" {
#include "quaternion_c/Quaternion.h"
}

namespace spacemath {

   quaternion::quaternion()
   : m_quat(new Quaternion)
   {
      Quaternion_setIdentity(m_quat);
   }

   void quaternion::assign(double x, double y, double z, double w)
   {
      Quaternion_set(w,x,y,z,m_quat);
   }

   quaternion::quaternion(const HTmatrix& M)
   : m_quat(new Quaternion)
   {
      // this implementation is based on final page from
      // https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf

      double m00 = M(0,0);
      double m10 = M(1,0);
      double m20 = M(2,0);

      double m01 = M(0,1);
      double m11 = M(1,1);
      double m21 = M(2,1);

      double m02 = M(0,2);
      double m12 = M(1,2);
      double m22 = M(2,2);

      double t   = 0.0;

      if(m22 < 0) {
         if(m00 > m11) {
            t = 1.0 + m00 - m11 - m22;
            assign( t, m01+m10, m20+m02, m12-m21);
         }
         else {
            t = 1.0 - m00 + m11 - m22;
            assign( m01+m10, t, m12+m21, m20-m02);
         }
      }
      else {
         if(m00 < -m11) {
            t = 1.0 - m00 - m11 + m22;
            assign( m20+m02, m12+m21, t, m01-m10 );
         }
         else {
            t = 1.0 + m00 + m11 + m22;
            assign( m12-m21, m20-m02, m01-m10, t );
         }
      }
      double factor = 0.5 / sqrt(t);

      // scale the result
      m_quat->w    *= factor;
      m_quat->v[0] *= factor;
      m_quat->v[1] *= factor;
      m_quat->v[2] *= factor;
   }

   quaternion::quaternion(const quaternion& other)
   : m_quat(new Quaternion)
   {
      Quaternion_copy(other.m_quat,m_quat);
   }

   quaternion& quaternion::operator=(const quaternion& other)
   {
      Quaternion_copy(other.m_quat,m_quat);
      return *this;
   }

   quaternion::quaternion(double x, double y, double z, double w)
   : m_quat(new Quaternion)
   {
      Quaternion_set(w,x,y,z,m_quat);
   }

   quaternion::~quaternion()
   {
      delete m_quat;
   }

   // ==========

   double quaternion::x() const { return m_quat->v[0]; }
   double quaternion::y() const { return m_quat->v[1]; }
   double quaternion::z() const { return m_quat->v[2]; }
   double quaternion::w() const { return m_quat->w; }

   quaternion& quaternion::normalize()
   {
      Quaternion_normalize(m_quat,m_quat);
      return *this;
   }

   bool quaternion::is_equal(const quaternion& other) const
   {
      return Quaternion_equal(m_quat,other.m_quat);
   }

   double quaternion::norm() const
   {
      return Quaternion_norm(m_quat);
   }

   quaternion quaternion::interpolate(const quaternion& q1, const quaternion& q2, double p)
   {
      quaternion res;
      Quaternion_slerp(q1.m_quat,q2.m_quat,p,res.m_quat);
      return std::move(res);
   }


   HTmatrix quaternion::matrix() const
   {
      // http://www.mrelusive.com/publications/papers/SIMD-From-Quaternion-to-Matrix-and-Back.pdf
      // https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf

      double w = m_quat->w;
      double x = m_quat->v[0];
      double y = m_quat->v[1];
      double z = m_quat->v[2];

      vmath::vec4<double> v0,v1,v2,v3;

      v0[0] = 1.0 - 2*y*y - 2*z*z;
      v0[1] = 2*x*y - 2*w*z;
      v0[2] = 2*x*z + 2*w*y;
      v0[3] = 0.0;

      v1[0] = 2*x*y + 2*w*z;
      v1[1] = 1.0 - 2*x*x - 2*z*z;
      v1[2] = 2*y*z - 2*w*x;
      v1[3] = 0.0;

      v2[0] = 2*x*z - 2*w*y;
      v2[1] = 2*y*z + 2*w*x;
      v2[2] = 1.0 - 2*x*x - 2*y*y;
      v2[3] = 0.0;

      v3[0] = 0.0;
      v3[1] = 0.0;
      v3[2] = 0.0;
      v3[3] = 1.0;

      return HTmatrix(v0,v1,v2,v3);
   }
}
