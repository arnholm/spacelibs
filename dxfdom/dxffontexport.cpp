#include "dxffontexport.h"
#include "dxfloop.h"
#include <iomanip>
#include <fstream>

dxffontexport::dxffontexport()
{
   //ctor
}

dxffontexport::~dxffontexport()
{
   //dtor
}

void dxffontexport::export_font(const string& font_name, const string& file_path)
{
   ofstream out(file_path);
   out << as_font_class(font_name) << endl;
}

string dxffontexport::as_font_class(const string& font_name)
{
   string class_name =  font_name;
   string funcdef_name = class_name+"_funcdef";

   ostringstream out;
   out << "#include \"as_font.as\"" << endl;
   out << " " << endl;
   out << "funcdef shape2d@ "<< funcdef_name << "();" << endl;
   out << "class " << class_name << " : as_font {" << endl;
   out << "   map<string," << funcdef_name<< "@> char;" << endl;
   out << "   " << endl;
   out << "   shape2d@ character(const string &in c) " << endl;
   out << "   {" << endl;
   out << "      "<< funcdef_name << "@ f = char.find(c);" << endl;
   out << "      return (@f != null)? f() : null; " << endl;
   out << "   }" << endl;
   out << "   " << endl;
   out << "   // constructor " << endl;
   out << "   "<<class_name<< "()"  << endl;  // constructor
   out << "   {"  << endl;

   list<pair<string,string> > funcs;

   size_t counter=0;
   for(auto& p : m_cmap) {
      string                    c = p.first;
      shared_ptr<dxfprofile> prof = p.second;
      auto p2 = as_function(font_name,++counter,prof);
      funcs.push_back(make_pair(c,p2.second));

      if(c[0] == '"') c = "\\\"";
      else if(c[0] == '\\') c = "\\\\";

      out << "      char.insert(\"" << c << "\",@" << p2.first<<");"<< endl;
   }

   out << "   }"  << endl;
   out << "}; " << endl;
   out << " " << endl;
   for(auto& p : funcs) {
      out << "// " << p.first << endl;
      out << p.second << endl;
   }


   return out.str();
}

std::pair<string,string> dxffontexport::as_function(const string& font_name, size_t counter, std::shared_ptr<dxfprofile> prof)
{
   string func_name = font_name + "_" + std::to_string(counter);

   ostringstream out;
   out << "shape2d@ " << func_name << "()" << endl;
   out << "{" << endl;

   size_t icount = 0;
   for(auto loop : *prof) {

      bool add = (loop->signed_area() > 0);
      if(icount++ == 0) {
         out << "   shape2d@ prof = "+ as_polygon(loop,true) +";" << endl;
      }
      else {
         if(add) out << "   @prof = prof + " + as_polygon(loop,true) +";" << endl;
         else    out << "   @prof = prof - " + as_polygon(loop,false)+";" << endl;
      }
   }

   out << "   return prof;" << endl;
   out << "}" << endl;

   return std::make_pair(func_name,out.str());
}

string dxffontexport::as_polygon(std::shared_ptr<dxfloop> loop, bool fwd)
{
   int icount = 0;
   ostringstream out;
   out << "polygon( array<pos2d@> = { ";
   for(auto p : *loop) {
      if(icount > 0) out << ',';
      out << "pos2d(" << setprecision(10) << p.x() << ','
                      << setprecision(10) << p.y() << ')';
      icount++;
   }
   out << "} )";

   return out.str();
}
