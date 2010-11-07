/* Author: Vladimir Belikov
 *
 * This file is part of G Library.
 *
 * G Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * G Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with G Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GVARIANT_H
#define GVARIANT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include "GException.h"
#include "GResources.h"
#include "GLogger.h"
#include "GSettings.h"

#ifdef USING_QT
    #include <QString>
#endif

/*! \addtogroup GLib
  */
/*@{*/

//! All types that currently can store GVariant type
enum GVarType {
    G_VAR_NULL = 0,    //!< type not set
    G_VAR_INT,         //!< int
    //G_VAR_LONG,        //!< long int
    //G_VAR_SHORT,       //!< short int
    //G_VAR_CHAR,        //!< char
    G_VAR_FLOAT,       //!< float
    G_VAR_DOUBLE,      //!< double
    //G_VAR_LONG_DOUBLE, //!< long double
    G_VAR_PCHAR,       //!< char*
    G_VAR_PVOID,       //!< void*
    G_VAR_BOOL,        //!< bool
    G_VAR_CUSTOM       //!< custom user type
};

//! Value that GVariant storing
union GVarData {
    int i;
    //long int li;
    //short int si;
    //char c;
    float f;
    double d;
    //long double ld;
    char *pc;
    void *pv;
    bool b;
    void *custom;
};

//! Does string means true constant?
inline bool doesStringMeansTrue(const char* c)
{
    std::string s = lowerCase(c);
    return s == "true" || s == "yes" || s == "y";
}

const bool G_VAR_CAST_VALIDNESS_TABLE[7][7] =
{
// base type       |  new type =>
//    ||           |
//    \/           | NULL | INT | FLOAT | DOUBLE | PCHAR | PVOID | BOOL |
/* NULL   */       {   0,    0,     0,       0,      0,     0,      0   },
/* INT    */       {   0,    1,     1,       1,      1,     0,      1   },
/* FLOAT  */       {   0,    1,     1,       1,      1,     0,      1   },
/* DOUBLE */       {   0,    1,     1,       1,      1,     0,      1   },
/* PCHAR  */       {   0,    1,     1,       1,      1,     1,      1   },
/* PVOID  */       {   0,    0,     0,       0,      0,     1,      0   },
/* BOOL   */       {   0,    1,     1,       1,      1,     0,      1   }
};

#define G_VAR_UNDEFINED_OPERATION \
    { \
        GError(GCritical, \
               GVariantName, \
               G_UNDEFINED_OPERATION, \
               GVariantUndefinedOperation); \
        return GVariant(G_VAR_NULL); \
    }

#define G_VAR_INVALID_TYPECAST \
    { \
        GError(GCritical, \
               GVariantName, \
               G_INVALID_TYPECAST, \
               GVariantCantCast); \
        return; \
    }

    inline void GVarInvalidCast(const std::string &fromType, const std::string &toType)
    {
        GError(GCritical, GVariantName, G_INVALID_TYPECAST, fromType + " to " + toType);
    }

/*! \class GVariant
  * \brief Variant type
  *
  * This type emulates 'variable with dynamic type'. It can store data of few types,
  * cast between them, dynamicaly change type, construct from few types, assign values
  * of different types. In fact it can hold data of any type, but if type is nonstandart
  * (if it does not in GVarType), it will be untyped pointer type.
  *
  * In fact GVariant variable takes minimum (sizeof(GVarType) + sizeof(double)) bytes.
  * In case of string data it can take even more (size of string).
  *
  * Smart set of constructors and operators allow to mix variants with different values
  * in one expression. The result of expression will have type of the left operand:
  * \code
  * GVariant v1 = "1234", v2 = 1234, v3;
  * v3 = v1 + v2; // v3 = "12341234";
  * v3 = v2 + v1; // v3 = 2468
  * \endcode
  * It might cause errors, so if you want to be sure about operation result type use casting:
  * \code
  * v3 = int(v1) + v2; // v3 = 2468;
  * \endcode
  *
  * In case of unsupported operation (like 'minus' for two strings) GVarInvalidCast error
  * will be raised.
  *
  * Also Custom type support is coming
  */
class GVariant
{
public:
    //! Default constructor
    /*! Value of variant will be eqivalent of zero (e.g. 0, NULL)
      * \param type Type of new variant
      */
    GVariant(GVarType type = G_VAR_NULL)
    {
        m_type = type;
        m_data.i = 0;
    }

    //! Destructor
    /*! Smart memory cleanup guaranted for known types
     */
    ~GVariant()
    {
        freeVarData();
    }

    //! Copy constructor
    /*! In case of string data, it will be copied
      */
    GVariant(const GVariant &value)
    {
        if (value.m_type != G_VAR_CUSTOM)
        {
            m_type = G_VAR_NULL;
            *this = value;
        }
        else
        {
            m_type = G_VAR_CUSTOM;
            m_customDestructor = value.m_customDestructor;
            m_customCopyConstructor = value.m_customCopyConstructor;
            m_data.custom = (value.*m_customCopyConstructor)();

        }
    }

    //! Contructor for int type
    GVariant(int value)
    {
        m_type = G_VAR_INT;
        m_data.i = value;
    }

    //! Contructor for float type
    GVariant(const float &value)
    {
        m_type = G_VAR_FLOAT;
        m_data.f = value;
    }

    //! Contructor for double type
    GVariant(const double &value)
    {
        m_type = G_VAR_DOUBLE;
        m_data.d = value;
    }

    //! Contructor for char* type
    GVariant(const char *value)
    {
        m_type = G_VAR_NULL;
        *this = value;
    }

    //! Contructor for std::string type
    GVariant(const std::string &value)
    {
        m_type = G_VAR_NULL;
        *this = value.c_str();
    }

    //! Contructor for void* type
    GVariant(void *value)
    {
        m_type = G_VAR_PVOID;
        m_data.pv = value;
    }

    //! Contructor for bool type
    GVariant(bool value)
    {
        m_type = G_VAR_BOOL;
        m_data.b = value;
    }

    //! Constructor for custom type
    template <typename T>
    explicit GVariant(const T& custom)
    {
        m_type = G_VAR_CUSTOM;
        m_data.custom = new T(custom);
        m_customDestructor = &GVariant::customDelete<T>;
        m_customCopyConstructor = &GVariant::customCopy<T>;
    }

    /* Type casting section */
    //! Cast to int
    /*! Impossible for void*, could be impossible for char*
      */
    operator int() const
    {
        switch (m_type)
        {
            case G_VAR_INT:
                return m_data.i;
            case G_VAR_FLOAT:
                return m_data.f;
            case G_VAR_DOUBLE:
                return m_data.d;
            case G_VAR_PCHAR:
                return atoi(m_data.pc);
            case G_VAR_BOOL:
                return m_data.b ? 1 : 0;
            default:
                GVarInvalidCast(typeName(), typeName(G_VAR_INT));
                return 0;
        }
    }

    //! Cast to float
    /*! Impossible for void*, could be impossible for char*
      */
    operator float() const
    {
        switch (m_type)
        {
            case G_VAR_INT:
                return m_data.i;
            case G_VAR_FLOAT:
                return m_data.f;
            case G_VAR_DOUBLE:
                return m_data.d;
            case G_VAR_PCHAR:
                return atof(m_data.pc);
            case G_VAR_BOOL:
                return m_data.b ? 1 : 0;
            default:
                GVarInvalidCast(typeName(), typeName(G_VAR_FLOAT));
                return 0.0f;
        }
    }

    //! Cast to double
    /*! Impossible for void*, could be impossible for char*
      */
    operator double() const
    {
        switch (m_type)
        {
            case G_VAR_INT:
                return m_data.i;
            case G_VAR_FLOAT:
                return m_data.f;
            case G_VAR_DOUBLE:
                return m_data.d;
            case G_VAR_PCHAR:
                return atof(m_data.pc);
            case G_VAR_BOOL:
                return m_data.b ? 1 : 0;
            default:
                GVarInvalidCast(typeName(), typeName(G_VAR_DOUBLE));
                return 0.0;
        }
    }

    //! Cast to bool
    /*! Impossible for void*
      * \throws GVarInvalidCast
      */
    operator bool() const
    {
        switch (m_type)
        {
            case G_VAR_INT:
                return m_data.i;
            case G_VAR_FLOAT:
                return m_data.f;
            case G_VAR_DOUBLE:
                return m_data.d;
            case G_VAR_PCHAR:
                return doesStringMeansTrue(m_data.pc);
            case G_VAR_BOOL:
                return m_data.b;
            default:
                GVarInvalidCast(typeName(), typeName(G_VAR_BOOL));
                return false;
        }
    }

    //! Cast to char*
    /*! Impossible for void*
      * \throws GVarInvalidCast
      */
    operator char*() const
    {
        char *buf;
        switch (m_type)
        {
            case G_VAR_INT:
                buf = createStaticBuffer();
                sprintf(buf, "%i", m_data.i);
                return buf;
            case G_VAR_FLOAT:
                buf = createStaticBuffer();
                sprintf(buf, "%f", m_data.f);
                return buf;
            case G_VAR_DOUBLE:
                buf = createStaticBuffer();
                sprintf(buf, "%lf", m_data.d);
                return buf;
            case G_VAR_PCHAR:
                return createStaticBuffer((strlen(m_data.pc)+1) * sizeof(char), m_data.pc);

            case G_VAR_BOOL:
                return m_data.b ? createStaticBuffer(5 * sizeof(char), "true") :
                                  createStaticBuffer(6 * sizeof(char), "false");

            default:
                GVarInvalidCast(typeName(), typeName(G_VAR_PCHAR));
                return createStaticBuffer(sizeof(char), "");
        }
    }

    //! Cast to void*
    /*! Only possible for char* and void*
      * \throws GVarInvalidCast
      */
    operator void*() const
    {
        switch (m_type)
        {
            case G_VAR_PVOID:
                return m_data.pv;
            case G_VAR_PCHAR:
                return reinterpret_cast<void*>(m_data.pc);
            case G_VAR_CUSTOM:
                return reinterpret_cast<void*>(m_data.custom);
            default:
                GVarInvalidCast(typeName(), typeName(G_VAR_PVOID));
        }
    }

    //! Cast to custom type
    /*! Only possible for same custom type
      */
    template <class T>
    operator T*() const
    {
        switch (m_type)
        {
            case G_VAR_PVOID:
                return reinterpret_cast<T*>(m_data.pv);
            case G_VAR_CUSTOM:
                return reinterpret_cast<T*>(m_data.custom);
            default:
                GVarInvalidCast(typeName(), typeName(G_VAR_CUSTOM));
                return NULL;
        }
    }

    //! Cast to std::string
    /*! Impossible for void*
      * \throws GVarInvalidCast
      */
    operator std::string() const
    {
        return std::string((char*)(*this));
    }

    //! Returns string representation of variant value
    /*! Another variant of string casting. Impossible for void*
      * \throws GVarInvalidCast
      */
    std::string toString() const {
        return std::string((char*)(*this));
    }

    #ifdef USING_QT
    //! Cast to QString
    /*! Impossible for void*
      * \throws GVarInvalidCast
      */
    operator QString() const
    {
        return QString((char*)(*this));
    }
    #endif

    //! Check if variant can be casted to argument type
    bool canCastTo(GVarType type)
    {
        return G_VAR_CAST_VALIDNESS_TABLE[m_type][type];
    }

    /* Assign */

    //! Assign GVariant
    /*! Old data will be deleted (in case of known variant type).
      * String data will be copied.
      */
    GVariant& operator=(const GVariant &value)
    {
        freeVarData();
        this->m_type = value.m_type;
        if (m_type == G_VAR_PCHAR)
        {
            if (value.m_data.pc)
            {
                int size = (strlen(value.m_data.pc)+1) * sizeof(char);
                m_data.pc = (char*)malloc(size);
                memmove(m_data.pc, value.m_data.pc, size-1);
                m_data.pc[size-1] = '\0';
            } else {
                m_data.pc = (char*)malloc(sizeof(char));
                m_data.pc[0] = '\0';
            }
        }
        else if (m_type == G_VAR_CUSTOM)
        {
            m_customDestructor = value.m_customDestructor;
            m_customCopyConstructor = value.m_customCopyConstructor;
            m_data.custom = (value.*m_customCopyConstructor)();

        }
        else
            this->m_data = value.m_data;
        return *this;
    }

    //! Assign void*
    /*! Old data will be deleted (in case of known variant type)
      */
    GVariant& operator=(void *value)
    {
        freeVarData();
        m_type = G_VAR_PVOID;
        m_data.pv = value;
        return *this;
    }

    //! Assign char*
    /*! Old data will be deleted (in case of known variant type).
      * String data will be copied.
      */
    GVariant& operator=(const char* value)
    {
        freeVarData();
        m_type = G_VAR_PCHAR;
        if (value)
        {
            int size = (strlen(value) + 1)*sizeof(char);
            m_data.pc = (char*)malloc(size);
            memmove(m_data.pc, value, size);
        } else
            m_data.pc = (char*)calloc(sizeof(char), 1);
        return *this;
    }

    //! Assign int
    /*! Old data will be deleted (in case of known variant type)
      */
    GVariant& operator=(const int &value)
    {
        freeVarData();
        m_type = G_VAR_INT;
        m_data.i = value;
        return *this;
    }

    //! Assign float
    /*! Old data will be deleted (in case of known variant type)
      */
    GVariant& operator=(const float &value)
    {
        freeVarData();
        m_type = G_VAR_FLOAT;
        m_data.f = value;
        return *this;
    }

    //! Assign double
    /*! Old data will be deleted (in case of known variant type)
      */
    GVariant& operator=(const double &value)
    {
        freeVarData();
        m_type = G_VAR_DOUBLE;
        m_data.d = value;
        return *this;
    }

    /* Math operators */
    /*! \brief Sum operator. It have special sematics for strings.
      *
      * Result have type of left operand.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend GVariant operator+(const GVariant &left, const GVariant &right);

    /*! \brief Summing operator for variant and char*
      *
      * It make sense only if variant's type is char*, otherwise error will be raised.
      */
    friend GVariant operator+(const GVariant &left, const char *right);

    /*! \brief Subtraction operator
      *
      * Result have type of left operand.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend GVariant operator-(const GVariant &left, const GVariant &right);

    /*! \brief Multiplication operator
      *
      * Result have type of left operand.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend GVariant operator*(const GVariant &left, const GVariant &right);

    /*! \brief Division operator
      *
      * Result have type of left operand.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend GVariant operator/(const GVariant &left, const GVariant &right);

    /* Comparison operators */

    /*! \brief Comparision operator
      *
      * Check for equality.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend bool operator==(const GVariant &left, const GVariant &right);

    /*! \brief Comparision operator
      *
      * Check for inequality.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend bool operator!=(const GVariant &left, const GVariant &right);

    /*! \brief Comparision operator
      *
      * Check for less.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend bool operator<(const GVariant &left, const GVariant &right);

    /*! \brief Comparision operator
      *
      * Check for greater.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend bool operator>(const GVariant &left, const GVariant &right);

    /*! \brief Comparision operator
      *
      * Check for less or equality.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend bool operator<=(const GVariant &left, const GVariant &right);

    /*! \brief Comparision operator
      *
      * Check for greater or equality.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend bool operator>=(const GVariant &left, const GVariant &right);

    /* Boolean operators */

    /*! \brief Boolean operator
      *
      * Invert boolean value.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    friend bool operator!(const GVariant &value);

    /* Other operators */

    /*! \brief Output to ostream
      *
      * If type is NULL, than output will be "NULL"
      */
    friend std::ostream& operator<<(std::ostream &o, const GVariant &v);

    /* Other */

    //! Get pointer for given class or NULL
    /*! Possible only for void*
      */
    template <class T> bool asClass(T **variable)
    {
        if (m_type == G_VAR_PVOID)
            *variable = reinterpret_cast<T*>(m_data.pv);
        else
            *variable = NULL;
        return *variable != NULL;
    }

    //! Get current type
    GVarType type() const { return m_type; }

    //! Get current type name
    std::string typeName() const
    {
        return typeName(m_type);
    }

    //! Get name of variant type
    static std::string typeName(GVarType type)
    {
        switch (type)
        {
            case G_VAR_NULL:   return "NULL";
            case G_VAR_INT:    return "int";
            case G_VAR_FLOAT:  return "float";
            case G_VAR_DOUBLE: return "double";
            case G_VAR_PCHAR:  return "pchar";
            case G_VAR_PVOID:  return "pointer";
            case G_VAR_BOOL:   return "bool";
            case G_VAR_CUSTOM: return "custom";
            default:           return "unknown";
        }
    }
protected:
    //! Free variant data memory
    void freeVarData()
    {
        // char* data
        if (m_type == G_VAR_PCHAR)
        {
            if (m_data.pc)
            {
                free(m_data.pc);
                m_data.pc = NULL;
            }
        }
        // custom data
        else if (m_type == G_VAR_CUSTOM)
        {
            (this->*m_customDestructor)();
        }
    }


    //! Custom type deleting method
    template <class T>
    void customDelete()
    {
        delete reinterpret_cast<T*>(m_data.custom);
    }

    //! Custom type copy method
    template <class T>
    void* customCopy() const
    {
        T* t = reinterpret_cast<T*>(m_data.custom);
        return reinterpret_cast<void*>( new T(*t) );
    }

    /** \brief Create temporary statical buffer
      *
      * It used for char* casting. It's potential source of subtle memory bugs =)
      * This cast technique must be rewriten, meanwhile programmer must remember that
      * he can't rely on long-term storing of GVariant-char* casting result.
      */
    static char* createStaticBuffer(int size = 200, const char* value = NULL) {
        static char **varBuffers = NULL;
        static int varBuffersPos;
        static int const varBuffersMax = 1000;
        // first call? - create buffers pull
        if (varBuffers == NULL)
        {
            varBuffers = (char**)calloc(sizeof(char**), varBuffersMax);
            varBuffersPos = 0;
        }
        // create new buffer
        if (++varBuffersPos >= varBuffersMax)
        {
            logg.warning("GVariant cast buffer was overloaded and cleared");
            varBuffersPos = 0;
        }
        if (varBuffers[varBuffersPos] != NULL)
            free(varBuffers[varBuffersPos]);
        varBuffers[varBuffersPos] = (char*)calloc(size, 1);
        if (value)
            memmove(varBuffers[varBuffersPos], value, size);
        return varBuffers[varBuffersPos];
    }

protected:
    //! Current variant type
    GVarType m_type;
    //! Current variant value
    GVarData m_data;

    //! Custom variant type destructor
    void (GVariant::*m_customDestructor)();
    void* (GVariant::*m_customCopyConstructor)() const;
};

/**************** Math *****************/
/*! \brief Add operator. It has special sematics for strings.
  *
  * Result of expression have type of left operand.
  * If type of one of the operands is undefined or unsupported than error will be raised.
  */
inline GVariant operator+(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    if (left.m_type == G_VAR_PCHAR)
    {
        GVariant res(G_VAR_PCHAR);
        char *buf = (char*)right;
        int len1 = strlen(left.m_data.pc), len2 = strlen(buf),
            size = (len1 + len2 + 1) * sizeof(char);
        res.m_data.pc = (char*)malloc(size);
        res.m_data.pc[size-1] = '\0';
        memmove(res.m_data.pc, left.m_data.pc, len1 * sizeof(char));
        memmove(res.m_data.pc + len1, buf, len2 * sizeof(char));
        return res;
    }

    switch (left.m_type)
    {
        case G_VAR_INT: return GVariant(left.m_data.i + int(right));
        case G_VAR_FLOAT: return GVariant(left.m_data.f + float(right));
        case G_VAR_DOUBLE: return GVariant(left.m_data.d + double(right));
        case G_VAR_BOOL: return GVariant(
                    int((left.m_data.b ? 1 : 0) + (bool(right) ? 1 : 0)) );
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}

/*! \brief Operator for add variant and char*
  *
  * It makes sanse only if GVariant's type is char*, otherwise GException will be raised.
  * \throws GException
  */
inline GVariant operator+(const GVariant &left, const char *right)
{
    if (left.m_type == G_VAR_PCHAR)
        return left + GVariant(right);
    else
        G_VAR_UNDEFINED_OPERATION
}

/*! \brief Subtraction operator
  *
  * Result of expression have type of left operand.
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline GVariant operator-(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left.m_type) {
        case G_VAR_INT: return GVariant(left.m_data.i - int(right));
        case G_VAR_FLOAT: return GVariant(left.m_data.f - float(right));
        case G_VAR_DOUBLE: return GVariant(left.m_data.d - double(right));
        case G_VAR_BOOL: return GVariant(
                    (left.m_data.b ? 1 : 0) - (bool(right) ? 1 : 0));
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}

/*! \brief Multiplication operator
  *
  * Result of expression have type of left operand.
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline GVariant operator*(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left.m_type) {
        case G_VAR_INT: return GVariant(left.m_data.i * int(right));
        case G_VAR_FLOAT: return GVariant(left.m_data.f * float(right));
        case G_VAR_DOUBLE: return GVariant(left.m_data.d * double(right));
        case G_VAR_BOOL: return GVariant( left.m_data.b && bool(right) );
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}

/*! \brief Division operator
  *
  * Result of expression have type of left operand.
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline GVariant operator/(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left.m_type) {
        case G_VAR_INT:    return GVariant(left.m_data.i / int(right));
        case G_VAR_FLOAT:  return GVariant(left.m_data.f / float(right));
        case G_VAR_DOUBLE: return GVariant(left.m_data.d / double(right));
        case G_VAR_BOOL:   return GVariant(left.m_data.b / bool(right));
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}

/* Операторы сравнения */

/*! \brief Equality operator
  *
  * Check for equality.
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline bool operator==(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left.m_type) {
        case G_VAR_INT: return left.m_data.i == int(right);
        case G_VAR_FLOAT: return left.m_data.f == float(right);
        case G_VAR_DOUBLE: return left.m_data.d == double(right);
        case G_VAR_PCHAR: return strcmp(left.m_data.pc, (char*)(right)) == 0;
        case G_VAR_BOOL: return left.m_data.b == right.m_data.b;
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}

/*! \brief Equality operator
  *
  * Special case for GVariant and char* string.
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */

inline bool operator==(const GVariant &left, const char *right)
{
    if (left.type() == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    return strcmp((char*)left, right) == 0;
}

/*! \brief Unequality operator
  *
  * Special case for GVariant and char* string.
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */

inline bool operator!=(const GVariant &left, const char *right)
{
    if (left.type() == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    return strcmp((char*)left, right) != 0;
}

/*! \brief Unequality operator
  *
  * Check for unequality.
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline bool operator!=(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left.m_type) {
        case G_VAR_INT: return left.m_data.i != int(right);
        case G_VAR_FLOAT: return left.m_data.f != float(right);
        case G_VAR_DOUBLE: return left.m_data.d != double(right);
        case G_VAR_PCHAR: return strcmp(left.m_data.pc, (char*)(right)) != 0;
        case G_VAR_BOOL: return left.m_data.b != right.m_data.b;
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}

/*! \brief Less operator
  *
  * Check if left is less than right. Alphobetic comparison for string (like strcmp).
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline bool operator<(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left.m_type) {
        case G_VAR_INT: return left.m_data.i < int(right);
        case G_VAR_FLOAT: return left.m_data.f < float(right);
        case G_VAR_DOUBLE: return left.m_data.d < double(right);
        case G_VAR_PCHAR: return strcmp(left.m_data.pc, (char*)(right)) == -1;
        case G_VAR_BOOL: return !left.m_data.b && right.m_data.b;
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}


/*! \brief Greater operator
  *
  * Check if left is greater than right. Alphobetic comparison for string (like strcmp).
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline bool operator>(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left.m_type) {
        case G_VAR_INT: return left.m_data.i > int(right);
        case G_VAR_FLOAT: return left.m_data.f > float(right);
        case G_VAR_DOUBLE: return left.m_data.d > double(right);
        case G_VAR_PCHAR: return strcmp(left.m_data.pc, (char*)(right)) == 1;
        case G_VAR_BOOL: return left.m_data.b && !right.m_data.b;
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}


/*! \brief Less or equal operator
  *
  * Check if left is less than or equal to right. Alphobetic comparison for string (like strcmp).
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline bool operator<=(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    int res;
    switch (left.m_type) {
        case G_VAR_INT: return left.m_data.i <= int(right);
        case G_VAR_FLOAT: return left.m_data.f <= float(right);
        case G_VAR_DOUBLE: return left.m_data.d <= double(right);
        case G_VAR_PCHAR:
            res = strcmp(left.m_data.pc, (char*)(right));
            return res == -1 || res == 0;
        case G_VAR_BOOL: return (left.m_data.b == right.m_data.b) || (!left.m_data.b && right.m_data.b);
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}


/*! \brief Greater or equal operator.
  *
  * Check if left is greater than or equal to right. Alphobetic comparison for string (like strcmp).
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline bool operator>=(const GVariant &left, const GVariant &right)
{
    if (left.m_type == G_VAR_NULL || right.m_type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    int res;
    switch (left.m_type) {
        case G_VAR_INT: return left.m_data.i >= int(right);
        case G_VAR_FLOAT: return left.m_data.f >= float(right);
        case G_VAR_DOUBLE: return left.m_data.d >= double(right);
        case G_VAR_PCHAR:
            res = strcmp(left.m_data.pc, (char*)(right));
            return res == 1 || res == 0;
        case G_VAR_BOOL: return (left.m_data.b == right.m_data.b) || (left.m_data.b && !right.m_data.b);
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}

/* Булевы операторы */

/*! \brief Boolean not operator
  *
  * Traditional sematics. Defined only for numerical and boolean types
  * \throws GException
  */
inline bool operator!(const GVariant &value)
{
    switch (value.m_type) {
        case G_VAR_INT: return !value.m_data.i;
        case G_VAR_FLOAT: return !value.m_data.f;
        case G_VAR_DOUBLE: return !value.m_data.d;
        case G_VAR_BOOL: return !value.m_data.b;
        default:
            G_VAR_UNDEFINED_OPERATION
    }
}

/* Helpers operators */

/*! \brief Output GVariant to std::ostream
  *
  * In case of undefined type of GVariant output will be "NULL"
  */
inline std::ostream& operator<<(std::ostream &o, const GVariant &v)
{
    return o << (v.m_type == G_VAR_NULL ? "NULL" : (char*)v);
}

//! Predefined NULL-variant constant
const GVariant GVARNULL(G_VAR_NULL);


/*@}*/

#endif // GVARIANT_H
