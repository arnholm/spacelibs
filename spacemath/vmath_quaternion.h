#ifndef VMATH_QUATERNION_H
#define VMATH_QUATERNION_H

#include "spacemath_config.h"
#include "vmath/vector_math.h"
#include "HTmatrix.h"


template<typename T=double>
struct Quat;

namespace spacemath {

   // basic quaternion with 4x4 homogeneous matrix compatibility
   class SPACEMATH_PUBLIC vmath_quaternion {
      public:

         // default construction => identity vmath_quaternion
         vmath_quaternion();

         // copy constructor
         vmath_quaternion(const vmath_quaternion& other);

         // assignment operator
         vmath_quaternion& operator=(const vmath_quaternion& other);

         // destructor
         virtual ~vmath_quaternion();

         // construct from rotational part of homogeneous 4x4 transformation matrix M
         // note that any translational part in M is ignored (4th column)
         vmath_quaternion(const HTmatrix& M);

         // construct from 3 vector terms + scalar
         vmath_quaternion(double x, double y, double z, double w);

         // return vmath_quaternion parameters
         double x() const;
         double y() const;
         double z() const;
         double w() const;

         // normalise and return *this
         vmath_quaternion& normalize();

         // norm of quaterion
         double norm() const;

         // Interpolate between two vmath_quaternions using "slerp" algorithm
         // p = interpolation parameter, range [0, 1], corresponding to [q1,q2]
         static vmath_quaternion interpolate(const vmath_quaternion& q1, const vmath_quaternion& q2, double p);

         // Convert to homogeneous 4x4 transformation matrix M
         // note that translational part will always be zero
         HTmatrix matrix() const;

      private:
         vmath::quat<double> m_quat;  // implementation detail
   };

}

#endif // VMATH_QUATERNION_H
