#ifndef FILEPAT_H
#define FILEPAT_H

#include "../patterns/Pattern.h"

namespace FL { namespace IO {

/*! @class FilePAT
  * @brief File with patterns data
  *
  * @section File structure
  * Patterns file consists of few sections, there is common structure:
  * @verbatim
  * PATTERNS:
  *     pattern1 = description1 @ guard1;
  *     pattern2 = description2 @ guard2;
  *     ....
  *     patternN = descriptionN @ guardN;
  * @endverbatim
  */
class FilePAT
{
public:
    //! Open file with specified name and load all patterns to @a patterns
    /*! @param fileName Name of file to open
      * @param patterns Output patterns set
      * @return true if file loaded normally, false if there was an errors
      */
    bool open(const std::string &fileName, FL::Patterns::PatternsSet &patterns);

    //! Get last error info
    const Exceptions::EParsing& lastError() const { return m_lastError; }
private:
    //! Last error info
    Exceptions::EParsing m_lastError;
};

}} // namespaces

#endif // FILEPAT_H
