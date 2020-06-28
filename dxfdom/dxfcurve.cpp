#include "dxfcurve.h"


dxfcurve::dxfcurve(dxfposmap<size_t>& pm, const std::list<dxfpos>& points)
: m_p(points)
, m_id(0)
{
   auto i1 = pm.find(m_p.front());
   if(i1 == pm.end()) m_n1 = pm.insert(std::make_pair(m_p.front(),pm.size()+1))->second.second;
   else               m_n1 = i1->second.second;

   auto i2 = pm.find(m_p.back());
   if(i2 == pm.end()) m_n2 = pm.insert(std::make_pair(m_p.back(),pm.size()+1))->second.second;
   else               m_n2 = i2->second.second;

   m_p.pop_front();
   m_p.pop_back();
}

dxfcurve::~dxfcurve()
{}

