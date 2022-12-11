#ifndef DXFPOLYLINE_H
#define DXFPOLYLINE_H

#include "dxfentity.h"
#include "dxfvertex.h"

class DXFDOM_PUBLIC dxfpolyline : public dxfentity {
public:
   dxfpolyline(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfpolyline();

   virtual string tag() const { return "POLYLINE"; }

   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof, const HTmatrix& T) const;

private:
   int                         m_flag;    // Polyline flag
   int                         m_ctyp;    // curve type
   dxfpos                      m_normal;  // plane normal vector
   list<shared_ptr<dxfvertex>> m_v;
};

#endif // DXFPOLYLINE_H
