#include "dxfprofile.h"
#include <iomanip>
#include <iostream>
#include <string>

#include "dxfnode.h"
#include "dxfloop.h"
#include "dxfblock.h"

#include "clipper_csg/clipper.hpp"

dxfprofile::dxfprofile(const dxfxmloptions& opt)
: m_opt(opt)
{
   m_pm.set_tolerance(opt.epspnt());
}

dxfprofile::~dxfprofile()
{}

void dxfprofile::insert_block(std::shared_ptr<dxfblock> block)
{
   m_blocks[block->name()] = block;
}

std::shared_ptr<dxfblock> dxfprofile::get_block(const string& block_name) const
{
   auto i = m_blocks.find(block_name);
   if(i==m_blocks.end()) return nullptr;

   return i->second;
}


void dxfprofile::push_back(std::shared_ptr<dxfcurve> curve)
{
   m_curves.push_back(curve);
   curve->set_id(m_curves.size());
}

dxfnode& dxfprofile::node(size_t inode)
{
   auto i = m_nodemap.find(inode);
   if(i == m_nodemap.end()) throw std::logic_error("dxfprofile: no such node: " + std::to_string(inode));
   return i->second;
}


void dxfprofile::build_loops()
{
   m_prof.clear();

   // build the nodemap
   m_nodemap.clear();
   for(auto& pmap : m_pm) {
      const std::pair<dxfpos,size_t>& p = pmap.second;
      m_nodemap.insert(std::make_pair(p.second,p.first));
   }

   set<size_t> duplicates;

   // update the nodemap with curve back pointers
   // also build the set of curves to pick from during profile resolution
   pcurve_map pcurves;

   for(auto& c : m_curves) {
      if(duplicates.find(c->id_nodes()) == duplicates.end()) {

         dxfnode& n1 = node(c->n1());
         n1.insert(c);

         dxfnode& n2 = node(c->n2());
         n2.insert(c);

         pcurves[c->id()] = c;

         if(c->is_closed()) {
            // consider only closed curves for duplicate check
            duplicates.insert(c->id_nodes());
         }
      }
      else {
         cout << " Warning: Skipped duplicated curve with end nodes " << c->n1() << " " << c->n2() << endl;
      }
   }

   cout << " Starting from " << pcurves.size() << " curves" << endl;

   // Build the contours by pulling curves from pcurves
   // and traversing the topology.
   // At the end of this stage, all loops will be CCW.
   while(pcurves.size() > 0) {
      build_loop(pcurves);
   }

   // finally orient the loops so that holes run CW
   orient_loops();
   cout << " Profile completed with " << m_prof.size() <<  ( (m_prof.size()==1) ? " loop":" loops") << endl;

   // loops created
   // clear the temporary curves, nodemap and position map
   m_curves.clear();
   m_nodemap.clear();
   m_pm.clear();

}


void dxfprofile::build_loop(pcurve_map& pcurves)
{
   size_t ncurves = 0;
   set<size_t> visited;

   auto c1 = pcurves.begin()->second;

   // the end nodes on this curve
   size_t n1         = c1->n1();
   size_t inode_next = c1->n2();

   // remove curve from pcurves
   pcurves.erase(c1->id());
   visited.insert(c1->id());

   bool open_curve = (n1 != inode_next) ;

   dxfnode& node1 = node(n1);
   dxfnode& node2 = node(inode_next);
   if(open_curve && node1.manifold() != 2) {
      const dxfpos& p = node1.pos();
      cout << " warning: non-manifold (" << node1.manifold() << ") point at [" <<  setw(10) << p.x() << ',' << setw(10) << p.y() << "] id=" << setw(4) << inode_next << ", skipping curve " << c1->id() << endl;

      pcurves.erase(c1->id());
      // erase curve from nodes
      node1.erase(c1);
      node2.erase(c1);
      return;
   }

   // create the new loop
   m_prof.push_back(make_shared<dxfloop>());
   shared_ptr<dxfloop> loop = m_prof.back();

   // push first curve to loop, forward direction
   loop->push_back(node1.pos());
   loop->push_back(c1->internal_points());

   ncurves++;

   // loop over connected curves until node n1 is seen again
   auto c = c1;
   while(inode_next != n1) {
      // add more curves to the loop

      // get next curve at other end
      dxfnode& node_next = node(inode_next);
      if(node_next.manifold()!=2  || pcurves.size()==0){

         // erase curve from node
         node_next.erase(c);

         const dxfpos& p = node_next.pos();
         cout << " warning: non-manifold (" << node_next.manifold() << ") point at [" <<  setw(10) << p.x() << ',' << setw(10) << p.y() << "] id=" << setw(4) << inode_next << ", skipping curve " << c->id() << endl;

         // skip this loop
         m_prof.pop_back();
         return;
      }

      // get next curve in loop
      c = node_next.next_curve(c);

      if(visited.find(c->id()) != visited.end()) {
         cout << " warning: Skipped loop, because this curve was seen before: " << c->id() << endl;
         // skip this loop
         m_prof.pop_back();
         return;
      }



      // remove curve from pcurves
      pcurves.erase(c->id());
      visited.insert(c->id());

      // push first position on curve
      dxfnode& next_node = node(inode_next);
      loop->push_back(next_node.pos());
      std::list<dxfpos> points = c->internal_points();
      ncurves++;

      if(c->n1() == inode_next) {

         // push internal points in forward direction
         loop->push_back(points);
         inode_next = c->n2();
      }
      else if(c->n2() == inode_next) {
         // push internal points in reverse direction
         points.reverse();
         loop->push_back(points);
         inode_next = c->n1();
      }
      else {
         throw std::logic_error("dxfprofile::build_loop error, node not connected");
      }
   }

   // make sure there are no subsequent duplicate points
   loop->erase_duplicates(m_opt.epspnt());

   // at this stage, we don't know which winding order is suitable. It will be resolved later.
   // so we just make sure the winding order is all CCW for now
   loop->canonicalize();

   cout << " Created loop from " << ncurves << " curve(s). " << pcurves.size() <<" curves remaining" << endl;

}


void dxfprofile::orient_loops()
{
   const ClipperLib::cInt TO_CLIPPER = (1 << 16);

   // use clipper to do the orientation

   // convert profile to clipper representation
   ClipperLib::Paths paths;
   paths.reserve(m_prof.size());
   for(auto& loop : m_prof) {
      ClipperLib::Path path;
      path.reserve(loop->size());
      for(const dxfpos& p : *loop) {
         path.push_back(ClipperLib::IntPoint(ClipperLib::cInt(p.x()*TO_CLIPPER),ClipperLib::cInt(p.y()*TO_CLIPPER)));
      }
      paths.push_back(path);
   }


   // then run clipper on the profile to orient the loops
   ClipperLib::Paths   result;
   ClipperLib::Clipper clipper;
   clipper.AddPaths(paths,ClipperLib::ptSubject,true);
   clipper.Execute(ClipperLib::ctUnion, result, ClipperLib::pftEvenOdd,ClipperLib::pftEvenOdd);

   // sort the aligned loops according to area
   multimap<double,std::shared_ptr<dxfloop>> sorted_loops;
   for(size_t ipath=0;ipath<result.size(); ipath++) {
      ClipperLib::Path& path = result[ipath];

      std::shared_ptr<dxfloop> loop = std::make_shared<dxfloop>();

      for(size_t ipoint=0; ipoint<path.size(); ipoint++) {
         const ClipperLib::IntPoint& p = path[ipoint];
         double x = double(p.X)/TO_CLIPPER;
         double y = double(p.Y)/TO_CLIPPER;
         loop->push_back(dxfpos(x,y,0));
      }

      double area = fabs(loop->signed_area());
      if(area > 0) {
         sorted_loops.insert(std::make_pair(1.0/area,loop));
      }
   }

   // finally recreate the profile with aligned and sorted loops
   m_prof.clear();
   for(auto& p : sorted_loops) {
      m_prof.push_back(p.second);
   }

}


bool dxfprofile::to_xml(xml_node& xml_this) const
{
   for(auto& loop : m_prof) {
      xml_node xml_loop = xml_this.add_child("loop");
      xml_loop.add_property("signed_area",loop->signed_area());
      for(auto& pos : *loop) {
         xml_node xml_vertex = xml_loop.add_child("vertex");
         xml_vertex.add_property("x",pos.x());
         xml_vertex.add_property("y",pos.y());
      }
   }
   return true;
}

