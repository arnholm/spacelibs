#include "dxfblock.h"

#include "dxfarc.h"
#include "dxfcircle.h"
#include "dxfellipse.h"
#include "dxfline.h"
#include "dxflwpolyline.h"
#include "dxfpolyline.h"
#include "dxfpoint.h"
#include "dxfinsert.h"

#include "dxfdummyentity.h"

dxfblock::dxfblock(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
{
   if(item->value() != "BLOCK") throw logic_error("dxfblock, expected 'BLOCK' but got " + item->value());

   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->value() != "ENDBLK") {

      string item_value = child->value();

      if(child->gc() == 0) {
              if(item_value == "LINE")       push_back(make_shared<dxfline>(child,opt));
         else if(item_value == "ARC")        push_back(make_shared<dxfarc>(child,opt));
         else if(item_value == "CIRCLE")     push_back(make_shared<dxfcircle>(child,opt));
         else if(item_value == "ELLIPSE")    push_back(make_shared<dxfellipse>(child,opt));
         else if(item_value == "LWPOLYLINE") push_back(make_shared<dxflwpolyline>(child,opt));
         else if(item_value == "POLYLINE")   push_back(make_shared<dxfpolyline>(child,opt));
         else if(item_value == "POINT")      push_back(make_shared<dxfpoint>(child,opt));
         else if(item_value == "INSERT")     push_back(make_shared<dxfinsert>(child,opt));

         else if(item_value == "SOLID")      push_back(make_shared<dxfdummyentity>(child,opt));
         else if(item_value == "MTEXT")      push_back(make_shared<dxfdummyentity>(child,opt));
         else if(item_value == "HATCH")      push_back(make_shared<dxfdummyentity>(child,opt));
         else {
            throw logic_error("dxfblock, child entity not supported " + item_value);
         }
      }
      else {
         // filter out what we are interested in
         int gc = child->gc();
         switch(gc) {

            case 2:
            case 3:  { m_name = child->value(); break; }

            case 5:  { set_handle(child->value()); break; }
            case 8:  { set_layer(child->value()); break; }

            case 10: { m_p.set_x(opt.scale_factor()*child->dvalue()); break; }
            case 20: { m_p.set_y(opt.scale_factor()*child->dvalue()); break; }
            case 30: { m_p.set_z(opt.scale_factor()*child->dvalue()); break; }

            case 70: { m_flag = child->ivalue(); break; }

            default: {}
         };
      }

      child = dxfitem::next_item();
   }
}

dxfblock::~dxfblock()
{
}

bool dxfblock::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);

   if(retval) {
      xml_this.add_property("name",m_name);
      xml_this.add_property("flag",m_flag);
      to_xml_xyz(xml_this,"pxyz",m_p);
   }

   return retval;
}

void dxfblock::push_profile(dxfprofile& prof) const
{
}
