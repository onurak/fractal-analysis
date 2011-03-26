#ifndef GTYPEINFO_H
#define GTYPEINFO_H

#include <stdlib.h>
#include <string.h>
#include <typeinfo>
#include "../../projects/GTestDLL/GTestDLL_global.h"

struct GTypeInfo
{
public:
    char *name;
    int id;
public:
    friend GTypeInfo* gGetFirstType();
    friend GTypeInfo* gGetNextType(GTypeInfo* tp);
    friend GTypeInfo* gAddType(int id, const char *name);
    friend void gClearRegisteredTypes();
    friend GTypeInfo* gGetType(const char *name);
    friend GTypeInfo* gGetType(int id);
private:
    GTypeInfo *next, *prev;
};

GTESTDLLSHARED_EXPORT GTypeInfo* gGetFirstType();
GTESTDLLSHARED_EXPORT GTypeInfo* gGetNextType(GTypeInfo* tp);
GTESTDLLSHARED_EXPORT GTypeInfo* gAddType(int id, const char *name);
GTESTDLLSHARED_EXPORT void gClearRegisteredTypes();
GTESTDLLSHARED_EXPORT GTypeInfo* gGetType(const char *name);
GTESTDLLSHARED_EXPORT GTypeInfo* gGetType(int id);

/*! \fn GTypeIdFn
  * \brief Get unique ID of type
  *
  * Yeah, it's a kind of magic.
  * At least this should work at GNU c++ compiler:
  *
  * and Microsoft c++ compiler:
  * http://msdn.microsoft.com/en-us/library/7k8twfx7(v=vs.80).aspx
  */
template <typename T>
int GTypeIdT(const char *typeName)
{
    GTypeInfo *tp = gGetType(typeName);
    if (tp == NULL)
    {
        int (*fp)(const char*) = &GTypeIdT<T>;
        int id = (int)fp;
        gAddType(id, typeName);
        return id;
    }
    else
        return tp->id;
}


#define GDeclareType(type) \
    GTypeIdT<type>(#type)

/*! \macro GTypeId
  * \brief Interface for getting unique ID of type
  */
#define GTypeId(type) \
    GTypeIdT<type>(#type)
    //GTypeIdT<type>(typeid(type).name()

#define GTypeName(type) \
    (GTypeIdT<type>(#type), gGetType(#type)->name)
    //(GTypeIdT<type>(typeid(type).name()), gGetType(typeid(type).name())->name)


#endif // GTYPEINFO_H
