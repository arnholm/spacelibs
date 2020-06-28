#ifndef DXFNODE_H
#define DXFNODE_H

#include "dxfdom_config.h"
#include "dxfpos.h"
#include <cstddef>
#include <set>
#include <memory>
class dxfcurve;

// dfxnode is used as topology when building profile contours

class DXFDOM_PUBLIC dxfnode {
public:
   dxfnode(const dxfpos& pos);
   virtual ~dxfnode();

   // return node position
   const dxfpos& pos() const { return m_pos; }

   // insert and erase curve in this node
   void insert(std::shared_ptr<dxfcurve> c) { m_curves.insert(c); }
   void erase(std::shared_ptr<dxfcurve> c) { m_curves.erase(c); }

   // get other curve in 2-manifold node
   std::shared_ptr<dxfcurve> next_curve(std::shared_ptr<dxfcurve> c) const;

   // manifold = number of curves referencing node
   size_t manifold() const { return m_curves.size(); }

private:
   dxfpos              m_pos;     // coordinates of node
   std::set<std::shared_ptr<dxfcurve>> m_curves;  // curves referencing this node
};

#endif // DXFNODE_H
