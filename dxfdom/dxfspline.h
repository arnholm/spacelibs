#ifndef DXFSPLINE_H
#define DXFSPLINE_H

#include "dxfentity.h"
#include "dxfpos.h"
#include <vector>

// Splines in DXF can be of degree 2 or 3
// Sometimes, only control points + knots are specified
// Sometimes, fit points are specified in addition to control points & knots
// It is possible to specify a DXF spline also with only fit points
// End tangents (1st derivatives) may be applied to fit points data, but this is not 100% clear
//    https://stackoverflow.com/questions/62472305/how-does-autocad-calculate-end-tangents-for-splines-defined-only-by-fit-points
class DXFDOM_PUBLIC dxfspline : public dxfentity {
public:
   dxfspline(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfspline();

   virtual string tag() const { return "SPLINE"; }

   virtual bool to_xml(xml_node& xml_this) const;
   virtual void push_profile(dxfprofile& prof, const HTmatrix& T) const;

protected:
   list<dxfpos> compute_curve() const;
   void compute_fit_points();

private:
   int            m_flag;    // Spline flag
   dxfpos         m_normal;  // plane normal vector
   size_t         m_degree;  // Degree of the spline curve
   list<dxfpos>   m_cp;      // control points (not on curve)
   vector<double> m_kv;      // knot values
   list<dxfpos>   m_fp;      // fit points (on curve), NOTE: Not always available from DXF!

   vector<int>    m_btx;     // boundary types  x, [btx1,btx2]
   vector<double> m_bvx;     // boundary values x, [bvx1,bvx2]
   vector<int>    m_bty;     // boundary types  y, [bty1,bty2]
   vector<double> m_bvy;     // boundary values y, [bvy1,bvy2]

   double         m_sectol;  // secant tolerance
};

#endif // DXFSPLINE_H
