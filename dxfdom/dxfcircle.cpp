#include "dxfcircle.h"

static const double pi = 4.0*atan(1.0);

dxfcircle::dxfcircle(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
, m_r(-1.0)
, m_normal(0,0,1)
{
   if(item->value() != "CIRCLE") throw logic_error("dxfcircle, expected 'CIRCLE' but got " + item->value());

   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->gc() != 0) {

      // always push the child object, regardless of contents
      push_back(make_shared<dxfobject>(child,opt));

      // filter out what we are interested in
      int gc = child->gc();
      switch(gc) {

         case 5:  { set_handle(child->value()); break; }
         case 8:  { set_layer(child->value()); break; }

         case 10: { m_pc.set_x(opt.scale_factor()*child->dvalue()); break; }
         case 20: { m_pc.set_y(opt.scale_factor()*child->dvalue()); break; }
         case 30: { m_pc.set_z(opt.scale_factor()*child->dvalue()); break; }
         case 40: { m_r  = opt.scale_factor()*child->dvalue(); break; }

         case 210: { m_normal.set_x(child->dvalue()); break; }
         case 220: { m_normal.set_y(child->dvalue()); break; }
         case 230: { m_normal.set_z(child->dvalue()); break; }

         default: {}
      };

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);
/*
   cout << "dxfcircle" << ' ' << item->gc() << " '" << item->value() << "' "
        << " pc=(" << m_pc.x() << ',' <<  m_pc.y() << ',' <<  m_pc.z() << ')'
        << " r=" << m_rad
        << " n=(" << m_normal.x() << ',' <<  m_normal.y() << ',' <<  m_normal.z() << ')'
        << endl;
*/
}

dxfcircle::~dxfcircle()
{
}


bool dxfcircle::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("radius",m_r);
      to_xml_xyz(xml_this,"pxyz",m_pc);
      to_xml_xyz(xml_this,"vxyz",m_normal);
   }

   return retval;
}

void dxfcircle::push_profile(dxfprofile& prof) const
{
   if(m_r>0.0) {
      double sectol = prof.sectol();
      size_t nseg = 4;
      double alpha = 2.0*pi/nseg;
      while(m_r*(1.0-cos(0.5*alpha)) >  sectol) {
         nseg += 2;
         alpha = 2*pi/nseg;
      }
      double dang = 2*pi/nseg;

      double x0 = m_pc.x();
      double y0 = m_pc.y();

      std::list<dxfpos> points;
      for(size_t iseg=0; iseg<nseg+1; iseg++) {
         double ang = iseg*dang;
         double x   = x0 + m_r*cos(ang);
         double y   = y0 + m_r*sin(ang);
         points.push_back(dxfpos(x,y,0));
      }

      prof.push_back(std::make_shared<dxfcurve>(prof.pm(),points));
   }
}
