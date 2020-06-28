#include "dxfitem.h"
#include <sstream>
#include <set>

set<string>               dxfitem::m_layers;
list<shared_ptr<dxfitem>> dxfitem::m_items;

void dxfitem::clear_items()
{
   m_layers.clear();
   m_items.clear();
}

size_t dxfitem::read_items(istream& in)
{
   int lno = 0;
   m_items.clear();
   bool read_more = true;
   while(read_more) {

      string line1,line2;
      if(!std::getline(in,line1))break;
      lno++;
      istringstream in1(line1);
      int gc=0;
      in1 >> gc;

      if(!std::getline(in,line2))throw logic_error("dxfitem::read_items, getline failed");
      lno++;

      // remove carriage return if found at the end of the string
      size_t lastchar = line2.size()-1;
      if (!line2.empty() && line2[lastchar] == '\r') line2.erase(lastchar);

      auto item = make_shared<dxfitem>(gc,line2,lno-1);
      if(item->gc() == 8) m_layers.insert(item->value());

      m_items.push_back(item);
   }

   return m_items.size();
}

shared_ptr<dxfitem> dxfitem::next_item()
{
   shared_ptr<dxfitem> item = front();
   pop_front();
   return item;
}

shared_ptr<dxfitem> dxfitem::front()
{
   if(m_items.size() == 0)return 0;
   return m_items.front();
}

void dxfitem::pop_front()
{
   if(m_items.size() > 0) {
      m_items.pop_front();
   }
}

void dxfitem::push_front(shared_ptr<dxfitem> item)
{
   if(item.get()) {
      m_items.push_front(item);
   }
}

void dxfitem::write(ostream& out) const
{
   out << m_gc << endl;
   out << m_value << endl;
}

dxfitem::dxfitem(int  gc, const string& value, int lno)
: m_gc(gc)
, m_value(value)
, m_lno(lno)
{}

dxfitem::~dxfitem()
{}

int dxfitem::ivalue() const
{
   int value=0;
   stringstream in(m_value);
   in >> value;
   return value;
}

double  dxfitem::dvalue() const
{
   double value=0;
   stringstream in(m_value);
   in >> value;
   return value;
}
