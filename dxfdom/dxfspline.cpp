#include "dxfspline.h"
#include <bitset>
#include "spacemath/spline2d.h"

dxfspline::dxfspline(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
, m_flag(0)
, m_degree(0)
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
         case 71: { m_degree = child->ivalue(); break; }
         case 72: { m_kv.reserve(child->ivalue()); break; }

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

         // Knot value
         case 40: { m_kv.push_back(child->dvalue()); break; }

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

  // for(auto& p : m_fp) cout << p.x() << ' ' << p.y() << endl;
}

dxfspline::~dxfspline()
{
}


bool dxfspline::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("flag",m_flag);
      xml_this.add_property("degree",m_degree);
      to_xml_xyz(xml_this,"vxyz",m_normal);

      xml_node xml_cp = xml_this.add_child("cp");
      for(auto& p : m_cp) {
         to_xml_xyz(xml_cp,"pxyz",p);
      }
      xml_node xml_knots = xml_this.add_child("knots");
      for(auto& v : m_kv) {
         xml_node xml_kv = xml_knots.add_child("kv");
         xml_kv.add_property("v",v);
      }

      xml_node xml_fp = xml_this.add_child("fp");
      for(auto& p : m_fp) {
         to_xml_xyz(xml_fp,"pxyz",p);
      }

      // boundary conditions for x
      xml_node xml_bx = xml_this.add_child("bx");
      xml_bx.add_property("bt0",m_btx[0]);
      xml_bx.add_property("bv0",m_bvx[0]);
      xml_bx.add_property("bt1",m_btx[0]);
      xml_bx.add_property("bv1",m_bvx[0]);

      // boundary conditions for y
      xml_node xml_by = xml_this.add_child("by");
      xml_by.add_property("bt0",m_bty[0]);
      xml_by.add_property("bv0",m_bvy[0]);
      xml_by.add_property("bt1",m_bty[0]);
      xml_by.add_property("bv1",m_bvy[0]);
   }

   return retval;
}

list<dxfpos> dxfspline::compute_curve() const
{
   list<dxfpos> curve;

   if(m_fp.size() > 1) {
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

      //Spline with boundary condition calculation
      spline.compute_spline(sp,m_btx,m_bvx,m_bty,m_bvy);

      // approximate the spline curve using 10x number of
      // polyline segments compared to the fit point segments
      // TODO: improve this using secant tolerance?

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
   if(m_fp.size() > 1) {
      prof.push_back(std::make_shared<dxfcurve>(prof.pm(),compute_curve()));
   }
   else {
      cout << "Error: DXF spline curves must have 2 or more fit points" << endl;
   }
}
