#ifndef DXFENTITYCONTAINER_H
#define DXFENTITYCONTAINER_H

#include "dxfentity.h"

class DXFDOM_PUBLIC dxfentitycontainer : public dxfentity {
public:
   dxfentitycontainer(shared_ptr<dxfitem> item, const dxfxmloptions& opt, const string& end_tag);
   virtual ~dxfentitycontainer();

   virtual void push_profile(dxfprofile& prof) const {}

private:
   string m_end_tag;
};

#endif // DXFENTITYCONTAINER_H
