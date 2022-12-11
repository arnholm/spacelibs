#ifndef DXFENTITYGENERIC_H
#define DXFENTITYGENERIC_H

#include "dxfentity.h"


class DXFDOM_PUBLIC dxfentitygeneric : public dxfentity {
public:
   dxfentitygeneric(shared_ptr<dxfitem> item, const dxfxmloptions& opt, const string& end_tag="");
   virtual ~dxfentitygeneric();

   virtual void push_profile(dxfprofile& prof, const HTmatrix& T) const {}
private:
   string m_end_tag;
};

#endif // DXFENTITYGENERIC_H
