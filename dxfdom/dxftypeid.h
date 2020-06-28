#ifndef DXF_TYPEID_H
#define DXF_TYPEID_H

#include <string>
#include <typeinfo>

/// return typeid string for any given class type_info
std::string dxfclassname(const std::type_info& type);

/// return typeid string for any given class (template parameter)
template<class T> std::string dxftypeid() { return dxfclassname(typeid(T)); }
template<class T> std::string dxftypeid(const T* object) { return dxfclassname(typeid(*object)); }

#endif // DXF_TYPEID_H
