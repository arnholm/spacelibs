#include "dxfprofileexport.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>

#include "dxfloop.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

string dxfprofileexport::GetFullPath(const string& path)
{
   string path_copy(path);
   std::replace(path_copy.begin(),path_copy.end(), '\\', '/');
   return path_copy;
}

string dxfprofileexport::GetFullName(const string& path)
{
   boost::filesystem::path fullpath(GetFullPath(path));
   return fullpath.filename().string();
}

string dxfprofileexport::GetName(const string& path)
{
   boost::filesystem::path fullpath(GetFullPath(path));
   return fullpath.stem().string();
}

string dxfprofileexport::GetExt(const string& path)
{
   boost::filesystem::path fullpath(GetFullPath(path));
   string ext = fullpath.extension().string();
   if(ext.length() > 0)ext = ext.substr(1);
   return ext;
}

string dxfprofileexport::GetFunctionName(const string& path)
{
   string path_copy(path);
   std::replace(path_copy.begin(),path_copy.end(), '-', '_');
   std::replace(path_copy.begin(),path_copy.end(), ' ', '_');
   std::replace(path_copy.begin(),path_copy.end(), '.', '_');
   if(!isalpha(path_copy[0])) path_copy = "_" + path_copy;
   return path_copy+"_dxf()";
}


dxfprofileexport::dxfprofileexport()
{}

dxfprofileexport::~dxfprofileexport()
{}

bool dxfprofileexport::export_as(const dxfprofile& profile, const string& path, bool func_only)
{
   string profile_function = GetFunctionName(GetName(path));

   time_t now = time(0);
   const size_t lbuf=30;
   char buff[lbuf];
   strftime(buff, lbuf, "%Y-%m-%d %H:%M:%S", localtime(&now));

   ofstream out(path);
   out << "// AngelCAD script" << endl;
   out << "shape2d@ " + profile_function << endl;
   out << "{" << endl;
   out << "   // Auto-generated from DXF at "<< buff  << endl;

   size_t icount = 0;
   for(auto loop : profile) {

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

   if(!func_only) {

      out << " " << endl;
      out << "shape@ main_shape()" << endl;
      out << "{" << endl;
      out << "   solid@ profile_body   = null;" << endl;
      out << " " << endl;
      out << "   double extrude_height = 10; " << endl;
      out << "   @profile_body = linear_extrude("+profile_function+",height:extrude_height);" << endl;
      out << " " << endl;
      out << "   double model_scale = 1.0;  // change this to get larger/smaller" << endl;
      out << "   return scale(model_scale)*profile_body;" << endl;
      out << "}" << endl;

      out << " " << endl;
      out << "void main()" << endl;
      out << "{" << endl;
      out << "   shape@ obj = main_shape();" << endl;
      out << "   obj.write_xcsg(GetOutputFullPath('.xcsg'));" << endl;
      out << "}" << endl;
   }


   return true;
}


string dxfprofileexport::as_polygon(std::shared_ptr<dxfloop> loop, bool fwd)
{
   int icount = 0;
   ostringstream out;
   out << "polygon( array<pos2d@> = { ";
   for(auto p : *loop) {
      if(icount > 0) out << ',';
      out << "{" << setprecision(10) << p.x() << ','
                 << setprecision(10) << p.y() << '}';
      icount++;
   }
   out << "} )";

   return out.str();
}



bool dxfprofileexport::export_scad(const dxfprofile& profile, const string& path)
{
   string profile_function = GetFunctionName(GetName(path));

   time_t now = time(0);
   const size_t lbuf=30;
   char buff[lbuf];
   strftime(buff, lbuf, "%Y-%m-%d %H:%M:%S", localtime(&now));

   // convert to a sorted list
   // the smallest area will be first on the list so loops need to be popped
   // from the back() to process the largest to smallest

   list<std::shared_ptr<dxfloop>> loop_list;
   size_t icount = 0;
   for(auto loop: profile) {
      loop_list.push_front(loop);
   }

   ofstream out(path);
   out << "// OpenSCAD script" << endl;
   out << "extrude_height = 10;" << endl;
   out << "scale_factor = 1.0;" << endl;
   out << " " << endl;
   out << "linear_extrude(height=extrude_height)" << endl;
   out << "scale(scale_factor)" << endl;
   out << profile_function << ';'<< endl;

   out << " " << endl;
   out << "module " + profile_function << endl;
   out << "{" << endl;
   out << "   // Auto-generated from DXF at "<< buff  << endl;

      std::shared_ptr<dxfloop> loop = loop_list.front();
      loop_list.pop_front();
      out << scad_booleans(1,loop,loop_list);

   out << "}" << endl;

   return true;
}

string dxfprofileexport::indent(size_t icount)
{
   ostringstream out;
   for(size_t i=0; i<3*icount; i++) out << ' ';
   return out.str();
}

string dxfprofileexport::scad_booleans(size_t icount, std::shared_ptr<dxfloop> A_loop, list<std::shared_ptr<dxfloop>>& loop_list)
{
   ostringstream out;
   bool   A_fwd  = (A_loop->signed_area() > 0.0);

   if(loop_list.size() > 0) {
      std::shared_ptr<dxfloop>  B_loop = loop_list.front();
      loop_list.pop_front();

      bool   B_fwd  = (B_loop->signed_area() > 0.0);
      if(A_fwd) out << indent(icount)   << "union() {" << endl;
      else      out << indent(icount)   << "difference() {" << endl;
                out <<                     scad_booleans(icount+1,B_loop,loop_list);
                out << indent(icount+1) << scad_polygon(A_loop,A_fwd) << endl;
                out << indent(icount)   << "}" << endl;
   }
   else {
      out << indent(icount) << scad_polygon(A_loop,A_fwd)<< endl;
   }

   return out.str();
}


string dxfprofileexport::scad_polygon(std::shared_ptr<dxfloop> loop, bool fwd)
{
   int icount = 0;
   ostringstream out;
   out << "polygon( points = [ ";
   for(auto p : *loop) {
      if(icount > 0) out << ',';
      out << "[" << p.x() << ',' << p.y() << "]";
      icount++;
   }
   out << " ] );";

   return out.str();
}




void dxfprofileexport::export_p2d(const dxfprofile& profile, const string& path)
{
   ofstream out(path);

   /*

      2d profile file format *.p2d
      ============================
      p2d <nc> <xlow> <ylow> <xhigh> <yhigh>   <--- number of contour paths + bounding box of profile
      p2d_path <np> <signed_area>              <--- number of points + signed area of path
      x y
      x y
      x y
      p2d_path <np> <signed_area>
      x y
      x y
      x y

   */
   double xlo = 1E9;
   double xhi = -1E9;
   double ylo = 1E9;
   double yhi = -1E9;

   size_t nc = 0;
   for(auto& contour : profile) {
      if(contour->size() > 0) nc++;
      for(auto& p : *contour) {
         xlo = (p.x() < xlo)? p.x() : xlo;
         ylo = (p.y() < ylo)? p.y() : ylo;

         xhi = (p.x() > xhi)? p.x() : xhi;
         yhi = (p.y() > yhi)? p.y() : yhi;
      }
   }

   out << "p2d " << nc << ' ' << xlo << ' ' <<  ylo << ' ' << xhi << ' ' << yhi << std::endl;
   for(auto& loop : profile) {
      if(loop->size() > 0) {
         out << "p2d_path " << loop->size() << " " << loop->signed_area()  << std::endl;
         for(auto& p : *loop) {
            out << std::setprecision(16) << p.x() << ' ' << std::setprecision(16) <<  p.y() << std::endl;
         }
      }
   }
}

