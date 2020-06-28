#ifndef DFXLINE_H
#define DFXLINE_H

#include "dxfentity.h"
#include "dxfpos.h"

class DXFDOM_PUBLIC dxfline : public dxfentity {
public:
   dxfline(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfline();

   virtual string tag() const { return "LINE"; }

   // start and end points
   const dxfpos& p1() const { return m_p1; }
   const dxfpos& p2() const { return m_p2; }

   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof) const;

private:
   dxfpos m_p1;
   dxfpos m_p2;
};

#endif // DFXLINE_H
