#include "dxfentity.h"

dxfentity::dxfentity(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfobject(item,opt)
{}

dxfentity::~dxfentity()
{}

bool dxfentity::to_xml(xml_node& xml_this) const
{
   // add layer only if set
   if(m_layer.length()>0)xml_this.add_property("layer",m_layer);

   return dxfobject::to_xml(xml_this);
}

dxfpos dxfentity::to_wcs(const dxfpos& normal, const dxfpos& pos) const
{
    if(normal.z() > 0.0) return pos;
    else                 return dxfpos(-pos.x(),pos.y(),0.0);
}

dxfpos dxfentity::transform_pos(const HTmatrix& T, const dxfpos& p) const
{
   const vmath::mat4<double>& t = T.detail();
   vmath::vec3<double> r=transform_point(t,vmath::vec3<double>(p.x(),p.y(),p.z() ) );
   return dxfpos(r[0],r[1],r[2]);
}
