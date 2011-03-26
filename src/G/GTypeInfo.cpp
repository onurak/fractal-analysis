#include "GTypeInfo.h"

GTypeInfo *gRegisteredTypes = NULL;

GTypeInfo* gGetFirstType()
{
    return gRegisteredTypes;
}

GTypeInfo* gGetNextType(GTypeInfo* tp)
{
    return tp ? tp->next : NULL;
}

GTypeInfo* gAddType(int id, const char *name)
{
    GTypeInfo *tp = (GTypeInfo*)malloc(sizeof(GTypeInfo));
    int strsz = strlen(name);
    tp->name = (char*)malloc((strsz+1) * sizeof(char));
    memcpy(tp->name, name, strsz);
    tp->name[strsz] = '\0';
    tp->id = id;
    tp->next = tp->prev = NULL;

    if (gRegisteredTypes != NULL)
    {
        GTypeInfo *p = gRegisteredTypes;
        while (p->next != NULL)
            p = p->next;
        p->next = tp;
        tp->prev = p;
    }
    else
        gRegisteredTypes = tp;

    return tp;
}

void gClearRegisteredTypes()
{
    GTypeInfo* tp = gRegisteredTypes;
    while (tp != NULL)
    {
        free(tp->name);
        GTypeInfo* tmp = tp;
        tp = tp->next;
        free(tmp);
    }
    gRegisteredTypes = NULL;
}

GTypeInfo* gGetType(const char *name)
{
    GTypeInfo* tp = gRegisteredTypes;
    while (tp != NULL)
    {
        if (strcmp(tp->name, name) == 0)
            return tp;
        else
            tp = tp->next;
    }
    return NULL;
}

GTypeInfo* gGetType(int id)
{
    GTypeInfo* tp = gRegisteredTypes;
    while (tp != NULL)
    {
        if (tp->id == id)
            return tp;
        else
            tp = tp->next;
    }
    return NULL;
}
