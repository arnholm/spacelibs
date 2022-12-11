#ifndef DXFELLIPSE_H
#define DXFELLIPSE_H

#include "dxfentity.h"
#include "dxfpos.h"

class DXFDOM_PUBLIC dxfellipse : public dxfentity {
public:
   dxfellipse(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfellipse();

   virtual string tag() const { return "ELLIPSE"; }
   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof, const HTmatrix& T) const;

protected:
private:
   dxfpos m_pc;     // center point of ellipse (axis intersection)
   dxfpos m_p1;     // end point of major axis, relative to center
   double m_ratio;  // ratio of minor axis to major axis
   double m_rad1;   // "start parameter" in radians (0.0 for full ellipse)
   double m_rad2;   // "end parameter" in radians (2*pi for full ellipse)
   dxfpos m_normal; // plane normal vector
};

#endif // DXFELLIPSE_H
