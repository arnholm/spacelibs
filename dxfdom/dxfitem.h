#ifndef DFXITEM_H
#define DFXITEM_H

#include "dxfdom_config.h"

#include <memory>
#include <istream>
#include <ostream>
#include <string>
#include <list>
#include <set>
using namespace std;

// dxfitem contains the original raw, uninterpreted data from a DXF file

class DXFDOM_PUBLIC dxfitem {
public:
   dxfitem(int gc, const string& value, int lno);
   virtual ~dxfitem();

   // Read all dxf items from input stream
   static void   clear_items();
   static size_t read_items(istream& in);
   // return layers in DXF
   static const set<string>& layers() { return m_layers; }

   static shared_ptr<dxfitem> next_item();
   static void push_front(shared_ptr<dxfitem> item);


   // write item to dxf output stream
   void  write(ostream& out) const;

   // return raw item values
   int gc() const       { return m_gc; }
   string value() const { return m_value; }

   // return value as integer
   int ivalue() const;

   // return value as double
   double dvalue() const;

   int lno() const { return m_lno; }

protected:
   static shared_ptr<dxfitem> front();
   static void pop_front();

private:
   int     m_gc;     // "group code"
   string  m_value;  // string value
   int     m_lno;    // DXF file line number of this item

private:
   static set<string>               m_layers;
   static list<shared_ptr<dxfitem>> m_items;
};

#endif // DFXITEM_H
