#ifndef DXFDUMMYENTITY_H
#define DXFDUMMYENTITY_H

#include "dxfentity.h"

class DXFDOM_PUBLIC dxfdummyentity : public dxfentity {
public:
   dxfdummyentity(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfdummyentity();

   virtual void push_profile(dxfprofile& prof, const HTmatrix& T) const {}

   virtual bool to_xml(xml_node& xml_this) const;

protected:

};

#endif // DXFDUMMYENTITY_H
