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
