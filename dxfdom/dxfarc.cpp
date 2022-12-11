#include "dxfarc.h"

static const double pi = 4.0*atan(1.0);

dxfarc::dxfarc(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
, m_r(-1.0)
, m_normal(0,0,1)
{
   if(item->value() != "ARC") throw logic_error("dxfarc, expected 'ARC' but got " + item->value());

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
         case 40: { m_r    = opt.scale_factor()*child->dvalue(); break; }
         case 50: { m_ang1 = child->dvalue(); break; }
         case 51: { m_ang2 = child->dvalue(); break; }

         case 210: { m_normal.set_x(child->dvalue()); break; }
         case 220: { m_normal.set_y(child->dvalue()); break; }
         case 230: { m_normal.set_z(child->dvalue()); break; }

         default: {}
      };

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);

/*
   cout << "dxfarc" << ' ' << item->gc() << " '" << item->value() << "' "
        << " \tpc=(" << m_pc.x() << ',' <<  m_pc.y() << ',' <<  m_pc.z() << ')'
        << " \tr=" << m_r << " \tang1=" << m_ang1 << " \tang2=" << m_ang2
        << " \tn=(" << m_normal.x() << ',' <<  m_normal.y() << ',' <<  m_normal.z() << ')'
        << endl;
*/
}

dxfarc::~dxfarc()
{
   //dtor
}

bool dxfarc::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("radius",m_r);
      xml_this.add_property("ang1",m_ang1);
      xml_this.add_property("ang2",m_ang2);
      to_xml_xyz(xml_this,"pxyz",m_pc);
      to_xml_xyz(xml_this,"vxyz",m_normal);
   }

   return retval;
}


pair<double,double> dxfarc::wcs_angles() const
{
   if(m_normal.z() > 0) return std::make_pair(m_ang1,m_ang2);
   else                 return std::make_pair(180.0-m_ang2,180.0-m_ang1);
}

void dxfarc::push_profile(dxfprofile& prof, const HTmatrix& T) const
{

   double sectol = prof.sectol();
   size_t nseg = 4;
   double alpha = 2.0*pi/nseg;
   while(m_r*(1.0-cos(0.5*alpha)) >  sectol) {
      nseg += 2;
      alpha = 2*pi/nseg;
   }
   double dang = 2*pi/nseg;

   // convert arc into WCS

   // this is a simplified conversion assuming the arc is in XY plane
   // and using the normal vector to determine the direction of the arc.
   // both the center and angles are adjusted when normal is pointing "down"
   // This implementation is not 100% general, but apparently good enough for XY.
   // Notice the order of the angles is switched when the normal is pointing "down"
   // since the arc is always CCW in OCS

   pair<double,double> angles = wcs_angles();
   dxfpos pc = to_wcs(m_normal,m_pc);

   // At this point we operate in WCS only

   // convert to radians
   double ang1 = angles.first*pi/180.0;
   double ang2 = angles.second*pi/180.0;
   if(ang2 < ang1) ang2 += 2*pi;

   // the arc spans ang_span radians
   double ang_span = ang2 - ang1;

   int cp=0;

   if(ang_span>0.0 && m_r>0.0) {

      std::list<dxfpos> points;

      // number of segments to create on arc
      nseg = 1 + int(ang_span/dang);

      double x0 = pc.x();
      double y0 = pc.y();

      double ang = ang1;
      bool done  = false;
      for(size_t iseg=0; iseg<nseg+1; iseg++) {

         double x   = x0 + m_r*cos(ang);
         double y   = y0 + m_r*sin(ang);
         points.push_back(dxfpos(x,y,0));
         cp++;
         if(done) break;
         ang += dang;
         if(ang >= ang2) {
            ang=ang2;
            done = true;
         }
      }

      if(points.size() > 0) {
         prof.push_back(std::make_shared<dxfcurve>(prof.pm(),points,T));
      }

   }
/*
   int w = 10;
   cout << "dxfarc" << ' '
        << "     pc=(" << setw(w) << m_pc.x() << ' '  << setw(w) <<  m_pc.y() << ')'
        << "     r="   << setw(w) << m_r << "     angles=" << setw(w) << m_ang1 << setw(w) <<m_ang2
        << "     cp="  << setw(w) << cp
        << endl;
*/

}
