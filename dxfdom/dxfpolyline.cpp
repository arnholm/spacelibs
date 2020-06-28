#include "dxfpolyline.h"
#include <bitset>

dxfpolyline::dxfpolyline(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
, m_flag(0)
, m_ctyp(0)
, m_normal(0,0,1)
{
   if(item->value() != "POLYLINE") throw logic_error("dxfpolyline, expected 'POLYLINE' but got " + item->value());

   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->value() != "SEQEND") {

      // always push the child object, regardless of contents
      push_back(make_shared<dxfobject>(child,opt));

      int gc = child->gc();
      switch(gc) {
         case 5:  { set_handle(child->value()); break; }
         case 8:  { set_layer(child->value()); break; }
         case 70: { m_flag  = child->ivalue(); break; }
         case 75: { m_ctyp  = child->ivalue(); break; }

         case 210: { m_normal.set_x(child->dvalue()); break; }
         case 220: { m_normal.set_y(child->dvalue()); break; }
         case 230: { m_normal.set_z(child->dvalue()); break; }
         default: {}
      };

      string value = child->value();
      if(value == "VERTEX") {
         m_v.push_back(make_shared<dxfvertex>(child,opt));
      }

      child = dxfitem::next_item();
   }
   // don't push SEQEND
   // dxfitem::push_front(child);

/*
   cout << "dxfpolyline" << ' ' << item->gc() << " '" << item->value() << "' "
        << " nv=" << m_v.size() << endl;
*/
}

dxfpolyline::~dxfpolyline()
{
   //dtor
}

bool dxfpolyline::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("flag",m_flag);
      xml_this.add_property("ctyp",m_ctyp);
      to_xml_xyz(xml_this,"vxyz",m_normal);

      for(auto& v : m_v) {
         xml_node xml_vertex = xml_this.add_child(v->tag());
         v->to_xml(xml_vertex);
      }
   }

   return retval;
}

void dxfpolyline::push_profile(dxfprofile& prof) const
{
   if(m_v.size() > 0) {

      std::bitset<32> bits(m_flag);

      std::list<dxfpos> points;
      int j=0;
      for(auto i=m_v.begin(); i!= m_v.end(); i++) {
         shared_ptr<dxfvertex> v = *i;
         points.push_back(v->p());


         // possibly add bulge points
         if(fabs(v->bulge()) > 0.0) {
            auto inext = i;
            inext++;
            v->compute_bulge(points,*inext,prof.sectol());
         }

      }

      // poyline must have at least 2 points
      if(points.size() > 1) {
         // if the polyline is flagged as closed but final point not matching 1st,
         // we add an extra point on the curve to close it.
         bool closed = bits[0];
         const dxfpos& p1 = points.front();
         const dxfpos& pn = points.back();
         double dist = p1.dist(pn);
         if(closed && (dist>prof.epspnt()) ) {
            points.push_back(p1);
         }
         prof.push_back(std::make_shared<dxfcurve>(prof.pm(),points));
      }
   }
}
