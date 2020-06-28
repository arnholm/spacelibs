#ifndef DXFROOT_H
#define DXFROOT_H

#include "dxfobject.h"
#include "dxfxmloptions.h"
#include "dxfprofile.h"
#include <string>
#include <set>
using namespace std;

#include "spaceio/xml_tree.h"
class dxfprofile;

// https://www.autodesk.com/techpubs/autocad/acad2000/dxf/

// main entry point for reading a DXF file and creating a dxfprofile
class DXFDOM_PUBLIC dxfroot : public dxfobject {
public:

   // create a DXF root object from a dxf input stream
   dxfroot(istream& in, const dxfxmloptions& opt);
   virtual ~dxfroot();

   // Access DXF layer names after parsong
   set<string> layers() const { return m_layers; }
   typedef std::set<string>::iterator layer_iterator;
   layer_iterator layer_begin() { return m_layers.begin(); }
   layer_iterator layer_end()   { return m_layers.end(); }

   // build the profile after importing
   void build_profile();

   // access the resulting profile
   shared_ptr<dxfprofile>  profile() const { return m_profile; }

   // optional: bouild and export DXF raw data as an XML file
   bool build_xml();
   const xml_tree&   tree() const { return m_tree; }

protected:
   set<string>             m_layers;  // DXF layer names
   xml_tree                m_tree;
   shared_ptr<dxfprofile>  m_profile; // computed profile
};

#endif // DXFROOT_H
