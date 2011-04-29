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

#ifndef GVariant_H
#define GVariant_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <cerrno>
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

const int G_VAR_CAST_VALIDNESS_TABLE[7][7] =
{
// base type       |  new type =>
//    ||           |
//    \/           | NULL | INT | FLOAT | DOUBLE | PCHAR | PVOID | BOOL |
/* NULL   */       {   0,    0,     0,       0,      0,     0,      0   },
/* INT    */       {   0,    1,     1,       1,      1,     0,      1   },
/* FLOAT  */       {   0,    1,     1,       1,      1,     0,      1   },
/* DOUBLE */       {   0,    1,     1,       1,      1,     0,      1   },
/* PCHAR  */       {   0,    2,     2,       2,      1,     1,      1   },
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

    inline void GVarInvalidCast(const std::string &fromType, const std::string &toType)
    {
        GError(GCritical, GVariantName, G_INVALID_TYPECAST, fromType + " to " + toType);
    }

    //inline void GVarInvalidInit(int)

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
private:
    //! Info of custom variant data
    struct CustomTypeInfo
    {
        CustomTypeInfo() : customDestructor(NULL), customCopyConstructor(NULL) {}

        //! Hash id of type
        //GVariantTypeHash hashId;

        //! Destruction method of custom type
        void (GVariant::*customDestructor)();

        //! Coping method of custom type
        void* (GVariant::*customCopyConstructor)() const;
    };


public:
    //! Default constructor
    /*! Value of variant will be eqivalent of zero (e.g. 0, NULL)
      * \param type Type of new variant
      */
    GVariant(GVarType type = G_VAR_NULL)
    {
        commonInit();
        if (type != G_VAR_CUSTOM)
        {
            _type = type;
            _data.i = 0;
        }
        else
        {
            _type = G_VAR_NULL;
            _data.i = 0;
            GError(GCritical, GVariantName, G_INVALID_VARIANT_INIT, "custom");
        }
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
        commonInit();
        if (value._type != G_VAR_CUSTOM)
        {
            _type = G_VAR_NULL;
            *this = value;
        }
        else
        {
            _type = G_VAR_CUSTOM;
            _customInfo = new CustomTypeInfo();
            _customInfo->customDestructor = value._customInfo->customDestructor;
            _customInfo->customCopyConstructor = value._customInfo->customCopyConstructor;

            void* (GVariant::*customCopyConstructor)() const = _customInfo->customCopyConstructor;
            _data.custom = (value.*customCopyConstructor)();

        }
    }

    //! Contructor for int type
    GVariant(int value)
    {
        commonInit();
        _type = G_VAR_INT;
        _data.i = value;
    }

    //! Contructor for float type
    GVariant(const float &value)
    {
        commonInit();
        _type = G_VAR_FLOAT;
        _data.f = value;
    }

    //! Contructor for double type
    GVariant(const double &value)
    {
        commonInit();
        _type = G_VAR_DOUBLE;
        _data.d = value;
    }

    //! Contructor for char* type
    GVariant(const char *value)
    {
        commonInit();
        _type = G_VAR_NULL;
        *this = value;
    }

    //! Contructor for std::string type
    GVariant(const std::string &value)
    {
        commonInit();
        _type = G_VAR_NULL;
        *this = value.c_str();
    }

    //! Contructor for void* type
    GVariant(void *value)
    {
        commonInit();
        _type = G_VAR_PVOID;
        _data.pv = value;
    }

    //! Contructor for bool type
    GVariant(bool value)
    {
        commonInit();
        _type = G_VAR_BOOL;
        _data.b = value;
    }

    //! Constructor for custom type
    template <typename T>
    explicit GVariant(const T& custom)
    {
        commonInit();
        _type = G_VAR_CUSTOM;
        _data.custom = new T(custom);
        _customInfo = new CustomTypeInfo();
        _customInfo->customDestructor = &GVariant::customDelete<T>;
        _customInfo->customCopyConstructor = &GVariant::customCopy<T>;
    }

    /* Type casting section */
    //! Cast to int
    /*! Impossible for void*, could be impossible for char*
      */
    operator int() const
    {
        switch (_type)
        {
            case G_VAR_INT:
                return _data.i;
            case G_VAR_FLOAT:
                return _data.f;
            case G_VAR_DOUBLE:
                return _data.d;
            case G_VAR_PCHAR:
            {
                char *eptr = NULL;
                int result = (int)strtol(_data.pc, &eptr, 0);
                if (*eptr != '\0')
                {
                    GVarInvalidCast(typeName(), typeName(G_VAR_INT));
                    return 0;
                }
                else
                    return result;
            }
            case G_VAR_BOOL:
                return _data.b ? 1 : 0;
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
        switch (_type)
        {
            case G_VAR_INT:
                return _data.i;
            case G_VAR_FLOAT:
                return _data.f;
            case G_VAR_DOUBLE:
                return _data.d;
            case G_VAR_PCHAR:
                return atof(_data.pc);
            case G_VAR_BOOL:
                return _data.b ? 1 : 0;
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
        switch (_type)
        {
            case G_VAR_INT:
                return _data.i;
            case G_VAR_FLOAT:
                return _data.f;
            case G_VAR_DOUBLE:
                return _data.d;
            case G_VAR_PCHAR:
                return atof(_data.pc);
            case G_VAR_BOOL:
                return _data.b ? 1 : 0;
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
        switch (_type)
        {
            case G_VAR_INT:
                return _data.i;
            case G_VAR_FLOAT:
                return _data.f;
            case G_VAR_DOUBLE:
                return _data.d;
            case G_VAR_PCHAR:
                return doesStringMeansTrue(_data.pc);
            case G_VAR_BOOL:
                return _data.b;
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
        switch (_type)
        {
            case G_VAR_INT:
                buf = createStaticBuffer();
                sprintf(buf, "%i", _data.i);
                return buf;
            case G_VAR_FLOAT:
                buf = createStaticBuffer();
                sprintf(buf, "%f", _data.f);
                return buf;
            case G_VAR_DOUBLE:
                buf = createStaticBuffer();
                sprintf(buf, "%lf", _data.d);
                return buf;
            case G_VAR_PCHAR:
                return createStaticBuffer((strlen(_data.pc)+1) * sizeof(char), _data.pc);

            case G_VAR_BOOL:
                return _data.b ? createStaticBuffer(5 * sizeof(char), "true") :
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
        switch (_type)
        {
            case G_VAR_PVOID:
                return _data.pv;
            case G_VAR_PCHAR:
                return reinterpret_cast<void*>(_data.pc);
            case G_VAR_CUSTOM:
                return reinterpret_cast<void*>(_data.custom);
            default:
                GVarInvalidCast(typeName(), typeName(G_VAR_PVOID));
                return NULL;
        }
    }

    //! Cast to custom type
    /*! Only possible for same custom type
      */
    template <class T>
    operator T*() const
    {
        switch (_type)
        {
            case G_VAR_PVOID:
                return reinterpret_cast<T*>(_data.pv);
            case G_VAR_CUSTOM:
                return reinterpret_cast<T*>(_data.custom);
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
    bool canCastTo(GVarType type) const
    {
        int result = G_VAR_CAST_VALIDNESS_TABLE[_type][type];
        if (result < 2)
            return result;
        switch (_type)
        {
            case G_VAR_PCHAR:
                switch (type)
                {
                    case G_VAR_INT:
                        { int i; return _atoi(_data.pc, i); }
                    case G_VAR_FLOAT:
                        { float i; return _atof(_data.pc, i); }
                    case G_VAR_DOUBLE:
                        { double i; return _atod(_data.pc, i); }
                    default:
                        return false;
                }
            default:
                return false;
        }
    }

    /* Assign */

    //! Assign GVariant
    /*! Old data will be deleted (in case of known variant type).
      * String data will be copied.
      */
    GVariant& operator=(const GVariant &value)
    {
        freeVarData();
        this->_type = value._type;
        if (_type == G_VAR_PCHAR)
        {
            if (value._data.pc)
            {
                int size = (strlen(value._data.pc)+1) * sizeof(char);
                _data.pc = (char*)malloc(size);
                memmove(_data.pc, value._data.pc, size-1);
                _data.pc[size-1] = '\0';
            } else {
                _data.pc = (char*)malloc(sizeof(char));
                _data.pc[0] = '\0';
            }
        }
        else if (_type == G_VAR_CUSTOM)
        {
            _customInfo->customDestructor = value._customInfo->customDestructor;
            _customInfo->customCopyConstructor = value._customInfo->customCopyConstructor;
            void* (GVariant::*customCopyConstructor)() const = _customInfo->customCopyConstructor;
            _data.custom = (value.*customCopyConstructor)();
        }
        else
            this->_data = value._data;
        return *this;
    }

    //! Assign void*
    /*! Old data will be deleted (in case of known variant type)
      */
    GVariant& operator=(void *value)
    {
        freeVarData();
        _type = G_VAR_PVOID;
        _data.pv = value;
        return *this;
    }

    //! Assign char*
    /*! Old data will be deleted (in case of known variant type).
      * String data will be copied.
      */
    GVariant& operator=(const char* value)
    {
        freeVarData();
        _type = G_VAR_PCHAR;
        if (value)
        {
            int size = (strlen(value) + 1)*sizeof(char);
            _data.pc = (char*)malloc(size);
            memmove(_data.pc, value, size);
        } else
            _data.pc = (char*)calloc(sizeof(char), 1);
        return *this;
    }

    //! Assign int
    /*! Old data will be deleted (in case of known variant type)
      */
    GVariant& operator=(const int &value)
    {
        freeVarData();
        _type = G_VAR_INT;
        _data.i = value;
        return *this;
    }

    //! Assign float
    /*! Old data will be deleted (in case of known variant type)
      */
    GVariant& operator=(const float &value)
    {
        freeVarData();
        _type = G_VAR_FLOAT;
        _data.f = value;
        return *this;
    }

    //! Assign double
    /*! Old data will be deleted (in case of known variant type)
      */
    GVariant& operator=(const double &value)
    {
        freeVarData();
        _type = G_VAR_DOUBLE;
        _data.d = value;
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

    /*! \brief Sum operator. It have special sematics for strings.
      *
      * Result have type of left operand.
      * If one of operands have NULL type or have unsupported type than error
      * will be raised.
      */
    GVariant operator+=(int i)
    {
        switch (_type)
        {
            case G_VAR_INT:
                _data.i += i;
                break;
            case G_VAR_BOOL:
                _data.b = !(_data.b && i);
                break;
            case G_VAR_DOUBLE:
                _data.d += (double)i;
                break;
            case G_VAR_FLOAT:
                _data.f += (float)i;
                break;
            //case G_VAR_PCHAR:
            //{
                //char *pc = malloc(strlen())
                //freeVarData();
                //_data.d += (double)i;
                //break;
            //}
            default:
                GError(GCritical,
                       GVariantName,
                       G_UNDEFINED_OPERATION,
                       GVariantUndefinedOperation);
                return GVariant(G_VAR_NULL);
        }
        return *this;
    }

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
    template <class T> bool as(T **variable)
    {
        if (_type == G_VAR_PVOID)
            *variable = reinterpret_cast<T*>(_data.pv);
        else
            *variable = NULL;
        return *variable != NULL;
    }

    //! Get current type
    GVarType type() const { return _type; }

    //! Get current type name
    std::string typeName() const
    {
        return typeName(_type);
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
        if (_type == G_VAR_PCHAR)
        {
            if (_data.pc)
            {
                free(_data.pc);
                _data.pc = NULL;
            }
        }
        // custom data
        else if (_type == G_VAR_CUSTOM)
        {
            if (_customInfo)
            {
                void (GVariant::*customDestructor)() = _customInfo->customDestructor;
                (this->*customDestructor)();
                delete _customInfo;
                _customInfo = NULL;
            }
        }
    }

    //! Common initializing procedure
    inline void commonInit()
    {
        _customInfo = NULL;
    }

    //! Custom type deleting method
    template <class T>
    void customDelete()
    {
        delete reinterpret_cast<T*>(_data.custom);
    }

    //! Custom type copy method
    template <class T>
    void* customCopy() const
    {
        T* t = reinterpret_cast<T*>(_data.custom);
        return reinterpret_cast<void*>( new T(*t) );
    }

    /** \brief Create temporary statical buffer
      *
      * It used for char* casting. It's potential source of subtle memory bugs =)
      * This cast technique must be rewriten, meanwhile programmer must remember that
      * he can't rely on long-term storing of GVariant-char* casting result.
      */
    static char* createStaticBuffer(int size = 200, const char* value = NULL)
    {
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

    bool _atoi(const char* s, int &out) const
    {
        char *pEnd;
        long val = strtol(s, &pEnd, 10);
        if (*pEnd || ((val == LONG_MAX || val == LONG_MIN) && errno == ERANGE))
            return false;
        out = (int)val;
        return true;
    }

    bool _atof(const char* s, float &out) const
    {
        char *pEnd;
        float val = strtof(s, &pEnd);
        if (*pEnd || ((val == LONG_MAX || val == LONG_MIN) && errno == ERANGE))
            return false;
        out = val;
        return true;
    }

    bool _atod(const char* s, double &out) const
    {
        char *pEnd;
        double val = strtod(s, &pEnd);
        if (*pEnd || ((val == LONG_MAX || val == LONG_MIN) && errno == ERANGE))
            return false;
        out = val;
        return true;
    }
protected:
    //! Current variant type
    GVarType _type;
    //! Current variant value
    GVarData _data;

    //! Custom variant type info.
    /*! It would be created only if type of variant became custom */
    CustomTypeInfo *_customInfo;
};



/**************** Math *****************/
/*! \brief Add operator. It has special sematics for strings.
  *
  * Result of expression have type of left operand.
  * If type of one of the operands is undefined or unsupported than error will be raised.
  */
inline GVariant operator+(const GVariant &left, const GVariant &right)
{
    if (left._type == G_VAR_NULL || right._type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    if (left._type == G_VAR_PCHAR)
    {
        GVariant res(G_VAR_PCHAR);
        char *buf = (char*)right;
        int len1 = strlen(left._data.pc), len2 = strlen(buf),
            size = (len1 + len2 + 1) * sizeof(char);
        res._data.pc = (char*)malloc(size);
        res._data.pc[size-1] = '\0';
        memmove(res._data.pc, left._data.pc, len1 * sizeof(char));
        memmove(res._data.pc + len1, buf, len2 * sizeof(char));
        return res;
    }

    switch (left._type)
    {
        case G_VAR_INT: return GVariant(left._data.i + int(right));
        case G_VAR_FLOAT: return GVariant(left._data.f + float(right));
        case G_VAR_DOUBLE: return GVariant(left._data.d + double(right));
        case G_VAR_BOOL: return GVariant(
                    int((left._data.b ? 1 : 0) + (bool(right) ? 1 : 0)) );
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
    if (left._type == G_VAR_PCHAR)
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
    if (left._type == G_VAR_NULL || right._type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left._type) {
        case G_VAR_INT: return GVariant(left._data.i - int(right));
        case G_VAR_FLOAT: return GVariant(left._data.f - float(right));
        case G_VAR_DOUBLE: return GVariant(left._data.d - double(right));
        case G_VAR_BOOL: return GVariant(
                    (left._data.b ? 1 : 0) - (bool(right) ? 1 : 0));
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
    if (left._type == G_VAR_NULL || right._type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left._type) {
        case G_VAR_INT: return GVariant(left._data.i * int(right));
        case G_VAR_FLOAT: return GVariant(left._data.f * float(right));
        case G_VAR_DOUBLE: return GVariant(left._data.d * double(right));
        case G_VAR_BOOL: return GVariant( left._data.b && bool(right) );
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
    if (left._type == G_VAR_NULL || right._type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left._type) {
        case G_VAR_INT:    return GVariant(left._data.i / int(right));
        case G_VAR_FLOAT:  return GVariant(left._data.f / float(right));
        case G_VAR_DOUBLE: return GVariant(left._data.d / double(right));
        case G_VAR_BOOL:   return GVariant(left._data.b / bool(right));
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
    if (left._type == G_VAR_NULL || right._type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left._type)
    {
        case G_VAR_INT:
            if (!right.canCastTo(G_VAR_INT))
                return false;
            return left._data.i == int(right);
        case G_VAR_FLOAT:
            if (!right.canCastTo(G_VAR_FLOAT))
                return false;
            return left._data.f == float(right);
        case G_VAR_DOUBLE:
            if (!right.canCastTo(G_VAR_FLOAT))
                return false;
            return left._data.d == double(right);
        case G_VAR_PCHAR:
            return strcmp(left._data.pc, (char*)(right)) == 0;
        case G_VAR_BOOL:
            if (!right.canCastTo(G_VAR_BOOL))
                return false;
            return left._data.b == (bool)right;
        case G_VAR_PVOID:
            if (!right.canCastTo(G_VAR_PVOID))
                return false;
            return left._data.pv == (void*)right;
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
    return !(left == right);
}

/*! \brief Less operator
  *
  * Check if left is less than right. Alphobetic comparison for string (like strcmp).
  * If type of one of the operands is undefined or unsupported than GException will be raised.
  * \throws GException
  */
inline bool operator<(const GVariant &left, const GVariant &right)
{
    if (left._type == G_VAR_NULL || right._type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left._type) {
        case G_VAR_INT: return left._data.i < int(right);
        case G_VAR_FLOAT: return left._data.f < float(right);
        case G_VAR_DOUBLE: return left._data.d < double(right);
        case G_VAR_PCHAR: return strcmp(left._data.pc, (char*)(right)) == -1;
        case G_VAR_BOOL: return !left._data.b && right._data.b;
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
    if (left._type == G_VAR_NULL || right._type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    switch (left._type) {
        case G_VAR_INT: return left._data.i > int(right);
        case G_VAR_FLOAT: return left._data.f > float(right);
        case G_VAR_DOUBLE: return left._data.d > double(right);
        case G_VAR_PCHAR: return strcmp(left._data.pc, (char*)(right)) == 1;
        case G_VAR_BOOL: return left._data.b && !right._data.b;
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
    if (left._type == G_VAR_NULL || right._type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    int res;
    switch (left._type) {
        case G_VAR_INT: return left._data.i <= int(right);
        case G_VAR_FLOAT: return left._data.f <= float(right);
        case G_VAR_DOUBLE: return left._data.d <= double(right);
        case G_VAR_PCHAR:
            res = strcmp(left._data.pc, (char*)(right));
            return res == -1 || res == 0;
        case G_VAR_BOOL: return (left._data.b == right._data.b) || (!left._data.b && right._data.b);
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
    if (left._type == G_VAR_NULL || right._type == G_VAR_NULL)
        G_VAR_UNDEFINED_OPERATION
    int res;
    switch (left._type) {
        case G_VAR_INT: return left._data.i >= int(right);
        case G_VAR_FLOAT: return left._data.f >= float(right);
        case G_VAR_DOUBLE: return left._data.d >= double(right);
        case G_VAR_PCHAR:
            res = strcmp(left._data.pc, (char*)(right));
            return res == 1 || res == 0;
        case G_VAR_BOOL: return (left._data.b == right._data.b) || (left._data.b && !right._data.b);
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
    switch (value._type) {
        case G_VAR_INT: return !value._data.i;
        case G_VAR_FLOAT: return !value._data.f;
        case G_VAR_DOUBLE: return !value._data.d;
        case G_VAR_BOOL: return !value._data.b;
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
    return o << (v._type == G_VAR_NULL ? "NULL" : (char*)v);
}

//! Predefined NULL-variant constant
const GVariant GVARNULL(G_VAR_NULL);


/*@}*/

#endif // GVariant_H
