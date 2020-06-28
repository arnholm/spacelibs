#include "dxfnode.h"
#include <stdexcept>

dxfnode::dxfnode(const dxfpos& pos)
: m_pos(pos)
{}

dxfnode::~dxfnode()
{}

std::shared_ptr<dxfcurve> dxfnode::next_curve(std::shared_ptr<dxfcurve> c) const
{
   std::shared_ptr<dxfcurve> next = 0;

   if(m_curves.size() != 2) throw std::logic_error("node is not 2-manifold");

   for(auto& nc : m_curves) {
      if(nc != c) {
         next = nc;
         break;
      }
   }

   if(!next) throw std::logic_error("dxfnode::next_curve, given curve is not referenced from  this node");

   return next;
}
