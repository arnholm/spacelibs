#ifndef DXFPROFILE_H
#define DXFPROFILE_H

#include <ostream>
#include <cstddef>
#include <memory>
#include "dxfdom_config.h"

#include "spaceio/xml_node.h"
using namespace spaceio;

#include <list>

#include "dxfposmap.h"
#include "dxfcurve.h"
#include "dxfnode.h"
#include "dxfxmloptions.h"
class dxfblock;
class dxfloop;

// dxfprofile is returned from dxfroot
// dxfprofile may be exported to other formats using dxfprofileexport
class DXFDOM_PUBLIC dxfprofile {
public:
   typedef std::list<std::shared_ptr<dxfloop>> Profile;
   typedef Profile::const_iterator const_iterator;

   // construct with coordinate and secant tolerance
   dxfprofile(const dxfxmloptions& opt);
   virtual ~dxfprofile();

   // tolerances
   double epspnt() const { return m_opt.epspnt(); }
   double sectol() const { return m_opt.sectol(); }

   // return posmap
   dxfposmap<size_t>& pm() { return m_pm; }

   // add a dxfblock for later reference by dxfinsert
   void insert_block(std::shared_ptr<dxfblock> block);

   // look up named block (may return null)
   std::shared_ptr<dxfblock> get_block(const string& block_name) const;

   // push_back is called by dxf entities to add their data
   void push_back(std::shared_ptr<dxfcurve> curve);

   // builds complete profile after all curves pushed
   void build_loops();

   // iterate over loops in finished profile
   const_iterator begin() const { return m_prof.begin(); }
   const_iterator end() const   { return m_prof.end(); }
   size_t         size() const  { return m_prof.size(); }

   // export to XML
   bool to_xml(xml_node& xml_this) const;

protected:
   dxfnode& node(size_t inode);

   typedef std::map<size_t,std::shared_ptr<dxfcurve>> pcurve_map;

   void build_loop(pcurve_map& pcurves);
   void auto_close(pcurve_map& pcurves);
   void orient_loops();

private:
   dxfxmloptions m_opt;
   Profile       m_prof;          // final computed profile with N loops

private: // temporary data structures used in build_contour()

   typedef std::map<std::string,std::shared_ptr<dxfblock>> BlockMap;
   BlockMap                             m_blocks;

   typedef std::map<size_t,dxfnode> NodeMap;
   std::list<std::shared_ptr<dxfcurve>> m_curves;        // initial curves generated from dxf entities
   dxfposmap<size_t>                    m_pm;            // for coordinate matching
   NodeMap                              m_nodemap;       // table of nodes (created after all curves added)

};

#endif // DXFPROFILE_H
