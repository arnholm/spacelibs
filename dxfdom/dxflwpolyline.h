#ifndef DFXLWPOLYLINE_H
#define DFXLWPOLYLINE_H

#include "dxfentity.h"
#include "dxfpos.h"

class DXFDOM_PUBLIC dxflwpolyline : public dxfentity {
public:
   dxflwpolyline(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxflwpolyline();

   virtual string tag() const { return "LWPOLYLINE"; }

   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof) const;

protected:
private:
   int          m_flag;    // Polyline flag
   dxfpos       m_normal;  // plane normal vector
   list<dxfpos> m_points;
};

#endif // DFXLWPOLYLINE_H
