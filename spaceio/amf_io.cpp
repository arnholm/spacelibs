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
   
#include "amf_io.h"
#include "xml_tree.h"
#include <ctime>

#include <boost/filesystem.hpp>

namespace spaceio {

bool amf_io::is_amf(const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);
   std::string ext = fullpath.extension().string();
   std::transform(ext.begin(),ext.end(),ext.begin(),::tolower);
   if(ext == ".amf")return true;

   return false;
}

std::shared_ptr<ph3d_vector> amf_io::read(const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);

   if(!is_amf(file_path)) {
      string message = "amf_io::read(...)  The file must be .amf: " + file_path;
      throw logic_error(message);
   }

   if(!boost::filesystem::exists(fullpath)) {
      string message = "amf_io::read(...) The file does not exist: " + file_path;
      throw logic_error(message);
   }

   std::shared_ptr<ph3d_vector> polyset(new ph3d_vector);

   xml_tree tree;
   if(tree.read_xml(file_path)) {
      xml_node root;
      if(tree.get_root(root)) {
         if("amf" == root.tag()) {

            double unit_factor = 1.0;
            std::string unit = root.get_property("unit","millimeter");
            if( (unit == "inch") || (unit == "inches")) unit_factor = 25.4;

            for(auto i=root.begin(); i!=root.end(); i++) {
               xml_node node(i);
               if(node.tag() == "object") {
                   size_t file_id = node.get_property("id",-1);
                   std::shared_ptr<polyhedron3d> poly = read_object(node,unit_factor);
                   if(poly.get()) {
                      polyset->push_back(poly);
                   }
               }
            }
         }
         else {
            string message = "amf_io::read(...) expected <amf> tag, but found : <" + root.tag() +"> , file :" + file_path;
            throw logic_error(message);
         }
      }
      else {
         string message = "amf_io::read(...) error reading XML data, no root: " + file_path;
         throw logic_error(message);
      }
   }

   return polyset;
}


std::shared_ptr<polyhedron3d> amf_io::read_object(xml_node& parent, double unit_factor)
{
   vtx_vec   vert;
   pface_vec faces;

   // reserve some, does not matter if the number isn't right
   vert.reserve(1024);
   faces.reserve(1024);

   xml_node mesh;
   if(parent.get_child("mesh",mesh)) {
      for(auto imesh=mesh.begin(); imesh!=mesh.end(); imesh++) {
         xml_node node(imesh);
         if(node.tag() == "vertices") read_vertices(node,vert);
         if(node.tag() == "volume")   read_faces(node,faces);
      }
   }
   else {
      string message = "amf_io::read_object(...) expected <mesh> tag not found.";
      throw logic_error(message);
   }

   if(vert.size()==0 || faces.size()==0) {
      throw logic_error("amf_io::read_object(...) zero vertices or faces found");
   }

   return std::shared_ptr<polyhedron3d>(new polyhedron3d(vert,faces));
}

bool amf_io::read_vertices(xml_node& node, vtx_vec& vert)
{
   for(auto i=node.begin(); i!=node.end(); i++) {
      xml_node child(i);
      if(child.tag() == "vertex") {
         xml_node coord;
         bool ok_vertex = false;
         if(child.get_child("coordinates",coord)) {
            xml_node x,y,z;
            if(coord.get_child("x",x) && coord.get_child("y",y) && coord.get_child("z",z)) {
               vert.push_back(pos3d(x.get_value(0.0),y.get_value(0.0),z.get_value(0.0)));
               ok_vertex = true;
            }
         }
         if(!ok_vertex) {
            string message = "amf_io::read_vertices(...): error reading vertex with index = " + std::to_string(vert.size());
            throw logic_error(message);
         }
      }
   }
   return true;
}

bool amf_io::read_faces(xml_node& node, pface_vec& faces)
{
   for(auto i=node.begin(); i!=node.end(); i++) {
      xml_node child(i);
      if(child.tag() == "triangle") {
         bool ok_triangle = false;
         xml_node v1,v2,v3;
         if(child.get_child("v1",v1) && child.get_child("v2",v2) && child.get_child("v3",v3)) {

            pface face;
            face.reserve(3);
            face.push_back(v1.get_value(0));
            face.push_back(v2.get_value(0));
            face.push_back(v3.get_value(0));
            faces.push_back(face);

            ok_triangle = true;
         }
         if(!ok_triangle) {
            string message = "amf_io::read_faces(...) error reading triangle with index = " + std::to_string(faces.size());
            throw logic_error(message);
         }
      }
   }
   return true;
}

std::string  amf_io::write(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path)
{
   // ISO8601 date and time string of current time
   time_t now = time(0);
   const size_t blen = 80;
   char buffer[blen];
   strftime(buffer,blen,"%Y-%m-%dT%H:%M:%S",gmtime(&now));
   string iso8601(buffer);

   boost::filesystem::path fullpath(file_path);
   boost::filesystem::path amf_path = fullpath.parent_path() / fullpath.stem();
   std::string path = amf_path.string() + ".amf";

   // fix inconsistent slashes to something that is consistent and works everytwhere
   std::replace(path.begin(),path.end(), '\\', '/');

   xml_tree tree;
   if(tree.create_root("amf")) {
      xml_node root;
      if(tree.get_root(root)) {
         root.add_property("unit","millimeter");

         // add some metadata
         xml_node name      = root.add_child("metadata"); name.add_property("type","name");         name.put_value(fullpath.stem().string());
         xml_node date_time = root.add_child("metadata"); date_time.add_property("type","created"); date_time.put_value(iso8601);

         // write one amf "object" per polyhedron
         for(size_t ipoly=0; ipoly<polyset->size(); ipoly++) {
            write_object(root,polyset,ipoly);
         }
         tree.write_xml(path);
      }
   }

   return path;
}

bool amf_io::write_object(xml_node& parent, std::shared_ptr<ph3d_vector> polyset, size_t index)
{
   shared_ptr<polyhedron3d> poly = (*polyset)[index];

   xml_node object = parent.add_child("object");
   object.add_property("id",index);
   xml_node mesh = object.add_child("mesh");

   xml_node vertices = mesh.add_child("vertices");
   for(size_t ivert=0; ivert<poly->vertex_size(); ivert++) {

      const pos3d& pos = poly->vertex(ivert);
      xml_node coordinates = vertices.add_child("vertex").add_child("coordinates");
      coordinates.add_child("x").put_value(pos.x());
      coordinates.add_child("y").put_value(pos.y());
      coordinates.add_child("z").put_value(pos.z());
   }

   xml_node volume = mesh.add_child("volume");
   const string vtags[] = { "v1", "v2", "v3" } ;

   for(size_t iface = 0; iface<poly->face_size(); ++iface) {

      const pface& face = poly->face(iface);
      size_t nvert = face.size();
      if(nvert != 3)throw std::logic_error("amf_io::write_object, non-triangular face encountered!");

      xml_node triangle = volume.add_child("triangle");
      for(size_t ivert=0; ivert<nvert; ivert++) {
         size_t index = face[ivert];
         triangle.add_child(vtags[ivert]).put_value(index);
      }
   }

   return true;
}

std::shared_ptr<polyhedron3d> amf_io::merge_lumps(std::shared_ptr<ph3d_vector> lumps)
{
   // count number of vertices and faces
   size_t nvert = 0;
   size_t nface = 0;

   for(size_t ipoly=0; ipoly<lumps->size(); ipoly++) {
      std::shared_ptr<polyhedron3d> poly =  (*lumps)[ipoly];
      nvert += poly->vertex_size();
      nface += poly->face_size();
   }

   // allocate space for the merged polyhedron
   vtx_vec   vert;  vert.reserve(nvert);
   pface_vec faces; faces.reserve(nface);

   // fill the merged polyhedron
   size_t iv_offset = 0;
   for(size_t ipoly=0; ipoly<lumps->size(); ipoly++) {

      // get the lump
      std::shared_ptr<polyhedron3d> poly =  (*lumps)[ipoly];
      size_t nv = poly->vertex_size();
      size_t nf = poly->face_size();

      // copy the vertices
      for(size_t iv=0; iv<nv; iv++) {
         vert.push_back(poly->vertex(iv));
      }

      // copy the faces, offsetting the vertex indices
      for(size_t iface=0; iface<nf; iface++) {
         pface face = poly->face(iface);
         size_t nfv = face.size();
         for(size_t ifv=0; ifv<nfv; ifv++) {
            face[ifv] += iv_offset;
         }
         faces.push_back(face);
      }

      // increase the vertex offset
      iv_offset += nv;
   }

   // finished merged vertex
   return std::make_shared<polyhedron3d>(polyhedron3d(vert,faces));
}


} // namespace spaceio
