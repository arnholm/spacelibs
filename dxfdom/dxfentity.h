#ifndef DXFENTITY_H
#define DXFENTITY_H

#include "dxfobject.h"
#include "dxfprofile.h"

#include "spacemath/HTmatrix.h"
using namespace spacemath;

class DXFDOM_PUBLIC dxfentity : public dxfobject {
public:
   dxfentity(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfentity();

   virtual bool to_xml(xml_node& xml_this) const;

   void set_layer(const string& layer) { m_layer = layer; }
   string layer() const { return m_layer; }

   virtual void push_profile(dxfprofile& prof, const HTmatrix& T) const = 0;

   // helper function to convert point from OCS to WCS
   dxfpos to_wcs(const dxfpos& normal, const dxfpos& pos) const;

   virtual HTmatrix get_matrix() const { return HTmatrix(); }

   dxfpos transform_pos(const HTmatrix& T, const dxfpos& p) const;

private:
   string        m_layer;  // layer this object belongs to (may be blank)
};

#endif // DXFENTITY_H
