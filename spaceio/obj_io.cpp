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

#include "obj_io.h"
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iomanip>

namespace spaceio {

   static void tokenize(const std::string& input,
                        const std::string& delimiters,
                        std::vector<std::string>& tokens)
   {
      using namespace std;
      string::size_type last_pos = 0;
      string::size_type pos = 0;
      while(true) {
         pos = input.find_first_of(delimiters, last_pos);
         if( pos == string::npos ) {
            if(input.length()-last_pos > 0)tokens.push_back(input.substr(last_pos));
            break;
         }
         else {
            if(pos-last_pos > 0)tokens.push_back(input.substr(last_pos, pos - last_pos));
            last_pos = pos + 1;
         }
      }
   }

   static pface read_face(const std::string& input)
   {
      // https://en.wikipedia.org/wiki/Wavefront_.obj_file

      // format of input line
      // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
      // we are only interested in v1,v2,v3...

      // Note that the minimal legal format is
      // f v1 v2 v3 ...

      std::vector<std::string> tokens;
      tokenize(input," ",tokens);
      pface face;

      // skip first token since that is the 'f' keyword
      for(size_t itok=1;itok<tokens.size();itok++) {
         const std::string& token   = tokens[itok];
         // vertex index is value before first '/' (if any)
         std::string::size_type pos = token.find_first_of('/');
         // subtract one since indices are 1-based in OBJ
         face.push_back(boost::lexical_cast<size_t>(token.substr(0,pos))-1);
      }
      return face;
   }


   obj_io::obj_io()
   {}

   obj_io::~obj_io()
   {}


   bool obj_io::is_obj(const std::string& file_path)
   {
      boost::filesystem::path fullpath(file_path);
      std::string ext = fullpath.extension().string();
      std::transform(ext.begin(),ext.end(),ext.begin(),::tolower);
      if(ext == ".obj")return true;

      return false;
   }

   std::shared_ptr<ph3d_vector> obj_io::read(const std::string& file_path)
   {
       if(!is_obj(file_path)) throw std::logic_error("File is not an OBJ file:" + file_path);

      boost::filesystem::path fullpath(file_path);
      if(!boost::filesystem::exists(fullpath)) {
         std::string message = "obj_io::read(...) The file does not exist: " + file_path;
         throw std::logic_error(message);
      }

      std::shared_ptr<ph3d_vector> polyset(new ph3d_vector);
      vtx_vec vert;
      pface_vec faces;

      std::ifstream obj(file_path);
      if(obj.is_open()) {

         std::string line;
         while(std::getline(obj,line)) {

            std::string keyword;
            std::istringstream in(line);

            // ignore everything but 'v' for vertex and 'f' for face
            in >> keyword;
            if(keyword == "v") {
               // vertex
               double x(0.0),y(0.0),z(0.0);
               in >> x >> y >> z;
               vert.push_back(pos3d(x,y,z));
            }
            else if (keyword == "f") {
               // face
               faces.push_back(read_face(line));
            }
         }
      }

      polyset->push_back(std::shared_ptr<polyhedron3d>(new polyhedron3d(vert,faces)));

      return polyset;
   }


   std::string obj_io::write(std::shared_ptr<spacemath::polyhedron3d> poly, const std::string& file_path)
   {
      boost::filesystem::path fullpath(file_path);
      boost::filesystem::path off_path = fullpath.parent_path() / fullpath.stem();

      std::string path;
      std::ostringstream postfix;
      postfix << ".obj";
      path = off_path.string() + postfix.str();
      std::replace(path.begin(),path.end(), '\\', '/');


      std::ofstream out(path);
      std::string object_id = fullpath.stem().string();
      out << "# OBJ file "<< std::endl;
      out  << "o " << object_id << std::endl;

      // ========= vertices =================
      for(size_t ivert=0; ivert<poly->vertex_size(); ivert++) {
         const pos3d& v = poly->vertex(ivert);
         out << "v " << std::setprecision(16) << v.x() << ' ' << std::setprecision(16) << v.y() << ' ' << std::setprecision(16) <<  v.z() << std::endl;
      }

      // ========= faces =================
      for(size_t iface = 0; iface<poly->face_size(); ++iface) {
         const pface& f = poly->face(iface);

         size_t nvert = f.size();
         out << "f ";
         for(size_t ivert=0; ivert<nvert; ivert++) {
            // indices are 1-based in OBJ
            out << f[ivert]+1 << ' ';
         }
         out << std::endl;
      }

      return path;
   }

    std::string  obj_io::write(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path)
   {
      std::string res;
      if(polyset->size() == 1) {
         res = write((*polyset)[0],file_path);
      }
      else {
         for(size_t ipoly=0; ipoly<polyset->size(); ipoly++) {
            boost::filesystem::path fullpath(file_path);
            boost::filesystem::path off_path = fullpath.parent_path() / fullpath.stem();
            std::string path = off_path.string() + '_' + std::to_string(ipoly) + ".obj";
            res = write((*polyset)[ipoly],path);
         }
      }
      return res;
   }

}
