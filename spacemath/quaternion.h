#ifndef QUATERNION_H
#define QUATERNION_H

#include "spacemath_config.h"
#include "HTmatrix.h"

struct Quaternion;

namespace spacemath {

   // basic quaternion with 4x4 homogeneous matrix compatibility
   class SPACEMATH_PUBLIC quaternion {
      public:

         // default construction => identity quaternion
         quaternion();

         // copy constructor
         quaternion(const quaternion& other);

         // assignment operator
         quaternion& operator=(const quaternion& other);

         // destructor
         virtual ~quaternion();

         // construct from rotational part of homogeneous 4x4 transformation matrix M
         // note that any translational part in M is ignored (4th column)
         quaternion(const HTmatrix& M);

         // construct from 3 vector terms + scalar
         quaternion(double x, double y, double z, double w);

         // return quaternion parameters
         double x() const;
         double y() const;
         double z() const;
         double w() const;

         // normalise and return *this
         quaternion& normalize();

         // check for equality
         bool is_equal(const quaternion& other) const;

         // norm of quaterion
         double norm() const;

         // Interpolate between two quaternions using "slerp" algorithm
         // p = interpolation parameter, range [0, 1], corresponding to [q1,q2]
         static quaternion interpolate(const quaternion& q1, const quaternion& q2, double p);

         // Convert to homogeneous 4x4 transformation matrix M
         // note that translational part will always be zero
         HTmatrix matrix() const;

      protected:
         void assign(double x, double y, double z, double w);

      private:
         Quaternion* m_quat;  // implementation detail
   };

}

#endif // QUATERNION_H
