#ifndef DXFFONTEXPORT_H
#define DXFFONTEXPORT_H

#include <string>
#include <map>
#include <memory>
using namespace std;

#include "dxfprofile.h"

class DXFDOM_PUBLIC dxffontexport {
public:
   dxffontexport();
   virtual ~dxffontexport();

   void insert(const string& c, shared_ptr<dxfprofile> p) { m_cmap[c] = p; }

   void export_font(const string& font_name, const string& file_path);

protected:
   string as_font_class(const string& font_name);
   std::pair<string,string> as_function(const string& font_name, size_t counter, std::shared_ptr<dxfprofile> prof);
   string as_polygon(std::shared_ptr<dxfloop> loop, bool fwd);

private:
   map<string,shared_ptr<dxfprofile>> m_cmap;
};

#endif // DXFFONTEXPORT_H
