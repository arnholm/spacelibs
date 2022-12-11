#ifndef DXFINSERT_H
#define DXFINSERT_H

#include "dxfentity.h"
#include "dxfpos.h"



class DXFDOM_PUBLIC dxfinsert : public dxfentity {
public:
   dxfinsert(shared_ptr<dxfitem> item, const dxfxmloptions& opt);
   virtual ~dxfinsert();

   virtual string tag() const { return "INSERT"; }

   virtual bool to_xml(xml_node& xml_this) const;

   virtual void push_profile(dxfprofile& prof, const HTmatrix& T) const;

   HTmatrix get_matrix() const;

private:
   string  m_name;   // BLOCK name of block to be inserted
   dxfpos  m_pos;    // "insertion point" (please define ...)
   dxfpos  m_scale;  // scale factors for x,y and z
   double  m_ang;    // rotation angle [deg]
   int     m_nrow;   // "Number of rows"
   double  m_srow;   // row spacing
   int     m_ncol;   // "number of columns"
   double  m_scol;   // column spacing
   dxfpos  m_normal; // plane normal
};

#endif // DXFINSERT_H
