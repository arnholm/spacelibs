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

#ifndef OFF_IO_H
#define OFF_IO_H

#include "spaceio_config.h"

#include "spacemath/polyhedron3d.h"
#include <memory>
#include <string>

namespace spaceio {

   using namespace spacemath;

   // Geomview Object File Format support
   class SPACEIO_PUBLIC off_io {
   public:
      // determine if the file extension indicates an OFF file
      static bool is_off(const std::string& file_path);

      off_io();
      virtual ~off_io();

      // read from OFF, return as vector of polyhedra, one per object. For OFF this will always be max=1
      static std::shared_ptr<ph3d_vector> read(const std::string& file_path);

      // Write to OFF, return the path to the file created
      static std::string  write(std::shared_ptr<ph3d_vector> polyset, const std::string& file_path);

      // write to OFF, return the path to the file created
      // input is full path to file, file extension will be replaced to ".obj"
      static std::string  write(std::shared_ptr<spacemath::polyhedron3d> poly, const std::string& file_path);

   };

}

#endif // OFF_IO_H

