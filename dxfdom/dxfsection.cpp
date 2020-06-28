#include "dxfsection.h"
#include "dxfarc.h"
#include "dxfcircle.h"
#include "dxfellipse.h"
#include "dxfinsert.h"
#include "dxfline.h"
#include "dxflwpolyline.h"
#include "dxfpolyline.h"
#include "dxfpoint.h"
#include "dxfblock.h"

#include "dxfdummyentity.h"
#include "dxfentitygeneric.h"
#include "dxfentitycontainer.h"

#include <iostream>

dxfsection::dxfsection(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfobject(item,opt)
{
   size_t nc = 0;
   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->value() != "ENDSEC") {

      string item_value = child->value();

      // set the section string for convenience
      // it is always the first item under SECTION
      if(nc==0){
         m_type = item_value;
        // cout << item->value() << ' ' << m_type;
      }
      nc++;

      // ENTITIES
      if(child->gc() == 0) {
              if(item_value == "LINE")       push_back(make_shared<dxfline>(child,opt));
         else if(item_value == "ARC")        push_back(make_shared<dxfarc>(child,opt));
         else if(item_value == "CIRCLE")     push_back(make_shared<dxfcircle>(child,opt));
         else if(item_value == "ELLIPSE")    push_back(make_shared<dxfellipse>(child,opt));
         else if(item_value == "INSERT")     push_back(make_shared<dxfinsert>(child,opt));
         else if(item_value == "LWPOLYLINE") push_back(make_shared<dxflwpolyline>(child,opt));
         else if(item_value == "POLYLINE")   push_back(make_shared<dxfpolyline>(child,opt));
         else if(item_value == "POINT")      push_back(make_shared<dxfpoint>(child,opt));
         else if(item_value == "BLOCK")      push_back(make_shared<dxfblock>(child,opt));

         else if(opt.include_raw()) {
            //   if(child->gc() == 0 && item_value=="BLOCK") push_back(make_shared<dxfentitygeneric>(child,opt,"ENDBLK"));
            if(child->gc() == 0 && item_value=="TABLE") push_back(make_shared<dxfentitycontainer>(child,opt,"ENDTAB"));
            else if(child->gc() == 0 )  {
               bool ignore = (item_value=="ENDTAB") || (item_value=="ENDBLK");
               if(!ignore) {
                  push_back(make_shared<dxfentitygeneric>(child,opt));
               }
            }
         }
      }
      else {
         push_back(make_shared<dxfobject>(child,opt));
      }
      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);

   // cout << " size=" << size() << endl;
}

dxfsection::~dxfsection()
{}


bool dxfsection::to_xml(xml_node& xml_this) const
{
   dxfobject::to_xml(xml_this);
   return true;
}


void dxfsection::build_profile(dxfprofile& prof)
{
   const dxfxmloptions& opts = options();

   for(auto& object : *this) {
      shared_ptr<dxfentity> entity = dynamic_pointer_cast<dxfentity>(object);
      if(entity.get()) {
         if(opts.layer_selected(entity->layer())) {
            entity->push_profile(prof);
         }
      }
   }
}
