#ifndef DXFBLOCK_H
#define DXFBLOCK_H

#include "dxfpos.h"
class dxfprofile;

#include "dxfentity.h"

class DXFDOM_PUBLIC dxfblock : public dxfentity {
public:
   dxfblock(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfblock();

   virtual string tag() const { return "BLOCK"; }

   string name() const { return m_name; }

   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof) const;

private:
   string m_name;
   int    m_flag;    // block flag
   dxfpos m_p;       // "base point" position
};

#endif // DXFBLOCK_H
