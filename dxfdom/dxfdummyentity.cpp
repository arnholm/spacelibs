#include "dxfdummyentity.h"

dxfdummyentity::dxfdummyentity(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: dxfentity(item,opt)
{
   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->gc() != 0) {

      // always push the child object, regardless of contents
      push_back(make_shared<dxfobject>(child,opt));

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);

}

dxfdummyentity::~dxfdummyentity()
{}

bool dxfdummyentity::to_xml(xml_node& xml_this) const
{
   bool retval = dxfentity::to_xml(xml_this);
   if(retval) {

      shared_ptr<dxfitem> item = this->item();

      xml_this.add_property("value",item->value());
      xml_this.add_property("lno",item->lno());
   }

   return retval;
}
