#include "dxfentitygeneric.h"

dxfentitygeneric::dxfentitygeneric(shared_ptr<dxfitem> item, const dxfxmloptions& opt, const string& end_tag)
: dxfentity(item,opt)
, m_end_tag(end_tag)
{
   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->gc() != 0) {

      // always push the child object, regardless of contents
      push_back(make_shared<dxfobject>(child,opt));

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);
}

dxfentitygeneric::~dxfentitygeneric()
{
}
