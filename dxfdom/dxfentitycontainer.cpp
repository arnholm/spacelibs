#include "dxfentitycontainer.h"
#include "dxfentitygeneric.h"

dxfentitycontainer::dxfentitycontainer(shared_ptr<dxfitem> item, const dxfxmloptions& opt, const string& end_tag)
: dxfentity(item,opt)
, m_end_tag(end_tag)
{
   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get() && child->value() != m_end_tag) {

      string item_value = child->value();

      if(m_end_tag.length()>0 && item_value==m_end_tag)return;

      // always push the child object, regardless of contents (except end tags)
           if(child->gc() == 0 && item_value=="BLOCK") push_back(make_shared<dxfentitygeneric>(child,opt,"ENDBLK"));
      else if(child->gc() == 0 && item_value=="TABLE") push_back(make_shared<dxfentitycontainer>(child,opt,"ENDTAB"));
      else  {
         bool ignore = (item_value=="ENDTAB") || (item_value=="ENDBLK");
         if(!ignore) {
            if(child->gc() == 0)                       push_back(make_shared<dxfentitygeneric>(child,opt));
            else                                       push_back(make_shared<dxfobject>(child,opt));
         }
      }

      child = dxfitem::next_item();
   }
   dxfitem::push_front(child);
}

dxfentitycontainer::~dxfentitycontainer()
{
   //dtor
}
