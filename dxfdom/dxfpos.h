#ifndef DFXPOS_H
#define DFXPOS_H

#include <cmath>
#include "dxfdom_config.h"

class DXFDOM_PUBLIC dxfpos {
public:
   dxfpos();
   dxfpos(double x, double y, double z);
   virtual ~dxfpos();

   void set_x(double x) { m_x = x; }
   void set_y(double y) { m_y = y; }
   void set_z(double z) { m_z = z; }

   double x() const { return m_x; }
   double y() const { return m_y; }
   double z() const { return m_z; }

   double dist(const dxfpos& p) const
      {
         double dx = m_x - p.m_x;
         double dy = m_y - p.m_y;
         double dz = m_z - p.m_z;
         return sqrt(dx*dx+dy*dy+dz*dz);
      }

   void scale(double factor)
      {
         m_x *= factor;
         m_y *= factor;
         m_z *= factor;
      }

   double length() const
      {
         return sqrt(m_x*m_x+m_y*m_y+m_z*m_z);
      }

protected:
private:
   double m_x;
   double m_y;
   double m_z;
};

#endif // DFXPOS_H
