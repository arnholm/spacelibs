#include "dxfloop_optimizer.h"
#include "spacemath/line2d.h"

#include <map>

typedef std::map<size_t,pos2d> pos_map;


dxfloop_optimizer::dxfloop_optimizer(double arrow_max, double dist_max)
: m_arrow_max(arrow_max)
, m_dist_max(dist_max)
{}

dxfloop_optimizer::~dxfloop_optimizer()
{}

static pos_map::iterator next_pos(pos_map& pmap, pos_map::iterator it)
{
   // next_pos finds the next position in the lopp, possibly beyond the vector end
   pos_map::iterator itnext = it;
   itnext++;
   if(itnext == pmap.end()) itnext = pmap.begin();
   return itnext;
}

std::vector<pos2d> dxfloop_optimizer::optimize(const std::vector<pos2d>& points) const
{
   std::vector<pos2d> opt_points;
   if(points.size() > 0) {

      // first make a map to simplify erasing, the key is the original vector index
      // we need to use a map and not an unordered_map, as the sequence is significant
      pos_map pmap;
      for(size_t i=0; i<points.size(); i++) {
         pmap[i] = points[i];
      }

      pos_map::iterator iprev = pmap.begin();
      pos_map::iterator imid  = next_pos(pmap,iprev);
      pos_map::iterator inext = next_pos(pmap,imid);

      size_t icount  = 0;  // counts the number of tests, i.e. rounds of the while loop
      size_t ierased = 0;  // count when last erased happened
      bool   finished = false;
      while(!finished) {

         // minimal size of loop vector is 3
         if(pmap.size() < 4)break;

         icount++;

         pos2d p1(iprev->second);
         pos2d midpos(imid->second);
         pos2d p2(inext->second);
         double dist = p1.dist(midpos);

         line2d  line(p1,p2);
         pos2d  proj = line.interpolate(line.project(midpos));
         double  arrow = midpos.dist(proj);
         if((dist > m_dist_max) || (arrow > m_arrow_max)) {
            // keep midpos
             iprev = next_pos(pmap,iprev);
             imid  = next_pos(pmap,imid);
             inext = next_pos(pmap,inext);
         }
         else {
            // eliminate midpos
            pos_map::iterator ierase = imid;
            pmap.erase(ierase);
            ierased = icount;

            iprev = inext;
            imid  = next_pos(pmap,iprev);
            inext = next_pos(pmap,imid);
         }

         // if we have gone 2 rounds without any points erased, we call it quits
         finished = ((icount - ierased) > 2*pmap.size());
      }

      // return the points remaining
      opt_points.reserve(pmap.size());
      for(auto p : pmap) {
         opt_points.push_back(p.second);
      }
   }

   return std::move(opt_points);
}
