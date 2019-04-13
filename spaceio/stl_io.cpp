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

#include "stl_io.h"
#include "spacemath/vec3d.h"
#include <boost/filesystem.hpp>

#include <locale>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <cstdio>
#include <stdint.h>


namespace spaceio {

using namespace std;


bool stl_io::is_stl(const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);
   std::string ext = fullpath.extension().string();
   std::transform(ext.begin(),ext.end(),ext.begin(),::tolower);
   if(ext == ".stl")return true;

   return false;
}

std::string stl_io::write(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path, bool binary)
{
   if(binary)return write_binary(polyset,file_path);
   return write_ascii(polyset,file_path);
}

std::shared_ptr<ph3d_vector> stl_io::read(const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);
   if(!is_stl(file_path)) {
      string message = "stl_io::read(...)  The file must be .stl: " + file_path;
      throw logic_error(message);
   }

   if(!boost::filesystem::exists(fullpath)) {
      string message = "stl_io::read(...) The file does not exist: " + file_path;
      throw logic_error(message);
   }


   // open the file as binary and read the 5 first bytes to see if it says "solid", meaning ascii format
   if(FILE* stl = std::fopen(file_path.c_str(),"rb")) {
      // read the header bytes
      const size_t blen=80;
      char buffer[blen];
      size_t nread = std::fread(buffer,sizeof(char),5,stl);
      std::fclose(stl);

      bool is_ascii = false;

      buffer[5]=0;
      string keyword(buffer);
      if(keyword=="solid") {
         // this is *probably* an ascii file, but some software break the rules....
         // skip the header and see if we find the word "facet" in line 2
         ifstream in(file_path.c_str());
         string line;
         std::getline(in,line);
         if(!in.bad() && !in.fail() && !in.eof()) {
            std::getline(in,line);
            size_t n = line.find_first_not_of(" \t");
            if(n != string::npos) {
               if(line.substr(n,5)=="facet")is_ascii=true;
            }
         }
      }

      if(is_ascii) return read_ascii(file_path);
      else         return read_binary(file_path);
   }

   string message = "stl_io::read(...)  Failed to open: " + file_path;
   throw logic_error(message);
}

std::shared_ptr<ph3d_vector> stl_io::read_binary(const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);

   if(!is_stl(file_path)) {
      string message = "stl_io::read_binary(...)  The file must be .stl: " + file_path;
      throw logic_error(message);
   }

   if(!boost::filesystem::exists(fullpath)) {
      string message = "stl_io::read_binary(...) The file does not exist: " + file_path;
      throw logic_error(message);
   }


   std::shared_ptr<ph3d_vector> polyset(new ph3d_vector);

   if(FILE* stl = std::fopen(file_path.c_str(),"rb")) {

      // temporary storage of facets
      map<size_t,vector<pos3d>> facets;

      // read the header
      const size_t blen=80;
      char buffer[blen];
      size_t nread = std::fread(buffer,sizeof(char),blen,stl);

      // read number of triangles
      uint32_t ntri=0;
      nread = std::fread(&ntri,sizeof(uint32_t),1,stl);


      // process triangle facets
      for(uint32_t i=0; i<ntri; i++) {

         vector<pos3d> facet;
         facet.reserve(3);

         // read the triangle , skip the normal vector
         for(size_t i=0; i<4; i++) {
            float xyz[] = {0.0f, 0.0f, 0.0f };
            nread = std::fread(xyz,sizeof(float),3,stl);
            if(i>0) {
               // convert to double
               facet.push_back(pos3d(double(xyz[0]),double(xyz[1]),double(xyz[2])));
            }
         }

         // skip the attribute byte count value
         uint16_t bcount = 0;
         nread = std::fread(&bcount, sizeof(bcount), 1, stl);

         facets.insert(make_pair(facets.size(),facet));
      }
      std::fclose(stl);

      // produce vertices and faces, with no sharing of vertices at all
      // There are simply 3 times as many vertices as faces
      vtx_vec vert;
      vert.reserve(facets.size()*3);

      pface_vec faces;
      faces.reserve(facets.size());

      // traverse facets and create triangle faces
      for(auto& p : facets) {

         // face number and coordinates
         size_t iface         = p.first;
         vector<pos3d>& coord = p.second;

         // push coordinates to vertex vector and set computed face vertex indices
         pface face(3);
         for(size_t i=0; i<3; i++) {
            vert.push_back(coord[i]);
            face[i] = iface*3 + i;
         }

         // push the new face
         faces.push_back(face);
      }
      // create the completely disconnected  polyhedron
      polyset->push_back(std::shared_ptr<polyhedron3d>(new polyhedron3d(vert,faces)));

      return polyset;
   }

   string message = "stl_io::read_binary(...)  Failed to open: " + file_path;
   throw logic_error(message);
}

std::shared_ptr<ph3d_vector> stl_io::read_ascii(const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);

   if(!is_stl(file_path)) {
      string message = "stl_io::read_ascii(...)  The file must be .stl: " + file_path;
      throw logic_error(message);
   }

   if(!boost::filesystem::exists(fullpath)) {
      string message = "stl_io::read_ascii(...) The file does not exist: " + file_path;
      throw logic_error(message);
   }

   std::shared_ptr<ph3d_vector> polyset(new ph3d_vector);

   ifstream stl(file_path);
   if(stl.is_open()) {

      map<size_t,vector<pos3d>> facets;

      string line;
      while(std::getline(stl,line)) {
         string keyword;
         istringstream in(line);

         // here we read facet by facet and insert into facets map
         in >> keyword;
         if(keyword == "facet") {

            vector<pos3d> facet = get_facet_ascii(stl);
            facets.insert(make_pair(facets.size(),facet));
         }
      }

      // produce vertices and faces, with no sharing of vertices at all
      // There are simply 3 times as many vertices as faces
      vtx_vec vert;
      vert.reserve(facets.size()*3);

      pface_vec faces;
      faces.reserve(facets.size());

      // traerse facets and create triangle faces
      for(auto& p : facets) {

         // face number and coordinates
         size_t iface         = p.first;
         vector<pos3d>& coord = p.second;

         // push coordinates to vertex vector and set computed face vertex indices
         pface face(3);
         for(size_t i=0; i<3; i++) {
            vert.push_back(coord[i]);
            face[i] = iface*3 + i;
         }

         // push the new face
         faces.push_back(face);
      }

      // create the completely disconnected  polyhedron
      polyset->push_back(std::shared_ptr<polyhedron3d>(new polyhedron3d(vert,faces)));
   }
   else {
      string message = "stl_io::read_ascii(...)  Failed to open: " + file_path;
      throw logic_error(message);
   }

   return polyset;
}

vector<pos3d> stl_io::get_facet_ascii(std::istream& stl)
{
   // read a single stl facet
   vector<pos3d> vtx;
   vtx.reserve(3);
   string line;

   std::getline(stl,line);  // skip "outer loop"

   for(size_t i=0; i<3; i++) {

      std::getline(stl,line);
      istringstream in(line);

      string keyword;
      in >> keyword;

      double x(0.0),y(0.0),z(0.0);
      if(keyword == "vertex") in >> x >> y >> z;
      else throw logic_error("stl_io::get_facet_ascii(...) expected 'vertex' , but found" + keyword);
      vtx.push_back(pos3d(x,y,z));
   }

   std::getline(stl,line);  // skip "endloop"

   // read & check endfacet
   std::getline(stl,line);
   istringstream in(line);
   string keyword;
   in >> keyword;
   if(keyword != "endfacet") throw logic_error("stl_io::get_facet_ascii(...) expected 'endfacet' , but found " + keyword);

   return std::move(vtx);
}


std::string  stl_io::write_ascii(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);
   boost::filesystem::path amf_path = fullpath.parent_path() / fullpath.stem();
   std::string path = amf_path.string() + ".stl";

   ofstream out(path);
   if(out.is_open()) {

      out << "solid polyfix" << endl;

      for(size_t ipoly=0; ipoly<polyset->size(); ipoly++) {
         std::shared_ptr<polyhedron3d> poly = (*polyset)[ipoly];
         for(size_t iface=0; iface<poly->face_size(); iface++) {

            const pface& face = poly->face(iface);
            size_t nv = face.size();
            if(nv > 2) {

               // compute face normal
               vec3d normal =  poly->face_normal(iface);
               normal.normalise();

               // facet normal does not require high precision, it is usually ignored, so we save some space instead
               out << "facet normal "
                                 << std::setprecision(8) << normal.x() << ' '
                                 << std::setprecision(8) << normal.y() << ' '
                                 << std::setprecision(8) << normal.z() << endl;

               out << "   outer loop" << endl;
               for(size_t iv=0;iv<nv;iv++) {
                  const pos3d& p = poly->vertex(face[iv]);
                  out << "      vertex "
                                 << std::setprecision(16) << p.x() << ' '
                                 << std::setprecision(16) << p.y() << ' '
                                 << std::setprecision(16) << p.z() << endl;
               }
               out << "   endloop" << endl;
               out << "endfacet" << endl;
            }
         }
      }
      out << "endsolid" << endl;
   }
   else {
      string message = "stl_io::write_ascii(...)  Failed to open: " + file_path;
      throw logic_error(message);
   }

   return path;
}


std::string  stl_io::write_binary(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);
   boost::filesystem::path stl_path = fullpath.parent_path() / fullpath.stem();
   std::string path = stl_path.string() + ".stl";

   if(FILE* stl = std::fopen(path.c_str(),"wb")) {

      // write the header
      const size_t blen=80;
      char buffer[blen];
      for(size_t i=0; i<blen; i++) buffer[i]=' ';
      std::fwrite(buffer,sizeof(char),blen,stl);

      // write number of triangles
      uint32_t ntri=0;
      for(size_t ipoly=0; ipoly<polyset->size(); ipoly++) {
         std::shared_ptr<polyhedron3d> poly = (*polyset)[ipoly];
         ntri += static_cast<uint32_t>(poly->face_size());
      }
      std::fwrite(&ntri,sizeof(uint32_t),1,stl);

      for(size_t ipoly=0; ipoly<polyset->size(); ipoly++) {

         std::shared_ptr<polyhedron3d> poly = (*polyset)[ipoly];
         for(size_t iface=0; iface<poly->face_size(); iface++) {

            const pface& face = poly->face(iface);
            size_t nv = face.size();

            // compute face normal
            vec3d normal =  poly->face_normal(iface);
            normal.normalise();

            // we write regardless of area here, because we didn't check the areas when we computed the number of triangles
            float xyz[] = {static_cast<float>(normal.x()), static_cast<float>(normal.y()),  static_cast<float>(normal.z()) };
            std::fwrite(xyz,sizeof(float),3,stl);

            for(size_t iv=0;iv<nv;iv++) {
               const pos3d& p = poly->vertex(face[iv]);
               float xyz[] = {static_cast<float>(p.x()), static_cast<float>(p.y()),  static_cast<float>(p.z()) };
               std::fwrite(xyz,sizeof(float),3,stl);
            }

            // write the attribute byte count value
            uint16_t bcount = 0;
            std::fwrite(&bcount, sizeof(bcount), 1, stl);
         }
      }

      std::fclose(stl);
   }
   else {
      string message = "stl_io::write_binary(...)  Failed to open: " + file_path;
      throw logic_error(message);
   }
   return path;
}

} // namespace spaceio
