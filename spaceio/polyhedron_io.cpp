#include "polyhedron_io.h"

#include "amf_io.h"
#include "obj_io.h"
#include "off_io.h"
#include "stl_io.h"

namespace spaceio {

   std::shared_ptr<ph3d_vector> polyhedron_io::read(const std::string& file_path)
   {
      if(amf_io::is_amf(file_path)) return amf_io::read(file_path);
      if(obj_io::is_obj(file_path)) return obj_io::read(file_path);
      if(off_io::is_off(file_path)) return off_io::read(file_path);
      if(stl_io::is_stl(file_path)) return stl_io::read(file_path);
      return nullptr;
   }
}
