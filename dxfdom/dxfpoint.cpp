#include "dxfpoint.h"

dxfpoint::dxfpoint(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
{
   if(item->value() != "POINT") throw logic_error("dxfpoint, expected 'POINT' but got " + item->value());

   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->gc() != 0) {

      // always push the child object, regardless of contents
      push_back(make_shared<dxfobject>(child,opt));

      // filter out what we are interested in
      int gc = child->gc();
      switch(gc) {

         case 5:  { set_handle(child->value()); break; }
         case 8:  { set_layer(child->value()); break; }

         case 10: { m_p.set_x(opt.scale_factor()*child->dvalue()); break; }
         case 20: { m_p.set_y(opt.scale_factor()*child->dvalue()); break; }
         case 30: { m_p.set_z(opt.scale_factor()*child->dvalue()); break; }

         default: {}
      };

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);

   /*
   cout << "dxfpoint" << ' ' << item->gc() << " '" << item->value() << "' "
        << " p=(" << m_p.x() << ',' <<  m_p.y() << ',' <<  m_p.z() << ')'
        << endl;
   */
}

dxfpoint::~dxfpoint()
{
}

bool dxfpoint::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      to_xml_xyz(xml_this,"pxyz",m_p);
   }

   return retval;
}

void dxfpoint::push_profile(dxfprofile& prof) const
{
  // prof.push_back(m_p);
}
