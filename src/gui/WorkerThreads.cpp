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

#include "WorkerThreads.h"

/****** LEVEL ANALYSER ********/

void WorkerThreadLevelAnalyser::run()
{
    watcher.setOption("debug", m_debugMode);

    // No parse trees - markup time series
    if (watcher.parseTreeSet().size() == 0)
    {
        logg.info("Markup with marker '") << watcher.option("marker").toString() << "'... ";
        if (watcher.markup())
        {
            m_result = RET_OK;
            logg << "Markup completed: OK";
        }
        else
        {
            m_result = RET_ERROR;
            m_resultDescription = QSTR(watcher.lastExceptionMsg());
            logg << "Markup completed: Failed (" << watcher.lastExceptionMsg() << ")";
        }
    }
    else
    {
        // Markup unmarked part
        if (m_unmarkedSize > 0)
        {
            if (watcher.markup(watcher.timeSeries()->dimSize(0) - m_unmarkedSize))
            {
                logg << "New values marked up (" << m_unmarkedSize << " points)";
            }
            else
            {
                m_result = RET_ERROR;
                m_resultDescription = QSTR(watcher.lastExceptionMsg());
                logg << "Error marking up new values: " << watcher.lastExceptionMsg();
                emit finished(this);
                return;
            }
        }

        // Parse all level
        logg.info("Parsing level with parser '") << watcher.option("parser").toString() << "'... ";
        FL::ParseResult result;
        watcher.parser()->onProgress = delegate(this, &WorkerThreadLevelAnalyser::onFLProgress);
        watcher.parser()->onDebug = delegate(this, &WorkerThreadLevelAnalyser::onFLDebug);
        if (watcher.parseLevel(&result))
        {
            m_result = result.parsedCount > 0 ? RET_OK : RET_LAST_LEVEL;
            logg << "Parse completed: OK (" << result.parsedCount << " patterns parsed)";
            m_resultDescription = "No error";
        }
        else
        {
            m_result = RET_ERROR;
            m_resultDescription = QSTR(watcher.lastExceptionMsg());
            logg << "Parse completed: Failed (" << watcher.lastExceptionMsg() << ")";
        }
    }
    emit finished(this);
}

void WorkerThreadLevelAnalyser::onFLProgress(int progress)
{
    QMutexLocker locker(&m_mutex);
    emit this->progressChanged(progress, "Analysis in progress...");
}

bool WorkerThreadLevelAnalyser::onFLDebug(FL::TSParser::DebugEvent event)
{
    QMutexLocker locker(&m_mutex);
    emit this->debugEvent(event);
    return true;
}


/********* FULL ANALYSER *********/
void WorkerThreadFullAnalyser::run()
{
    watcher.setOption("debug", m_debugMode);

    // No parse trees - markup time series
    if (watcher.parseTreeSet().size() == 0)
    {
        logg.info("Markup with marker '") << watcher.option("marker").toString() << "'... ";
        if (watcher.markup())
        {
            m_result = RET_OK;
            logg << "Markup completed: OK";
        }
        else
        {
            m_result = RET_ERROR;
            m_resultDescription = QSTR(watcher.lastExceptionMsg());
            logg << "Markup completed: Failed (" << watcher.lastExceptionMsg() << ")";
        }
    }

    // Markup unmarked part
    if (m_unmarkedSize > 0)
    {
        if (watcher.markup(watcher.timeSeries()->size()-m_unmarkedSize))
        {
            logg << "New values marked up (" << m_unmarkedSize << " points)";
        }
        else
        {
            m_result = RET_ERROR;
            m_resultDescription = QSTR(watcher.lastExceptionMsg());
            logg << "Error marking up new values: " << watcher.lastExceptionMsg();
            emit finished(this);
            return;
        }
    }

    // Parse now
    logg.info("Parsing all level with parser '") << watcher.option("parser").toString() << "'... ";
    int levelsCount;
    watcher.parser()->onProgress = delegate(this, &WorkerThreadFullAnalyser::onFLProgress);
    watcher.parser()->onDebug = delegate(this, &WorkerThreadFullAnalyser::onFLDebug);
    if (watcher.parseAll(&levelsCount))
    {
        m_result = levelsCount > 0 ? RET_OK : RET_LAST_LEVEL;
        logg << "Parse completed: OK (" << levelsCount << " levels parsed)";
        m_resultDescription = "No error";
    }
    else
    {
        m_result = RET_ERROR;
        m_resultDescription = QSTR(watcher.lastExceptionMsg());
        logg << "Parse completed: Failed (" << watcher.lastExceptionMsg() << ")";
    }
    emit finished(this);
}

void WorkerThreadFullAnalyser::onFLProgress(int progress)
{
    QMutexLocker locker(&m_mutex);
    emit this->progressChanged(progress, "Analysis in progress...");
}

bool WorkerThreadFullAnalyser::onFLDebug(FL::TSParser::DebugEvent event)
{
    QMutexLocker locker(&m_mutex);
    emit this->debugEvent(event);
    return true;
}
