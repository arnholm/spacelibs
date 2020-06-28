#ifndef DFXOBJECT_H
#define DFXOBJECT_H

#include <iomanip>
#include <iostream>
#include <memory>
#include <list>
#include <map>

#include "spaceio/xml_node.h"
using namespace spaceio;

#include "dxfxmloptions.h"
#include "dxfitem.h"
class dxfpos;
using namespace std;

class DXFDOM_PUBLIC dxfobject {
public:
   typedef list<shared_ptr<dxfobject>> dxfobject_list;
   typedef dxfobject_list::const_iterator const_iterator;

   virtual string tag() const;

   dxfobject(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfobject();

   void set_handle(const string& handle) { m_handle = handle; }
   string handle() const { return m_handle; }

   // write item to dxf output stream
   void  write(ostream& out) const;

   // return the raw item for this object
   shared_ptr<dxfitem> item() const { return m_item; }

   // add child to this object
   void push_back(shared_ptr<dxfobject> object) { m_children.push_back(object); }

   // traversal
   const_iterator begin() const                 { return m_children.begin(); }
   const_iterator end()   const                 { return m_children.end(); }

   size_t size() const { return m_children.size(); }

   // the parent is responsible for creating the child tag
   virtual bool to_xml(xml_node& xml_this) const;

   static void to_xml_xyz(xml_node& xml_parent, const string& tag, const dxfpos& xyz);

   const dxfxmloptions& options() const { return m_opt; }

private:
   string              m_handle;    // dxf handle (gc=5) for this object
   shared_ptr<dxfitem> m_item;      // item for this object
   dxfobject_list      m_children;  // child objects
   dxfxmloptions       m_opt;       // options for XML objects
};

#endif // DFXOBJECT_H
