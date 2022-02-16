#ifndef POLYHEDRON_IO_H
#define POLYHEDRON_IO_H

#include "spaceio_config.h"
#include "spacemath/polyhedron3d.h"
#include <memory>
#include <string>


namespace spaceio {

   using namespace spacemath;

   class SPACEIO_PUBLIC polyhedron_io {
   public:

      // read from file, return as vector of polyhedra, return nullptr if not supported
      static std::shared_ptr<ph3d_vector> read(const std::string& file_path);

   };

}

#endif // POLYHEDRON_IO_H
