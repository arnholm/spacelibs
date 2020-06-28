#include "dxfposmap.h"
#include <limits>

dxfposmap::dxfposmap(double tol)
: m_tol(tol)
{}

dxfposmap::~dxfposmap()
{}

size_t dxfposmap::key(const dxfpos& p) const
{
   double dist = 1000*m_base.dist(p);
   return size_t(dist/m_tol + 0.5);
}


dxfposmap::key_range dxfposmap::equal_key_range(const dxfpos& p) const
{
   size_t the_key      = key(p);
   key_range range_lo  = m_pos.equal_range(the_key-1);
   key_range range_key = m_pos.equal_range(the_key);
   key_range range_hi  = m_pos.equal_range(the_key+1);

   key_range range_tmp = merge(range_lo,range_key);
   key_range range_tot = merge(range_tmp,range_hi);

   return range_tot;
}

dxfposmap::key_range dxfposmap::merge(const key_range& r1, const key_range& r2) const
{
   if(r1.first == r1.second)return r2;
   if(r2.first == r2.second)return r1;
   return key_range(r1.first,r2.second);
}

dxfposmap::const_iterator dxfposmap::find(const dxfpos& p) const
{
   key_range range = equal_key_range(p);

   // itfound indicates nothing found
   const_iterator itfound = end();
   double dmin            = std::numeric_limits<double>::max();

   // look in the range of candidates
   const_iterator it = range.first;
   while(it != range.second) {

      const dxfpos& pos = it->second;
      double dist = pos.dist(p);
      if(dist<=m_tol && dist<dmin ) {

         // new candidate closer than tolerance
         dmin = dist;
         itfound = it;
      }
      it++;
   }

   return itfound;
}


dxfposmap::const_iterator dxfposmap::find_create(const dxfpos& p)
{
   const_iterator itfound = find(p);

   if(itfound == end()) {
      // no match, so create new entry
      size_t key_value = key(p);
      itfound = m_pos.insert(std::make_pair(key_value,p));
   }

   return itfound;
}
