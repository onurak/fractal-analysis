/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TSPARSER_H
#define TSPARSER_H

#include "../G/GDelegate.h"
#include "../groupcheckers/GroupChecker.h"
#include "../Pattern.h"
#include "../ParseContext.h"
#include "../ParseResult.h"
#include "../ProjectSettings.h"

namespace FL {

/*! \addtogroup FLAlgorithm
  */
/*@{*/


/*! \class TSParser
  * \brief Parser for building parse tree of time series
  *
  * Searching for templates in terminal-marked time series
  */
class TSParser
{
public:
    //! Parser execution work mode
    enum WorkMode
    {
        WorkNormal, //!< Normal execution
        WorkDebug   //!< Debug mode (means step by step work)
    };

    //! Parser debug event structure
    struct DebugEvent
    {
        DebugEvent(int type = 0, ParseContext *context = NULL): type(type), context(context) {}
        int type;
        ParseContext *context;
    };

    /*! \typedef NotifyProgress
      * \brief Notification about progress of analysis
      *
      * Argument is the complete percent (0 to 100)
      */
    typedef GDelegate1<void, int> NotifyProgress;

    /*! \typedef NotifyDebug
      * \brief Notification about new debug event
      *
      * Argument is the event info.
      * Result is the continue execution flag.
      */
    typedef GDelegate1<bool, DebugEvent> NotifyDebug;
public:
    //! Parsing time series markup, moving to next fractal level
    /*! Realisation in children depending on algorithm
      * \param trees Current set of parse trees
      * \param patterns Set of patterns to parse
      * \param ts Time series
      * \param checker Used checking algorithm
      * \param mode Runing mode
      */
    virtual ParseResult parse(ParseTreeSet &trees,
                              PatternCollection &patterns,
                              FL::TimeSeries *ts,
                              FL::Patterns::GroupChecker &checker,
                              WorkMode mode = WorkNormal) = 0;

    //! Some algorithms can trigger this event while progress (argument - percent of completion)
    NotifyProgress onProgress;

    //! Some algorithms can trigger this event if new debug info available (only if FL_ENABLE_PARSER_DEBUG is defined)
    NotifyDebug onDebug;

protected:
    bool DEBUG_NOTIFICATION(int type, ParseContext *context)
    {
        #ifdef FL_ENABLE_PARSER_DEBUG
            return this->onDebug(DebugEvent(type, context));
        #else
            return true;
        #endif
    }
};



/*@}*/

} // namespace

#endif // TSPARSER_H
