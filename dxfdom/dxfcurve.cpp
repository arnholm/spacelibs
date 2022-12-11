#include "dxfcurve.h"

dxfcurve::dxfcurve(dxfposmap<size_t>& pm, const std::list<dxfpos>& points, const HTmatrix& T)
: m_p(transform_points(T,points))
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

dxfcurve::dxfcurve(size_t n1, size_t n2)
: m_id(0)
, m_n1(n1)
, m_n2(n2)
{}

dxfcurve::~dxfcurve()
{}

std::list<dxfpos> dxfcurve::transform_points(const HTmatrix& T, const std::list<dxfpos>& lp)
{
   std::list<dxfpos> lp_trans;

   for(const dxfpos& p : lp) {
      const vmath::mat4<double>& t = T.detail();
      vmath::vec3<double> r = vmath::transform_point(t,vmath::vec3<double>(p.x(),p.y(),p.z() ) );
      lp_trans.push_back(dxfpos(r[0],r[1],r[2]));
   }

   return lp_trans;
}
