#ifndef DXFARC_H
#define DXFARC_H

#include "dxfentity.h"
#include "dxfpos.h"

class DXFDOM_PUBLIC dxfarc : public dxfentity {
public:
   dxfarc(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfarc();

   virtual string tag() const { return "ARC"; }

   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof, const HTmatrix& T) const;

protected:

   // return arc WCS start and end angles as a pair
   // The pair will always be <start,end>, even when normal pointing down
   pair<double,double> wcs_angles() const;

private:
   dxfpos m_pc;     // center point of arc  in OCS
   double m_r;      // radius
   double m_ang1;   // start angle [deg]    in OCS
   double m_ang2;   // end angle [deg]      in OCS
   dxfpos m_normal; // plane normal vector  in WCS
};

#endif // DXFARC_H
