#ifndef DXFSPLINE_H
#define DXFSPLINE_H

#include "dxfentity.h"
#include "dxfpos.h"
#include <vector>

class DXFDOM_PUBLIC dxfspline : public dxfentity {
public:
   dxfspline(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfspline();

   virtual string tag() const { return "SPLINE"; }

   virtual bool to_xml(xml_node& xml_this) const;
   virtual void push_profile(dxfprofile& prof) const;

protected:
   list<dxfpos> compute_curve() const;

private:
   int            m_flag;    // Spline flag
   dxfpos         m_normal;  // plane normal vector
   size_t         m_degree;  // Degree of the spline curve
   list<dxfpos>   m_cp;      // control points (not on curve)
   vector<double> m_kv;      // knot values
   list<dxfpos>   m_fp;      // fit points (on curve), NOTE: Not always available!

   vector<int>    m_btx;     // boundary types  x, [btx1,btx2]
   vector<double> m_bvx;     // boundary values x, [bvx1,bvx2]
   vector<int>    m_bty;     // boundary types  y, [bty1,bty2]
   vector<double> m_bvy;     // boundary values y, [bvy1,bvy2]
};

#endif // DXFSPLINE_H
