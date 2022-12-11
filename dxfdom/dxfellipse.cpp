#include "dxfellipse.h"

static const double pi = 4.0*atan(1.0);

dxfellipse::dxfellipse(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
, m_normal(0,0,1)
, m_ratio(-1)
, m_rad1(-1)
, m_rad2(-1)
{
   if(item->value() != "ELLIPSE") throw logic_error("dxfellipse, expected 'ELLIPSE' but got " + item->value());

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

         case 11: { m_p1.set_x(opt.scale_factor()*child->dvalue()); break; }
         case 21: { m_p1.set_y(opt.scale_factor()*child->dvalue()); break; }
         case 31: { m_p1.set_z(opt.scale_factor()*child->dvalue()); break; }

         case 40: { m_ratio  = child->dvalue(); break; }
         case 41: { m_rad1   = child->dvalue(); break; }
         case 42: { m_rad2   = child->dvalue(); break; }

         case 210: { m_normal.set_x(child->dvalue()); break; }
         case 220: { m_normal.set_y(child->dvalue()); break; }
         case 230: { m_normal.set_z(child->dvalue()); break; }

         default: {}
      };

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);
/*
   cout << "dxfellipse" << ' ' << item->gc() << " '" << item->value() << "' "
        << " pc=(" << m_pc.x() << ',' <<  m_pc.y() << ',' <<  m_pc.z() << ')'
        << " p1=(" << m_p1.x() << ',' <<  m_p1.y() << ',' <<  m_p1.z() << ')'
        << " ratio=" << m_ratio
        << " par1=" << m_rad1
        << " par2=" << m_rad2
        << " n=(" << m_normal.x() << ',' <<  m_normal.y() << ',' <<  m_normal.z() << ')'
        << endl;
*/
}

dxfellipse::~dxfellipse()
{
   //dtor
}

bool dxfellipse::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("ratio",m_ratio);
      xml_this.add_property("par1",m_rad1);
      xml_this.add_property("par2",m_rad2);
      to_xml_xyz(xml_this,"pxyz",m_pc);
      to_xml_xyz(xml_this,"pxyz1",m_p1);
      to_xml_xyz(xml_this,"vxyz",m_normal);
   }

   return retval;
}

void dxfellipse::push_profile(dxfprofile& prof, const HTmatrix& T) const
{
   // https://www.autodesk.com/techpubs/autocad/acad2000/dxf/ellipse_command39s_parameter_option_dxf_06.htm

   // center of ellipse
   double x0 = m_pc.x();
   double y0 = m_pc.y();
   double z0 = m_pc.z();

   // rotation of ellipse major axis
   double angle0 = atan2(m_p1.y(),m_p1.x());
   double cos0   = cos(angle0);
   double sin0   = sin(angle0);

   // end point on major axis, relative to center
   double xm = m_p1.x();
   double ym = m_p1.y();

   // half major and minor axis
   double a  = sqrt(xm*xm + ym*ym);  // 0.5*length_of_major_axis
   double b  = m_ratio*a;            // 0.5*length_of_minor_axis

   double r  = m_ratio*a;  // radius used for estimation of nseg

   double sectol = prof.sectol();
   size_t nseg = 4;
   double alpha = 2.0*pi/nseg;
   while(r*(1.0-cos(0.5*alpha)) >  sectol) {
      nseg += 2;
      alpha = 2*pi/nseg;
   }
   double dang = 2*pi/nseg;

   // ang2 must always be > ang1
   double ang1 = m_rad1;
   double ang2 = m_rad2;
   if(ang2 < ang1) ang2 += 2*pi;

   // the ellipse arc spans ang_span radians
   double ang_span = ang2 - ang1;

   if(fabs(ang_span)>0.0) {

      std::list<dxfpos> points;

      // number of segments to create on arc
      nseg = 1 + int(fabs(ang_span/dang));

      double ang = ang1;
      bool done  = false;
      for(size_t iseg=0; iseg<nseg+1; iseg++) {

         // point in local ellipse coordinate system
         double xl = a*cos(ang);
         double yl = b*sin(ang);

         // transform to global coordinates
         double x = x0 + cos0*xl - sin0*yl;
         double y = y0 + sin0*xl + cos0*yl;

         points.push_back(dxfpos(x,y,0));
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
}
