#include "dxfinsert.h"
#include "dxfblock.h"
#include "dxfline.h"

dxfinsert::dxfinsert(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
, m_scale(1,1,1)
, m_ang(0.0)
, m_nrow(1)
, m_srow(0)
, m_ncol(1)
, m_scol(0)
, m_normal(0,0,1)
{
   if(item->value() != "INSERT") throw logic_error("dxfinsert, expected 'INSERT' but got " + item->value());

   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->gc() != 0) {

      // always push the child object, regardless of contents
      push_back(make_shared<dxfobject>(child,opt));

      // filter out what we are interested in
      int gc = child->gc();
      switch(gc) {

         case 2:
         case 3:  { m_name = child->value(); break; }

         case 5:  { set_handle(child->value()); break; }
         case 8:  { set_layer(child->value()); break; }

         case 10: { m_pos.set_x(opt.scale_factor()*child->dvalue()); break; }
         case 20: { m_pos.set_y(opt.scale_factor()*child->dvalue()); break; }
         case 30: { m_pos.set_z(opt.scale_factor()*child->dvalue()); break; }

         case 41: { m_scale.set_x(child->dvalue()); break; }
         case 42: { m_scale.set_y(child->dvalue()); break; }
         case 43: { m_scale.set_z(child->dvalue()); break; }

         case 44: { m_scol = child->dvalue(); break; }
         case 45: { m_srow = child->dvalue(); break; }

         case 50: { m_ang = child->dvalue(); break; }

         case 70: { m_ncol = child->ivalue(); break; }
         case 71: { m_nrow = child->ivalue(); break; }

         case 210: { m_normal.set_x(child->dvalue()); break; }
         case 220: { m_normal.set_y(child->dvalue()); break; }
         case 230: { m_normal.set_z(child->dvalue()); break; }

         default: {}
      };

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);
/*
   cout << "dxfinsert" << ' ' << item->gc() << " '" << item->value() << "' "
        << " pc=(" << m_pos.x() << ',' <<  m_pos.y() << ',' <<  m_pos.z() << ')' << endl;
*/
}

dxfinsert::~dxfinsert()
{
   //dtor
}


bool dxfinsert::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("block",m_name);
      xml_this.add_property("ncol",m_ncol);
      xml_this.add_property("nrow",m_nrow);
      xml_this.add_property("scol",m_scol);
      xml_this.add_property("srow",m_srow);

      to_xml_xyz(xml_this,"pxyz",m_pos);
      to_xml_xyz(xml_this,"scale",m_scale);
      xml_this.add_property("angle",m_ang);
      to_xml_xyz(xml_this,"vxyz",m_normal);
   }

   // then the raw input
   return retval;
}

HTmatrix dxfinsert::get_matrix() const
{
   static const double pi = 4.0*atan(1.0);

   double rad = pi*m_ang/180;

   HTmatrix rm;  // rotation matrix
   rm(0,0) =  cos(rad);
   rm(1,0) =  sin(rad);
   rm(0,1) = -sin(rad);
   rm(1,1) =  cos(rad);

   HTmatrix sm;  // scaling matrix
   sm(0,0) =  m_scale.x();
   sm(1,1) =  m_scale.y();
   sm(2,2) =  m_scale.z();

   HTmatrix pm;  // position matrix
   pm(0,3) =  m_pos.x();
   pm(1,3) =  m_pos.y();
   pm(2,3) =  m_pos.z();

   // multiplication order has been determined by trial an error
   // this seems to work

   return pm*rm*sm;
}

dxfpos transform_pos(const HTmatrix& T, const dxfpos& p)
{
   const vmath::mat4<double>& t = T.detail();
   vmath::vec3<double> r=transform_point(t,vmath::vec3<double>(p.x(),p.y(),p.z() ) );
   return dxfpos(r[0],r[1],r[2]);
}

void dxfinsert::push_profile(dxfprofile& prof) const
{
   std::shared_ptr<dxfblock> block = prof.get_block(m_name);
   if(block.get()) {

      // block transformation matrix
      HTmatrix T = get_matrix();

      for(auto& object : *block) {

         shared_ptr<dxfline> line = dynamic_pointer_cast<dxfline>(object);
         if(line.get()) {

            dxfpos p1 = line->p1();
            dxfpos p2 = line->p2();

            p1 = transform_pos(T,p1);
            p2 = transform_pos(T,p2);

            std::list<dxfpos> points = { p1,p2};
            prof.push_back(std::make_shared<dxfcurve>(prof.pm(),points));
         }
         else {
            cout << "Warning, BLOCK item not implemented (only LINE supported): " << object->tag() << endl;
         }
      }

   }
   else {
       throw std::logic_error("dxfinsert::push_profile: BLOCK not found: " + m_name);
   }
}
