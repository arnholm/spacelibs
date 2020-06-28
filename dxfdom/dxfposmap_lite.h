#ifndef DXFPOSMAP_H
#define DXFPOSMAP_H

#include "dxfdom_config.h"
#include "dxfpos.h"
#include <map>


class DXFDOM_PUBLIC dxfposmap {
public:
   typedef std::multimap<size_t,dxfpos> PosMap;
   typedef PosMap::const_iterator const_iterator;
   typedef std::pair<const_iterator,const_iterator> key_range;

   void clear() { m_pos.clear(); }


   dxfposmap(double tol);
   virtual ~dxfposmap();

   const_iterator begin() const { return m_pos.begin(); }
   const_iterator end() const   { return m_pos.end(); }

   // find existing or return end()
   const_iterator find(const dxfpos& p) const;

   // find existing or create new entry for p
   const_iterator find_create(const dxfpos& p);

protected:

   size_t key(const dxfpos& p) const;
   key_range equal_key_range(const dxfpos& p) const;
   key_range merge(const key_range& r1, const key_range& r2) const;

private:
   double  m_tol;
   dxfpos  m_base;
   PosMap  m_pos;
};

#endif // DXFPOSMAP_H
