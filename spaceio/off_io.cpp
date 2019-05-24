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

#include "off_io.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
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


   off_io::off_io()
   {}

   off_io::~off_io()
   {}


   bool off_io::is_off(const std::string& file_path)
   {
      boost::filesystem::path fullpath(file_path);
      std::string ext = fullpath.extension().string();
      std::transform(ext.begin(),ext.end(),ext.begin(),::tolower);
      if(ext == ".off")return true;

      return false;
   }


   std::shared_ptr<ph3d_vector> off_io::read(const std::string& file_path)
   {
      if(!is_off(file_path)) throw std::logic_error("File is not an OFF file:" + file_path);

      boost::filesystem::path fullpath(file_path);
      if(!boost::filesystem::exists(fullpath)) {
         std::string message = "off_io::read(...) The file does not exist: " + file_path;
         throw std::logic_error(message);
      }

      std::shared_ptr<ph3d_vector> polyset(new ph3d_vector);
      vtx_vec vert;
      pface_vec faces;

      std::ifstream off(file_path);
      if(off.is_open()) {

         std::string line;
         std::vector<std::string> tokens;
         while(tokens.size()==0) {
            std::getline(off,line);
            tokenize(line," ",tokens);
         }

         // here we might see one of 2 variants
         //
         // a) one line
         //    OFF numVertices numFaces numEdges
         //
         // b) two lines
         //    OFF
         //    numVertices numFaces numEdges

         if(tokens[0].substr(0,3) != "OFF") throw std::logic_error("Keyword 'OFF' not found in :" + file_path);

         size_t nvert=0,nface=0;
         if(tokens.size() >=3) {
            // case a
            std::string offtxt;
            std::istringstream in(line);
            in >> offtxt >> nvert >> nface;
         }
         else {

            // case b
            while(std::getline(off,line)) {

               // check for non-blank line
               std::vector<std::string> tokens;
               tokenize(line," ",tokens);
               if(tokens.size() > 0) {

                  // ok, at least one token
                  if(tokens[0][0] != '#') {

                     // We must first read number of vertices and faces
                     if(nvert==0) {
                        std::istringstream in(line);
                        in >> nvert >> nface;
                        vert.reserve(nvert);
                        faces.reserve(nface);
                        break;
                     }
                  }
               }
            }
         }

         // read vertices and faces
         size_t ivert = 0;
         size_t iface = 0;
         while(std::getline(off,line)) {

            // check for non-blank line
            std::vector<std::string> tokens;
            tokenize(line," ",tokens);
            if(tokens.size() > 0) {
               // ok, at least one token
               if(tokens[0][0] != '#') {
                  std::istringstream in(line);

                  if(ivert++ < nvert) {

                     // vertex
                     double x(0.0),y(0.0),z(0.0);
                     in >> x >> y >> z;
                     vert.push_back(pos3d(x,y,z));
                  }
                  else if(iface++ < nface) {

                     // face
                     size_t nvf=0;
                     in >> nvf;  // number of vertices in face
                     pface face;
                     face.reserve(nvf);
                     for(size_t ivf=0;ivf<nvf;ivf++) {
                        size_t ivert=0;
                        in >> ivert;
                        face.push_back(ivert);
                     }
                     faces.push_back(face);
                  }
               }
            }
         }
      }
      polyset->push_back(std::shared_ptr<polyhedron3d>(new polyhedron3d(vert,faces)));

      return polyset;
   }


   std::string off_io::write(std::shared_ptr<spacemath::polyhedron3d> poly, const std::string& file_path)
   {
      boost::filesystem::path fullpath(file_path);
      boost::filesystem::path off_path = fullpath.parent_path() / fullpath.stem();

      std::string path;
      std::ostringstream postfix;
      postfix << ".off";
      path = off_path.string() + postfix.str();
      std::replace(path.begin(),path.end(), '\\', '/');


      std::ofstream out(path);

      out << "OFF " << std::endl;
      out << poly->vertex_size() << ' ' << poly->face_size() << " 0 " <<  std::endl;  // numedges always zero

      // ========= vertices =================
      for(size_t ivert=0; ivert<poly->vertex_size(); ivert++) {
         const pos3d& v = poly->vertex(ivert);
         out << std::setprecision(12) << v.x() << ' ' << std::setprecision(12) << v.y() << ' ' << std::setprecision(12) <<  v.z() << std::endl;
      }

      // ========= faces =================
      for(size_t iface = 0; iface<poly->face_size(); ++iface) {
         const pface& f = poly->face(iface);

         size_t nvert = f.size();
         out << nvert << ' ';
         for(size_t ivert=0; ivert<nvert; ivert++) {
            out << f[ivert] << ' ';
         }
         out << std::endl;
      }

      return path;
   }

    std::string  off_io::write(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path)
   {
      std::string res;
      if(polyset->size() == 1) {
         res = write((*polyset)[0],file_path);
      }
      else {
         for(size_t ipoly=0; ipoly<polyset->size(); ipoly++) {
            boost::filesystem::path fullpath(file_path);
            boost::filesystem::path off_path = fullpath.parent_path() / fullpath.stem();
            std::string path = off_path.string() + '_' + std::to_string(ipoly) + ".off";
            res = write((*polyset)[ipoly],path);
         }
      }
      return res;
   }

}
