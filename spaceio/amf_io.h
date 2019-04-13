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

#ifndef AMF_IO_H
#define AMF_IO_H

#include "spaceio_config.h"

#include "spacemath/polyhedron3d.h"
#include <memory>
#include <string>


namespace spaceio {

class xml_node;
using namespace spacemath;

// Support of AMF format (uncompressed, plain XML only)
class SPACEIO_PUBLIC amf_io {
public:
   // determine if the file extension indicates an AMF file
   static bool is_amf(const std::string& file_path);

   // read from AMF, return as vector of polyhedra, one per AMF object
   static std::shared_ptr<ph3d_vector> read(const std::string& file_path);

   // write to AMF, return the path to the file created
   // input is full path to file, file extension will be replaced to ".amf"
   static std::string  write(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path);

   // merge the lumps into a single polyhedron (no boolean performed)
   static std::shared_ptr<polyhedron3d> merge_lumps(std::shared_ptr<ph3d_vector> lumps);

protected:
   static bool write_object(xml_node& parent, std::shared_ptr<ph3d_vector> polyset, size_t index);
   static std::shared_ptr<polyhedron3d> read_object(xml_node& parent, double unit_factor);
   static bool read_vertices(xml_node& node, vtx_vec& vert);
   static bool read_faces(xml_node& node, pface_vec& faces);
};

} // namespace spaceio

#endif // AMF_IO_H
