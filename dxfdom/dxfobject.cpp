#include "dxfobject.h"
#include "dxftypeid.h"
#include "dxfpos.h"

dxfobject::dxfobject(shared_ptr<dxfitem> item, const dxfxmloptions& opt)
: m_item(item)
, m_opt(opt)
{
}

dxfobject::~dxfobject()
{}

string dxfobject::tag() const
{
   return dxfclassname(typeid(*this));
}

void dxfobject::write(ostream& out) const
{
   m_item->write(out);
   for(auto& child : m_children) {
      child->write(out);
   }
}

bool dxfobject::to_xml(xml_node& xml_this) const
{
   if(m_handle.length()>0) xml_this.add_property("handle",m_handle);

   if(m_opt.include_raw()) {
      xml_this.add_property("gc",m_item->gc());
      xml_this.add_property("value",m_item->value());
   }


   for(auto& child : m_children) {
      bool add_child = (child->tag() != "dxfobject") || m_opt.include_raw();
      if(add_child) {
         xml_node xml_child = xml_this.add_child(child->tag());
         child->to_xml(xml_child);
      }
   }
   return true;
}

void dxfobject::to_xml_xyz(xml_node& xml_parent, const string& tag, const dxfpos& xyz)
{
   xml_node xml_pos = xml_parent.add_child(tag);
   xml_pos.add_property("x",xyz.x());
   xml_pos.add_property("y",xyz.y());
   xml_pos.add_property("z",xyz.z());
}
