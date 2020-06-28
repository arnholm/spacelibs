#include "dxfline.h"

dxfline::dxfline(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
{

   if(item->value() != "LINE") throw logic_error("dxflwpolyline, expected 'LINE' but got " + item->value());

   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->gc() != 0) {

      // always push the child object, regardless of contents
      push_back(make_shared<dxfobject>(child,opt));

      // filter out what we are interested in
      int gc = child->gc();
      switch(gc) {

         case 5:  { set_handle(child->value()); break; }
         case 8:  { set_layer(child->value()); break; }

         case 10: { m_p1.set_x(opt.scale_factor()*child->dvalue()); break; }
         case 20: { m_p1.set_y(opt.scale_factor()*child->dvalue()); break; }
         case 30: { m_p1.set_z(opt.scale_factor()*child->dvalue()); break; }

         case 11: { m_p2.set_x(opt.scale_factor()*child->dvalue()); break; }
         case 21: { m_p2.set_y(opt.scale_factor()*child->dvalue()); break; }
         case 31: { m_p2.set_z(opt.scale_factor()*child->dvalue()); break; }
         default: {}
      };

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);

/*
   cout << "dxfline" << ' ' << item->gc() << " '" << item->value() << "' " ;
   cout << " size = " << size()
        << " p1=(" << m_p1.x() << ',' <<  m_p1.y() << ',' <<  m_p1.z() << ')'
        << " p2=(" << m_p2.x() << ',' <<  m_p2.y() << ',' <<  m_p2.z() << ')'
        << endl;
*/
}

dxfline::~dxfline()
{}

bool dxfline::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      to_xml_xyz(xml_this,"pxyz1",m_p1);
      to_xml_xyz(xml_this,"pxyz2",m_p2);
   }

   return retval;
}

void dxfline::push_profile(dxfprofile& prof) const
{
   // skip zero length lines
   if(m_p1.dist(m_p2) > 0.0) {
      std::list<dxfpos> points = { m_p1, m_p2 };
      prof.push_back(std::make_shared<dxfcurve>(prof.pm(),points));
   }
}
