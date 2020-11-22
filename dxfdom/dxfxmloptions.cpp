#include "dxfxmloptions.h"
#include <algorithm>

dxfxmloptions::dxfxmloptions(bool include_raw, double scale_factor, double sectol, double epspnt, const std::set<std::string>& layers, bool keep_case)
: m_include_raw(include_raw)
, m_scale_factor(scale_factor)
, m_sectol(sectol)
, m_epspnt(epspnt)
, m_keep_case(keep_case)
, m_auto_close(false)
{
   // make sure layer names are all uppercase here
   for(auto l : layers) {
      if(!m_keep_case) std::transform(l.begin(),l.end(),l.begin(),::toupper);
      m_layers.insert(l);
   }
}

dxfxmloptions::~dxfxmloptions()
{}

bool dxfxmloptions::layer_selected(const std::string& layer) const
{
   if(m_layers.size()==0)return true;

   // uppercase comparison
   std::string l(layer);
   if(!m_keep_case) std::transform(l.begin(),l.end(),l.begin(),::toupper);
   return (m_layers.find(l) != m_layers.end());
}
