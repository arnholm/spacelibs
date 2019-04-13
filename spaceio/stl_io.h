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

#ifndef STL_IO_H
#define STL_IO_H

#include "spaceio_config.h"

#include "spacemath/polyhedron3d.h"
#include <memory>
#include <string>
#include <iostream>

namespace spaceio {

using namespace spacemath;

// STL file format (ASCII/binary)
class SPACEIO_PUBLIC stl_io {
public:
   // determine if the file extension indicates an STL file
   static bool is_stl(const std::string& file_path);

   // determine binary/ascii and read as required
   static std::shared_ptr<ph3d_vector> read(const std::string& file_path);

   // write binary by default
   static std::string write(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path, bool binary = true);

protected:

   // read from binary STL, return as vector of polyhedra
   static std::shared_ptr<ph3d_vector> read_binary(const std::string& file_path);

   // read from ASCII STL, return as vector of polyhedra
   static std::shared_ptr<ph3d_vector> read_ascii(const std::string& file_path);
   static std::vector<pos3d> get_facet_ascii(std::istream& stl);

   // write to ASCII STL, return the path to the file created
   // input is full path to file, file extension will be replaced to ".stl"
   static std::string  write_ascii(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path);

   // write to binary STL, return the path to the file created
   // input is full path to file, file extension will be replaced to ".stl"
   static std::string  write_binary(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path);

};

} // namespace spaceio

#endif // STL_IO_H
