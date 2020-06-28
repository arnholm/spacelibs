#ifndef DXFLOOP_OPTIMIZER_H
#define DXFLOOP_OPTIMIZER_H

#include <vector>
#include "spacemath/pos2d.h"
using namespace spacemath;

// dxfloop_optimizer is a stand alone optimizer intended for optimising points
// such as those returned from a dxfloop (which is always closed).
//
// The original vector of points is copied to a vector with fewer entries.
// Thus it works like a filter, removing some of the original points, adding no new points.
//
// Points are erased on the basis of the "arrow tolerance", i.e. for
// 3 consecutive points, p1 -- p2 -- p3 how far from the line p1-p3 is p2 allowed to be.

class dxfloop_optimizer {
public:

   dxfloop_optimizer(double arrow_max, double dist_max);
   virtual ~dxfloop_optimizer();

   // run the optimizer
   std::vector<pos2d> optimize(const std::vector<pos2d>& points) const;

   double arrow_max() const { return m_arrow_max; }
   double dist_max() const { return m_dist_max; }

private:
   double m_arrow_max;  // "arrow tolerance" - max arrow distance from p2 to projection p1 - p3
   double m_dist_max;   // "distance tolerance" - max distance between vertices along loop
};

#endif // DXFLOOP_OPTIMIZER_H
