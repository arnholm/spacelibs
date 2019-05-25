// BeginLicense:
// Part of: spacelibs - reusable libraries for 3d space calculations
// Copyright (C) 2017 Carsten Arnholm
// All rights reserved
//
// This file may be used under the terms of either the GNU General
// Public License version 2 or 3 (at your option) as published by the
// Free Software Foundation and appearing in the files LICENSE.GPL2
// and LICENSE.GPL3 included in the packaging of this file.
//
// This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE.
// EndLicense:

#ifndef MULTIMAP_POS3D_H
#define MULTIMAP_POS3D_H

/*
   Multimap_pos3d is a special purpose STL-style generic container
   which uses 3d coordinates as keys (i.e. pos3d).

   Many applications need to search for information based on coordinates
   and this container does so very efficiently.
*/

#include <map>
#include "spacemath/pos3d.h"
using namespace std;
using namespace spacemath;

template<class T>
class multimap_pos3d {
public:

   multimap_pos3d() : m_tol(1.0e-3){};

   void setTolerance(double tol) { m_tol = tol; }

   // value_type is used when inserting values
   typedef pair<pos3d,T>                              value_type;

   // internal container type, iterator and range
   typedef multimap<long,value_type,std::less<long> > PosMap;
   typedef typename PosMap::iterator                  iterator;
   typedef pair<iterator,iterator>                    range_pair;

   // keyed_value_type is the actual type stored in the container
   typedef pair<long,value_type>                      keyed_value_type;

    // simple iteration over the whole multimap
   iterator begin();
   iterator end();

   // find returns the match closest to the given pos
   // or end() if no match found
   iterator find(const pos3d& pos);

   // size and clear do the obvious things
   size_t size();
   void clear();

   // erase isn't exactly a mystery eiter
   iterator erase(iterator it);

   // pos_range is the special type that allows
   // finding instances based on coordinates
   class pos_range {
   public:
      pos_range(const pos3d& pos,const range_pair& range,double tol)
         : m_pos(pos),m_range(range),m_tol(tol) {}

      const pos3d& range_position() const        { return m_pos; }
      long  key(iterator it) const               { return it->first; }
      const pos3d& position(iterator it) const   { return it->second.first;}
      const T& value(iterator it) const          { return it->second.second; }
      range_pair& range()                        { return m_range; }
      const range_pair& range() const            { return m_range; }

      // retirieve the first item in the range (always exact match)
      iterator begin() const                     { return m_range.first; }

      // one beyond range
      iterator end() const                       { return m_range.second; }

      // next exact match
      iterator next_match(iterator it)           {
                                                   it = ++it;
                                                   while(it != end()) {
                                                      const pos3d& pos = it->second.first;
                                                      double distance = pos.dist(m_pos);
                                                      if(distance <= m_tol)break;
                                                      it++;
                                                   }
                                                   return it;
                                                 }
      double tol() { return m_tol; }
   private:
      pos3d      m_pos;      // the range is based on this coordinate set
      range_pair m_range;    // the candidate instances
      double     m_tol;      // the tolerance (and distance between spherical shells)
   };

   // insertion of a new value
   iterator  insert(const value_type& value);

   // extract a range of candidates
   pos_range equal_key_range(const pos3d& pos);

private:
   pos3d base();
   long  key(const pos3d& pos);
   range_pair merge(const range_pair& r1,const range_pair& r2);
   pos_range narrow_range(const pos_range& range);

private:
   PosMap  m_map;
   double  m_tol;      // the tolerance (and distance between sphere shells)
};

template <class T>
pos3d multimap_pos3d<T>::base()
{
   /*
       return the base point that everything is compared with.
       An arbitrary point is selected, which is likely to be away from other points.
   */
   return pos3d(-314.1592654,2.818000,0);
}

template <class T>
long multimap_pos3d<T>::key(const pos3d& pos)
{
   /*
       calculate the key for a given position
   */
   pos3d origin = base();
   double dist = origin.dist(pos);
   long the_key = long(dist/m_tol);
   return the_key;
}

template <class T>
typename multimap_pos3d<T>::range_pair multimap_pos3d<T>::merge(const range_pair& r1,const range_pair& r2)
{
   /*
       create one range out of two sub-ranges
   */

   if(r1.first == r1.second)return r2;
   if(r2.first == r2.second)return r1;

   return range_pair(r1.first,r2.second);
}

template <class T>
typename multimap_pos3d<T>::pos_range multimap_pos3d<T>::narrow_range(const pos_range& range)
{
   /*
      limit the range such that first and last instance actually
      matches the position. There may be non-matching instances between.
   */

   pos_range true_range(range);
   const pos3d& pos = range.range_position();

   // find first true match
   iterator it = range.begin();
   while(it != range.end()){
      double distance = pos.dist(it->second.first);
      if(distance <= m_tol){
         true_range.range().first = it;
         break;
      }
      it++;
   }

   if(it == range.range().second) {
      // empty range
      true_range.range().first  = m_map.end();
      true_range.range().second = m_map.end();
      return true_range;
   }

   // find last true match
   it = range.range().second;
   do {
      it--;
      double distance = pos.dist(it->second.first);
      if(distance <= m_tol){
         true_range.range().second = ++it;
         break;
      }
   } while (it != true_range.range().first);

   return true_range;
}

template <class T>
typename multimap_pos3d<T>::iterator multimap_pos3d<T>::insert(const value_type& value)
{
   // insert a new keyed instance
   return m_map.insert(keyed_value_type(key(value.first),value));
}

template <class T>
typename multimap_pos3d<T>::pos_range multimap_pos3d<T>::equal_key_range(const pos3d& pos)
{
   // return all instances that have the same key as the specified position

   long the_key = key(pos);
   range_pair range_low     = m_map.equal_range(the_key-1);
   range_pair range_key     = m_map.equal_range(the_key);
   range_pair range_high    = m_map.equal_range(the_key+1);

   range_pair range_low_key = merge(range_low,range_key);
   range_pair range_tot     = merge(range_low_key,range_high);
   return narrow_range(pos_range(pos,range_tot,m_tol));
}

template <class T>
typename multimap_pos3d<T>::iterator multimap_pos3d<T>::begin()
{
   return m_map.begin();
}

template <class T>
typename multimap_pos3d<T>::iterator multimap_pos3d<T>::end()
{
   return m_map.end();
}

template <class T>
size_t multimap_pos3d<T>::size()
{
   return m_map.size();
}

template <class T>
void multimap_pos3d<T>::clear()
{
   m_map.clear();
}

template <class T>
typename multimap_pos3d<T>::iterator multimap_pos3d<T>::erase(iterator it)
{
   return m_map.erase(it);
}


template <class T>
typename multimap_pos3d<T>::iterator multimap_pos3d<T>::find(const pos3d& pos)
{
   // get the range of candidates
   pos_range range = equal_key_range(pos);
   auto it = range.begin();

   // traverse the candidates (if any) and select the nearest one
   if(it != range.end()) {

      // distance to first match
      double dist0 = range.position(it).dist(pos);
      iterator itfound = it;
      it = range.next_match(it);

      // traverse the other matches
      while(it != range.end()) {
         double dist = range.position(it).dist(pos);
         if(dist < dist0) {
            // this one was closer
            dist0   = dist;
            itfound = it;
         }

         it = range.next_match(it);
      }

      // return the closest match
      return itfound;
   }

   return end();
}

#endif
