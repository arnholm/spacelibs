#include "dxfloop.h"

dxfloop::dxfloop()
{}

dxfloop::~dxfloop()
{}

void dxfloop::push_back(const dxfpos& p)
{
   m_points.push_back(p);
}

void dxfloop::push_back(const std::list<dxfpos>& points)
{
   for(auto& p : points) m_points.push_back(p);
}


double dxfloop::signed_area() const
{
   /*
     Method:

     ref http://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
     Some of the suggested methods will fail in the case of a non-convex polygon,
     such as a crescent. Here's a simple one that will work with non-convex polygons
     (it'll even work with a self-intersecting polygon like a figure-eight, telling you whether it's mostly clockwise).

     Sum over the edges, (x2-x1)(y2+y1). If the result is positive the curve is clockwise,
     if it's negative the curve is counter-clockwise. (The result is twice the enclosed area, with a +/- convention.)

     point[0] = (5,0)   edge[0]: (6-5)(4+0) =   4
     point[1] = (6,4)   edge[1]: (4-6)(5+4) = -18
     point[2] = (4,5)   edge[2]: (1-4)(5+5) = -30
     point[3] = (1,5)   edge[3]: (1-1)(0+5) =   0
     point[4] = (1,0)   edge[4]: (5-1)(0+0) =   0
                                              ---
                                              -44  counter-clockwise
   */

   double sum = 0.0;
   if(m_points.size() > 1) {

      auto i    = m_points.begin();
      dxfpos p1 = *i;
      while(++i != m_points.end()) {

         const dxfpos& p2 = *i;

         double x1 = p1.x();
         double y1 = p1.y();

         double x2 = p2.x();
         double y2 = p2.y();

         sum += (x2 - x1)*(y2 + y1);
         p1 = p2;
      }

      // final edge
      const dxfpos& p2 = *m_points.begin();
      double x1 = p1.x();
      double y1 = p1.y();

      double x2 = p2.x();
      double y2 = p2.y();
      sum += (x2 - x1)*(y2 + y1);
   }

   // we reverse the sign here as positive areas shall correspond to CCW loops
   // whereas the above formula gives a negative result for CCW.

   // In summary, this function will return a positive area for CCW loops-
   double s_area = -0.5*sum;
   return s_area;

}

void dxfloop::erase_duplicates(double epspnt)
{
   std::list<dxfpos> tmp;
   auto iprev=m_points.end();
   for(auto i=m_points.begin();i!=m_points.end();i++) {
      if(iprev!=m_points.end()) {
         if(iprev->dist(*i) > epspnt)tmp.push_back(*i);
      }
      else {
         tmp.push_back(*i);
      }
      iprev = i;
   }
   swap(m_points,tmp);
}

void dxfloop::canonicalize()
{
   if(signed_area() < 0)  {
      m_points.reverse();
   }
}
