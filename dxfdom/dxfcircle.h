#ifndef DXFCIRCLE_H
#define DXFCIRCLE_H

#include "dxfentity.h"
#include "dxfpos.h"

class DXFDOM_PUBLIC dxfcircle : public dxfentity {
public:
   dxfcircle(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfcircle();

   virtual string tag() const { return "CIRCLE"; }

   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof) const;

protected:
private:
   dxfpos m_pc;     // center point of circle
   double m_r;      // radius
   dxfpos m_normal; // plane normal vector
};

#endif // DXFCIRCLE_H
