#include "vmath_quaternion.h"
#include <utility>
#include "maths/quat.h"

namespace spacemath {

   vmath_quaternion::vmath_quaternion()
   : m_quat(vmath::identityq<double>())
   { }

   vmath_quaternion::vmath_quaternion(const HTmatrix& M)
   : m_quat(vmath::mat_to_quat(M.detail()))
   { }

   vmath_quaternion::vmath_quaternion(const vmath_quaternion& other)
   : m_quat(other.m_quat)
   { }

   vmath_quaternion& vmath_quaternion::operator=(const vmath_quaternion& other)
   {
      m_quat = other.m_quat;
      return *this;
   }

   vmath_quaternion::vmath_quaternion(double x, double y, double z, double w)
   : m_quat(x,y,z,w)
   { }

   vmath_quaternion::~vmath_quaternion()
   { }

   // ==========

   double vmath_quaternion::x() const { return m_quat.v[0]; }
   double vmath_quaternion::y() const { return m_quat.v[1]; }
   double vmath_quaternion::z() const { return m_quat.v[2]; }
   double vmath_quaternion::w() const { return m_quat.w; }

   vmath_quaternion& vmath_quaternion::normalize()
   {
      m_quat = vmath::normalize(m_quat);
      return *this;
   }

   double vmath_quaternion::norm() const
   {
      const double& x = m_quat.v[0];
      const double& y = m_quat.v[1];
      const double& z = m_quat.v[2];
      const double& w = m_quat.w;
      return sqrt(x*x + y*y + z*z + w*w);
   }


   static vmath::quat<double> slerp_internal(const vmath::quat<double>& v0, const vmath::quat<double>& v1, const double& t)
   {
     // Only unit vmath_quaternions are valid rotations.
     // Normalize to avoid undefined behavior.
     vmath::quat<double> q0(v0); vmath::normalize(q0);
     vmath::quat<double> q1(v1); vmath::normalize(q1);

     const double one = 1.0 - std::numeric_limits<double>::epsilon();
     double d = vmath::dot(q0,q1);
     double absD = abs(d);

     double scale0;
     double scale1;

     // When the dot product of the vmath_quaternion vectors are within machine epsilon of 1,
     // The two vmath_quaternions are almost representing the same spatial rotation. Hence
     // use linear interpolation
     if(absD>=one)
     {
       scale0 = 1.0 - t;
       scale1 = t;
     }
     else
     {
       // theta is the angle between the 2 vmath_quaternions
       double theta = acos(absD);
       double sinTheta = sin(theta);

       scale0 = sin((1.0 - t) * theta) / sinTheta;
       scale1 = sin((t * theta)) / sinTheta;
     }

     // If the dot product is negative, slerp won't take
     // the shorter path. Correct by negating the scale
     if(d<0.0) scale1 = -scale1;

     double w = scale0 * q0.w    + scale1 * q1.w;
     double x = scale0 * q0.v[0] + scale1 * q1.v[0];
     double y = scale0 * q0.v[1] + scale1 * q1.v[1];
     double z = scale0 * q0.v[2] + scale1 * q1.v[2];

     return vmath::quat<double>(x,y,z,w);
   }


   vmath_quaternion vmath_quaternion::interpolate(const vmath_quaternion& q1, const vmath_quaternion& q2, double p)
   {
      vmath_quaternion res;
      res.m_quat = slerp_internal(q1.m_quat,q2.m_quat,p);
      return std::move(res);
   }


   HTmatrix vmath_quaternion::matrix() const
   {
      // http://www.mrelusive.com/publications/papers/SIMD-From-Quat<double>-to-Matrix-and-Back.pdf
      // https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf

      double w = m_quat.w;
      double x = m_quat.v[0];
      double y = m_quat.v[1];
      double z = m_quat.v[2];

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
