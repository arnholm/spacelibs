#ifndef DXFPOINT_H
#define DXFPOINT_H

#include "dxfentity.h"
#include "dxfpos.h"


class DXFDOM_PUBLIC dxfpoint : public dxfentity {
public:
   dxfpoint(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfpoint();

   virtual string tag() const { return "POINT"; }

   const dxfpos& p() const { return m_p; }

   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof, const HTmatrix& T) const;

private:
   dxfpos m_p;     // point position
};

#endif // DXFPOINT_H
