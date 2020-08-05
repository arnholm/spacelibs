#include "bspline2d.h"
#include <set>
#include "tinyspline/tinysplinecxx.h"
#include <stdexcept>

typedef tinyspline::BSpline::type TSB_type;

namespace spacemath {

   bspline2d::bspline2d(const std::vector<pos2d>&  ctrl_pnts,
                        const std::vector<double>& knots,
                        size_t degree)
   : m_cp(ctrl_pnts)
   , m_kn(knots)
   , m_degree(degree)
   {
      m_spline = compute_spline(m_cp,m_kn,m_degree);
   }

   bspline2d::~bspline2d()
   { }

   std::shared_ptr<tinyspline::BSpline>  bspline2d::compute_spline(const std::vector<pos2d>&  cp,
                                                                   const std::vector<double>& kn,
                                                                   size_t degree)
   {
      if(cp.size()  < 2) throw std::runtime_error("bspline2d::compute_spline, cp.size() must be > 2");
      if(kn.size() == 0) throw std::runtime_error("bspline2d::compute_spline, kn.size() must be > 0");

      TSB_type type = (kn.size() == 0)? TSB_type::TS_CLAMPED : TSB_type::TS_OPENED;

      // create the 2d cubic spline with given number of control points
      size_t dimension(2);
      std::shared_ptr<tinyspline::BSpline> spline = std::make_shared<tinyspline::BSpline>(cp.size(),dimension,degree,type);

      // Define and set the control points
      std::vector<tinyspline::real> ctrlp;
      ctrlp.reserve(cp.size()*dimension);
      for(auto& p : cp) {
         ctrlp.push_back(static_cast<tinyspline::real>(p.x()));
         ctrlp.push_back(static_cast<tinyspline::real>(p.y()));
      }
      spline->setControlPoints(ctrlp);

      // Define and set the knots vector
      std::vector<tinyspline::real> knots;
      knots.reserve(kn.size());
      for(auto& k : kn) {
         knots.push_back(static_cast<tinyspline::real>(k));
      }
      spline->setKnots(knots);

      return spline;
   }

   std::vector<pos2d> bspline2d::knot_points() const
   {
      // obtain a unique set of knot values
      std::set<double> fp_t;
      for(auto& k : m_kn) {
         fp_t.insert(k);
      }

      // compute the fit points, i.e. the points on the curve that match the knot values
      std::vector<pos2d> fp;
      fp.reserve(fp_t.size());
      for(double t : fp_t) {
         std::vector<tinyspline::real> result = m_spline->eval(t).result();
         fp.push_back(pos2d(result[0],result[1]));
      }

      return std::move(fp);
   }

   std::vector<pos2d> bspline2d::interpolated_points(size_t n) const
   {
      // obtain a unique set of knot values
      std::set<double> fp_t;
      for(auto& k : m_kn) {
         fp_t.insert(k);
      }

      // get unique knot values as vector
      std::vector<double> kn;
      kn.reserve(fp_t.size());
      for(double t : fp_t) kn.push_back(t);

      // expand the knot parameters
      std::vector<double> kn2;
      kn2.reserve(kn.size()*n);
      kn2.push_back(kn[0]);
      for(size_t i=1; i<kn.size(); i++) {
         double v0 = kn[i-1];
         double v1 = kn[i];
         double dv = (v1-v0)/(n+1);
         double v = v0+dv;
         for(size_t j=0;j<n;j++) {
            kn2.push_back(v);
            v += dv;
         }
         kn2.push_back(v1);
      }


      // compute the fit points, i.e. the points on the curve that match the expanded knot values
      std::vector<pos2d> fp;
      fp.reserve(kn2.size());
      for(double t : kn2) {
         std::vector<tinyspline::real> result = m_spline->eval(t).result();
         fp.push_back(pos2d(result[0],result[1]));
      }

      return std::move(fp);
   }

}
