#ifndef DXFSECTION_H
#define DXFSECTION_H

#include "dxfobject.h"
class dxfprofile;

class DXFDOM_PUBLIC dxfsection : public dxfobject {
public:
   dxfsection(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfsection();

   virtual string tag() const { return "SECTION_"+m_type; }

   virtual bool to_xml(xml_node& xml_this) const;

   void build_profile(dxfprofile& prof);

private:
   string m_type;
};

#endif // DXFSECTION_H
