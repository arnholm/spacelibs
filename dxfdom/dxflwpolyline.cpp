#include "dxflwpolyline.h"
#include <bitset>

dxflwpolyline::dxflwpolyline(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
, m_flag(0)
, m_normal(0,0,1)
{
   if(item->value() != "LWPOLYLINE") throw logic_error("dxflwpolyline, expected 'LWPOLYLINE' but got " + item->value());

   map<int,double> vtx;

   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->gc() != 0) {

      // always push the child object, regardless of contents
      push_back(make_shared<dxfobject>(child,opt));

      // filter out what we are interested in
      int gc = child->gc();
      switch(gc) {
         case 5:  { set_handle(child->value()); break; }
         case 8:  { set_layer(child->value()); break; }
         case 10:
         case 20: { vtx[gc] = opt.scale_factor()*child->dvalue(); break; }
         case 70: { m_flag  = child->ivalue(); break; }

         case 210: { m_normal.set_x(child->dvalue()); break; }
         case 220: { m_normal.set_y(child->dvalue()); break; }
         case 230: { m_normal.set_z(child->dvalue()); break; }

         default: {}
      };

      if(vtx.size() == 2) {
         m_points.push_back(dxfpos(vtx[10],vtx[20],0.0));
         vtx.clear();
      }

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);
/*
   cout << "dxflwpolyline" << ' ' << item->gc() << " '" << item->value() << "' "
        << " np=" << m_points.size() << endl;

   for(auto& p : m_points) {
      cout << " p=(" << p.x() << ',' <<  p.y() << ',' <<  p.z() << ')' << endl;
   }
*/
}

dxflwpolyline::~dxflwpolyline()
{}

bool dxflwpolyline::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("flag",m_flag);
      to_xml_xyz(xml_this,"vxyz",m_normal);
      for(auto& p : m_points) {
         to_xml_xyz(xml_this,"pxyz",p);
      }
   }

   return retval;
}

void dxflwpolyline::push_profile(dxfprofile& prof) const
{
   if(m_points.size() > 0) {

      std::bitset<32> bits(m_flag);
      bool closed      =  bits[0];
      if(closed) {
         list<dxfpos> points(m_points);
         points.push_back(m_points.front());
         prof.push_back(std::make_shared<dxfcurve>(prof.pm(),points));
      }
      else {
         prof.push_back(std::make_shared<dxfcurve>(prof.pm(),m_points));
      }
   }
}
