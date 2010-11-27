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

#ifndef WORKERTHREADS_H
#define WORKERTHREADS_H

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaType>
#include "fractallib/FL.h"
#include "fractallib/flqt.h"

Q_DECLARE_METATYPE(FL::TSParser::DebugEvent);

class WorkerThread: public QThread
{
    Q_OBJECT
signals:
    void progressChanged(int progress = 0, QString msg = "");
    void finished(WorkerThread *thread);
    void debugEvent(FL::TSParser::DebugEvent event);
public:
    int result() const { return m_result; }
    QString resultDescription() { return m_resultDescription; }
    virtual void run() {}
protected:
    int m_result;
    QString m_resultDescription;
};

const int R_OK = -1;
const int R_ERROR = -2;
const int R_LAST_LEVEL = -3;

class WorkerThreadLevelAnalyser: public WorkerThread
{
public:
    WorkerThreadLevelAnalyser(FL::Watcher &watcher, int unmarkedSize, bool debugMode = false)
        : watcher(watcher), m_debugMode(debugMode)
    {
        qRegisterMetaType<FL::TSParser::DebugEvent>("FL::TSParser::DebugEvent");
        m_unmarkedSize = unmarkedSize;
    }
public:
    void run();
private:
    FL::Watcher &watcher;
    void onFLProgress(int progress);
    bool onFLDebug(FL::TSParser::DebugEvent event);
    bool m_debugMode;
    mutable QMutex m_mutex;
    int m_unmarkedSize;
};

class WorkerThreadFullAnalyser: public WorkerThread
{
public:
    WorkerThreadFullAnalyser(FL::Watcher &watcher, int unmarkedSize, bool debugMode = false)
        : watcher(watcher), m_debugMode(debugMode)
    {
        qRegisterMetaType<FL::TSParser::DebugEvent>("FL::TSParser::DebugEvent");
        m_unmarkedSize = unmarkedSize;
    }
public:
    void run();
private:
    FL::Watcher &watcher;
    void onFLProgress(int progress);
    bool onFLDebug(FL::TSParser::DebugEvent event);
    bool m_debugMode;
    int m_unmarkedSize;
    mutable QMutex m_mutex;
};

#endif // WORKERTHREADS_H
