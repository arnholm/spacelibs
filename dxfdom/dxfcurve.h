#ifndef DXFCURVE_H
#define DXFCURVE_H

#include "dxfposmap.h"
#include <list>

// dxfcurve is an explicit or computed "curve" from a dxf entity
// it is not necessarily a smooth curve, it can alo be a polyline with corners or "bulges"
// The constructor takes all points on the curve, but only the internal ones are retained
// as a list of dxfpos, while the ends are kept as node references to the dxfposmap and later
// to the node table.

class DXFDOM_PUBLIC dxfcurve {
public:

   dxfcurve(dxfposmap<size_t>& pm, const std::list<dxfpos>& points);
   virtual ~dxfcurve();
   void set_id(size_t id) { m_id = id; }
   size_t id() const { return m_id; }

   // end node numbers
   size_t n1() const { return m_n1; }
   size_t n2() const { return m_n2; }

   size_t id_nodes() const { return m_n1 + 100000000*m_n2; }
   bool   is_closed() const { return (m_n1==m_n2); }

   // internal points, not including the end nodes
   std::list<dxfpos> internal_points() const { return m_p; }

private:
   size_t             m_id;  // curve sequence number

   size_t             m_n1;  // end1 node
   std::list<dxfpos>  m_p;   // internal curve points
   size_t             m_n2;  // end2 node
};

#endif // DXFCURVE_H
