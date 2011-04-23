#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>
#include "../../../G/GVariant.h"
#include "../../Common.h"
#include "../../exceptions/EException.h"
#include "../Context.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \struct IndexedName
  * \brief Name and it's index
  */
struct IndexedName
{
    IndexedName(int nameId = -1, int no = -2) : nameId(nameId), no(no) {}
    IndexedName(const std::string &name = "", int no = -2) : no(no)
    {
        nameId = IDGenerator::idOf(name);
    }
    int nameId;
    int no;
};

/*! \enum Error
  * \brief Function's error
  */
enum Error
{
    OK = 0,
    INVALID_ARGS,
    INVALID_NODE
};

//! Get description of error code
const char* errorDescription(Error err);


//! Function's arguments
typedef std::vector<GVariant*> FunctionArgs;

/*! \class Function
  * \brief Function prototype.
  */
class Function
{
public:
    //! Main function of class. Children must override it
    virtual const GVariant& operator()(Patterns::Context&, FunctionArgs&) = 0;

    //! Another way to execute predicate (same as operator())
    inline const GVariant& call(Patterns::Context& context, FunctionArgs& args)
    {
        return this->operator ()(context, args);
    }

    //! Getter of predicate's name
    const std::string& name() const { return m_name; }
protected:
    //! Define switch argument of function.
    /*! Define optional argument of function.
      * \param argName Name of argument that makes it enabled. It automaically
      *                add argument with name "no<argName>" that makes it disabled
      * \param defaultValue true if argument is enabled by default, false otherwise
      */
    void defineArg(const std::string &argName, bool defaultValue);

    //! Parse optional arguments defined by defineArg method
    void parseArgs(FunctionArgs& args);

    //! Check that argument is the valid FL::Trees::Node*
    inline void checkValidNode(GVariant *arg)
    {
        if (arg == NULL || (FL::Trees::Node *)(*arg) == NULL)
            throw FL::Exceptions::EArguments(m_name, "Tree node", "NULL");
    }

protected:
    //! Predicate name. Every child must set it the in constructor
    std::string m_name;

    //! Internal storage for last result
    GVariant m_result;

    //! Available string arguments.
    /*! It could be used for automatic parsing with parseArgs() method.
      * Just define all acceptable arguments names in constructor with
      * defineArg() method. After parseArgs() call it would contain pairs
      * <argName, isEnabled>
      */
    std::map<std::string, bool> m_options;
private:
    //! Pairs of defined arguments - name and default value
    std::map<std::string, bool> m_argsData;
};


}}} // namespaces

#endif // FUNCTION_H
