#ifndef QUATERNION_H
#define QUATERNION_H

#include "spacemath_config.h"
#include "HTmatrix.h"

struct Quaternion;

namespace spacemath {

   class SPACEMATH_PUBLIC quaternion {
      public:

         // identity quaternion
         quaternion();

         // copy constructor
         quaternion(const quaternion& other);

         // construct from rotational part of Homogenous 4x4 transformation matrix M
         // note that any translational part in M is ignored (4th column)
         quaternion(const HTmatrix& M);

         // construct from scalar + 3 vector terms
         quaternion(double w, double v1, double v2, double v3);

         // destructor
         virtual ~quaternion();

         // normalise and return *this
         quaternion& normalize();

         // check for equality
         bool is_equal(const quaternion& other) const;

         // norm of quaterion
         double norm() const;

         // Interpolate between two quaternions using "slerp" algorithm
         // * p = interpolation parameter, range [0, 1], corresponding to [q1,q2]
         static quaternion interpolate(const quaternion& q1, const quaternion& q2, double p);

         // Convert to Homogenous 4x4 transformation matrix M
         // note that translational part will always be zero
         HTmatrix matrix() const;

      protected:
         void assign(double x, double y, double z, double w);

      private:
         Quaternion* m_quat;  // implementation detail
   };

}

#endif // QUATERNION_H
