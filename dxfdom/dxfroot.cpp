#include "dxfroot.h"
#include "dxfsection.h"
#include "dxfblock.h"
#include "dxfprofile.h"
#include "spaceio/xml_node.h"

dxfroot::dxfroot(istream& in, const dxfxmloptions& opt)
: dxfobject(make_shared<dxfitem>(0,"dxfroot",-1),opt)
, m_profile(make_shared<dxfprofile>(opt))
{
   dxfitem::read_items(in);
   m_layers = dxfitem::layers();

   shared_ptr<dxfitem> child = dxfitem::next_item();
   while(child.get()) {

      string item_value = child->value();
      if(item_value == "SECTION")     push_back(make_shared<dxfsection>(child,opt));
      else if(item_value != "ENDSEC") push_back(make_shared<dxfobject>(child,opt));
      child = dxfitem::next_item();
   }
   dxfitem::clear_items();
}

dxfroot::~dxfroot()
{
}

bool dxfroot::build_xml()
{
   if(m_tree.create_root("dfxroot")) {
      xml_node root;
      if(m_tree.get_root(root)) {
         for(auto& object : *this) {
            bool add_child = (object->tag() != "dxfobject") || options().include_raw();
            if(add_child) {
               xml_node xml_child = root.add_child(object->tag());
               object->to_xml(xml_child);
            }
         }

         if(m_profile->size() > 0.0) {
            xml_node xml_child = root.add_child("loops");
            m_profile->to_xml(xml_child);
         }

         return true;
      }
   }
   return false;
}

void dxfroot::build_profile()
{

   // make sure BLOCK objects are inserted in the profile first
   for(auto& object : *this) {
      if(object->tag() == "SECTION_BLOCKS") {
         shared_ptr<dxfsection> blocks = dynamic_pointer_cast<dxfsection>(object);
         if(blocks.get()){
            for(auto& item : *blocks) {
               shared_ptr<dxfblock> block = dynamic_pointer_cast<dxfblock>(item);
               if(block) {
                  m_profile->insert_block(block);
               }
            }
         }
         break;
      }
   }


   for(auto& object : *this) {
      if(object->tag() == "SECTION_ENTITIES") {
         shared_ptr<dxfsection> entities = dynamic_pointer_cast<dxfsection>(object);
         if(entities.get()){
            HTmatrix T;
            entities->build_profile(*m_profile,T);
         }
      }
   }

   m_profile->build_loops();
}


