#ifndef DXFPROFILEEXPORT_H
#define DXFPROFILEEXPORT_H

#include <string>
using namespace std;

#include "dxfprofile.h"

class DXFDOM_PUBLIC dxfprofileexport {
public:
   dxfprofileexport();
   virtual ~dxfprofileexport();

   bool export_as(const dxfprofile& profile, const string& path, bool func_only);
   bool export_scad(const dxfprofile& profile, const string& path);
   void export_p2d(const dxfprofile& profile, const string& path);

   string scad_booleans(size_t icount, std::shared_ptr<dxfloop> A_loop, list<std::shared_ptr<dxfloop>>& loop_list);

private:

   string GetFullPath(const string& path);
   string GetFullName(const string& path);
   string GetName(const string& path);
   string GetExt(const string& path);

   string GetFunctionName(const string& path);

   string as_polygon(std::shared_ptr<dxfloop> loop, bool fwd);
   string scad_polygon(std::shared_ptr<dxfloop> loop, bool fwd);

   string indent(size_t icount);

};

#endif // DXFPROFILEEXPORT_H
