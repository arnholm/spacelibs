#ifndef DXFLOOP_H
#define DXFLOOP_H

#include <list>
#include <memory>

#include "dxfdom_config.h"
#include "dxfpos.h"

// dxfloop is the final closed loop formed by chaining together dxfcurves

class DXFDOM_PUBLIC dxfloop {
public:
   typedef std::list<dxfpos> pos_list;
   typedef pos_list::const_iterator const_iterator;

   dxfloop();
   virtual ~dxfloop();


   void push_back(const dxfpos& p);
   void push_back(const std::list<dxfpos> & points);

   const_iterator begin() const { return m_points.begin(); }
   const_iterator end()   const { return m_points.end(); }
   size_t size() const { return m_points.size(); }

   // signed_area() returns a positive area for CCW loops
   double signed_area() const;

   // erase any duplicant positions from loop
   void erase_duplicates(double epspnt);

   // force the loop to be CCW winding order
   void canonicalize();

private:
   std::list<dxfpos>  m_points;
};

#endif // DXFLOOP_H
