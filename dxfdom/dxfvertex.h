#ifndef DXFVERTEX_H
#define DXFVERTEX_H

#include "dxfentity.h"
#include "dxfpos.h"


class DXFDOM_PUBLIC dxfvertex : public dxfentity {
public:
   dxfvertex(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfvertex();

   virtual string tag() const { return "VERTEX"; }

   const dxfpos& p() const { return m_p; }
   double bulge() const { return m_bulge; }

   size_t compute_bulge(std::list<dxfpos>& points, shared_ptr<dxfvertex> vnext, double sectol);

   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof) const;

private:
   int    m_flag;  // Vertex flag
   dxfpos m_p;     // vertex position
   double m_bulge; // A bulge is the tangent of one fourth the included angle
                   // for an arc segment, made negative if the arc goes clockwise
                   // from the start point to the endpoint.
                   // A bulge of 0 indicates a straight segment, and a bulge of 1 is a semicircle.
};

#endif // DXFVERTEX_H
