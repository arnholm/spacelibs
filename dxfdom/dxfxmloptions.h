#ifndef DFXXMLOPTIONS_H
#define DFXXMLOPTIONS_H

#include "dxfdom_config.h"
#include <string>
#include <set>

// Options for DXF import (see dxfroot)
class DXFDOM_PUBLIC dxfxmloptions {
public:
   dxfxmloptions(bool include_raw, double scale_factor, double sectol, double epspnt, const std::set<std::string>& layers, bool keep_case);
   virtual ~dxfxmloptions();

   inline bool        include_raw() const  { return m_include_raw; }
   inline double      scale_factor() const { return m_scale_factor; }
   inline double      sectol() const       { return m_sectol; }
   inline double      epspnt() const       { return m_epspnt; }
   bool layer_selected(const std::string& layer) const;
   bool auto_close() const { return m_auto_close; }

   void set_layers(const std::set<std::string>& layers) { m_layers = layers; }
   void set_auto_close(bool auto_close) { m_auto_close = auto_close; }

private:
   bool                  m_include_raw;
   double                m_scale_factor;
   double                m_sectol;        // secant tolerance for circles, arcs and ellipses
   double                m_epspnt;        // tolerance for point comparison
   std::set<std::string> m_layers;        // selected layers to convert (or empty vector to get all)
   bool                  m_keep_case;     // true if layer case to be preserved
   bool                  m_auto_close;    // default:false. Close open loops
};
#endif // DFXXMLOPTIONS_H
