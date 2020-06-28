#include "dxfvertex.h"

static const double pi = 4.0*atan(1.0);

dxfvertex::dxfvertex(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
, m_flag(0)
, m_bulge(0.0)
{
   if(item->value() != "VERTEX") throw logic_error("dxfvertex, expected 'VERTEX' but got " + item->value());

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

         case 42: { m_bulge = child->dvalue(); break; }

         case 70: { m_flag  = child->ivalue(); break; }

         default: {}
      };

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);

   /*
   cout << "dxfvertex" << ' ' << item->gc() << " '" << item->value() << "' "
        << " p=(" << m_p.x() << ',' <<  m_p.y() << ',' <<  m_p.z() << ')'
        << endl;
   */
}

dxfvertex::~dxfvertex()
{
}

bool dxfvertex::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("flag",m_flag);
      xml_this.add_property("bulge",m_bulge);
      to_xml_xyz(xml_this,"pxyz",m_p);
   }

   return retval;
}

void dxfvertex::push_profile(dxfprofile& prof) const
{
   // prof.push_back(m_p);
}


size_t dxfvertex::compute_bulge(std::list<dxfpos>& points, shared_ptr<dxfvertex> vnext, double sectol)
{
   // save number of points on input
   size_t np = points.size();

   if(fabs(m_bulge) > 0.0) {

      // angle spanned by the bulge arc
      double alpha     = 4.0*atan(fabs(m_bulge));

      // length of secant q (=distance between this point and next)
      dxfpos p1 = m_p;
      dxfpos p2 = vnext->p();
      double q  = p1.dist(p2);

      // Length of secant, given radius and angle:  sin(0.5*alpha)=q/(2*r) => r=0.5*q/sin(0.5*alpha)
      double r = 0.5*q/sin(0.5*alpha);

      // distance from center of circle to secant
      double s = r*cos(0.5*alpha);

      // start point
      double x1 = p1.x();
      double y1 = p1.y();

      // end point
      double x2 = p2.x();
      double y2 = p2.y();

      // midpoint on secant
      double x3 = 0.5*(x1 + x2);
      double y3 = 0.5*(y1 + y2);

      // normal ised vector to secant (nx , ny)
      double dx = (x2-x1)/q;
      double dy = (y2-y1)/q;

      // normal vector
      double nx = -dy;
      double ny =  dx;

      // compute bulge arc rotation center point, plus start and end angles
      double x0  =0.0;
      double y0  =0.0;
      double ang1=0.0;
      double ang2=0.0;
      double factor = 1.0;
      if(m_bulge > 0.0) {
         // bulge goes CCW (positive angle dir)

         x0 = x3 + s*nx;
         y0 = y3 + s*ny;

         double dx1 = x1 -x0;
         double dy1 = y1 -y0;
         ang1       = atan2(dy1,dx1);

         double dx2 = x2 -x0;
         double dy2 = y2 -y0;
         ang2       = atan2(dy2,dx2);

         if(ang1 > ang2) ang2 += 2*pi;

         factor = +1;
      }
      else {
         // bulge goes CW (negative angle dir)
         x0 = x3 - s*nx;
         y0 = y3 - s*ny;

         double dx1 = x1 -x0;
         double dy1 = y1 -y0;
         ang1       = atan2(dy1,dx1);

         double dx2 = x2 -x0;
         double dy2 = y2 -y0;
         ang2       = atan2(dy2,dx2);

         if(ang1 < ang2) ang2 -= 2*pi;

         factor = -1;
      }

      // compute number of segments corresponding to a full circle
      size_t nseg = 4;
      double beta = 2.0*pi/nseg;
      while(r*(1.0-cos(0.5*beta)) >  sectol) {
         nseg += 2;
         beta = 2*pi/nseg;
      }

      // delta angle
      double dang = factor*2*pi/nseg;

      // the arc spans ang_span radians
      double ang_span = ang2 - ang1;

      // number of segments to create on arc
      nseg = 1 + int(fabs(ang_span/dang));

      // now we can compute the bulge points
      std::list<dxfpos> bulge_points;
      double ang = ang1;
      bool done  = false;
      for(size_t iseg=0; iseg<nseg+1; iseg++) {

         double x   = x0 + r*cos(ang);
         double y   = y0 + r*sin(ang);
         bulge_points.push_back(dxfpos(x,y,0));

         if(done) break;
         ang += dang;

         if(factor > 0.0) {
            if(ang >= ang2) {
               ang=ang2;
               done = true;
            }
         }
         else {
            if(ang <= ang2) {
               ang=ang2;
               done = true;
            }
         }
      }

      if(bulge_points.size() > 1) {

         // include only internal points, not endpoints
         bulge_points.pop_front();
         bulge_points.pop_back();

         // push to result points
         for(auto& p : bulge_points) {
            points.push_back(p);
         }
      }
   }

   // return number of points added
   return points.size() - np;
}
