#include "dxfspline.h"
#include <bitset>
#include "spacemath/spline2d.h"

dxfspline::dxfspline(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
, m_flag(0)
, m_normal(0,0,1)
{
   m_btx = {0,0};
   m_bvx = {0,0};
   m_bty = {0,0};
   m_bvy = {0,0};

   if(item->value() != "SPLINE") throw logic_error("dxfspline, expected 'SPLINE' but got " + item->value());

   map<int,double> cp,fp;

   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->gc() != 0) {

      // always push the child object, regardless of contents
      push_back(make_shared<dxfobject>(child,opt));

      // filter out what we are interested in
      int gc = child->gc();
      switch(gc) {
         case 5:  { set_handle(child->value()); break; }
         case 8:  { set_layer(child->value()); break; }

         // spline flag
         case 70: { m_flag  = child->ivalue(); break; }

         // Plane normal
         case 210: { m_normal.set_x(child->dvalue()); break; }
         case 220: { m_normal.set_y(child->dvalue()); break; }
         case 230: { m_normal.set_z(child->dvalue()); break; }

         // Control point (not on curve)
         case 10:
         case 20: { cp[gc] = opt.scale_factor()*child->dvalue(); break; }

         // Fit point (on curve)
         case 11:
         case 21: { fp[gc] = opt.scale_factor()*child->dvalue(); break; }

         // Start tangent
         case 12: { m_btx[0]=1; m_bvx[0]=child->dvalue(); break; }
         case 22: { m_bty[0]=1; m_bvy[0]=child->dvalue(); break; }

         // end tangent
         case 13: { m_btx[1]=1; m_bvx[1]=child->dvalue(); break; }
         case 23: { m_bty[1]=1; m_bvy[1]=child->dvalue(); break; }

         default: {}
      };

      if(cp.size() == 2) {
         m_cp.push_back(dxfpos(cp[10],cp[20],0.0));
         cp.clear();
      }

      if(fp.size() == 2) {
         m_fp.push_back(dxfpos(fp[11],fp[21],0.0));
         fp.clear();
      }

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);
}

dxfspline::~dxfspline()
{
}


bool dxfspline::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("flag",m_flag);
      to_xml_xyz(xml_this,"vxyz",m_normal);

      for(auto& p : m_fp) {
         to_xml_xyz(xml_this,"pxyz",p);
      }
   }

   return retval;
}

list<dxfpos> dxfspline::compute_curve() const
{
   list<dxfpos> curve;

   if(m_fp.size() > 0) {
      list<dxfpos> points(m_fp);

      // expand the fit points if curve is closed
      std::bitset<32> bits(m_flag);
      bool closed      = bits[0];
      bool periodic    = bits[1];

      // compute the spline from fit points
      spacemath::spline2d spline;
      vector<spacemath::pos2d> sp;
      sp.reserve(points.size());
      for(auto& fp : points) sp.push_back(spacemath::pos2d(fp.x(),fp.y()));

      // standard parameter range for spline curve
      double t0 = 0.0;
      double t1 = 1.0;

      if(periodic) {

         // repeat the curve 3 times to make it continous across edges,
         // then extract the middle 3rd parameter range
         size_t n = sp.size();
         vector<spacemath::pos2d> sp2;
         sp2.reserve(3*n);
         for(size_t i=0; i<n; i++)   sp2.push_back(sp[i]);
         for(size_t i=1; i<n-1; i++) sp2.push_back(sp[i]);
         for(size_t i=0; i<n; i++)   sp2.push_back(sp[i]);
         spline.compute_spline(sp2);

         // define normalised parameter range for original curve
         t0 = 1.0/3.0;
         t1 = 2.0/3.0;
      }
      else {
         // boundary condition calculation is questionable here...
         spline.compute_spline(sp,m_btx,m_bvx,m_bty,m_bvy);
      }

      // approximate the spline curve using 10x number of segments
      // TODO: improve this using secant tolerance

      size_t nseg = 10*(points.size()-1);
      double dt   = (t1-t0)/nseg;
      double t    = t0;
      while(t <= t1) {
         auto p = spline.pos(t);
         curve.push_back(dxfpos(p.x(),p.y(),0.0));
         t += dt;
      }
   }
   return std::move(curve);
}


void dxfspline::push_profile(dxfprofile& prof) const
{
   prof.push_back(std::make_shared<dxfcurve>(prof.pm(),compute_curve()));
}
