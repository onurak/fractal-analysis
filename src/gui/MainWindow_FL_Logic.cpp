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

#include "GuiCommon.h"
#include "MainWindow.h"

void MainWindow::initializeFractalLibrary()
{
    logg.info("Initializing Fractal Library... ");
    watcher.setOption("safe_mode", "yes");
    watcher.setOption("marker", "abmarker");
    watcher.setOption("parser", "simple");
    watcher.setOption("threading_mode", "none");
    logg.info("Fractal library initialized OK");
}

void MainWindow::loadTimeSeries(QString fileName)
{
    if (fileName.isEmpty())
        return;
    logg.debug(STR("Loading time series from " + fileName + "... "));
    try
    {
        // Open the file
        FL::FileCSV file(STR(fileName));

        // Is column known from settings
        std::vector<std::string>::const_iterator
                i = std::find(file.header().begin(), file.header().end(),
                              env["TimeSeries"].valueOf("Column", "none"));
        int index =  i != file.header().end() ?  i-file.header().begin() : -2;

        // Try to guess column with data. If it's impossible - ask user
        if (index == -2)
            index = file.guessTimeSeries();
        if (index == -1)
        {
            // Prepare columns list
            QStringList columns;
            for (unsigned int i = 0; i < file.header().size(); i++)
                columns.append(QSTR(file.header()[i]));

            // Ask user
            bool ok;
            QString choosedColumn = QInputDialog::getItem(this,
                    tr("Load Time Series"),
                    tr("Wich column of file contains data?"),
                    columns, 0, false, &ok
                    );
            if (!ok) return;

            index = columns.indexOf(choosedColumn);
        }

        // Set new time series for FL.
        // Old Time Series and Parse Tree will be destroyed by watcher.
        FL::TimeSeries *ts = new FL::TimeSeries();
        file.loadTimeSeries(ts, index);
        watcher.setTimeSeries(ts);
        setStatus(tr("Time series loaded successful"));

        logg.log("OK");
    }
    /*
    catch (GException e)
    {
        logg.log(e.msg());
        showError(e);
    }
    */
    catch (...)
    {
        logg.log("Unknown error!");
        showError("Unknown error!");
    }
}

void MainWindow::loadPatterns(QString fileName)
{
    if (::loadPatterns(watcher, fileName))
        setStatus(QString("%1 patterns loaded").arg(watcher.patterns().size()));
    else
        showError(QString("Error loading patterns: ") + QSTR(watcher.lastExceptionMsg()));
}

void MainWindow::loadEnvironment()
{
    watcher.parseTreeSet().clear();
    watcher.loadFromEnv(env);
    loadTimeSeries(env["TimeSeries"].valueOf("FileName", ""));
    loadPatterns(env["Patterns"].valueOf("FileName", ""));
    readAnalysisProperties();
    setStatus("Successfuly loaded");
    timeSeriesChanged();
    draw();
    updateInfo();
}

void MainWindow::haltAnalysis()
{
    this->state = STATE_READY;
    if (currentWorker != NULL)
    {
        currentWorker->terminate();
        currentWorker->wait();
        delete currentWorker;
        currentWorker = NULL;
    }
    watcher.parseTreeSet().clear();
    updateInfo();
    draw();
}

FL::ParseTree* MainWindow::createRootsMarkupFromTree(FL::ParseTree *tree)
{
    FL::ParseTree *markup = new FL::ParseTree(&watcher.parseTreeSet());
    FL::ParseTree::ConstLayer roots = tree->roots();
    FL::ParseTree::ConstLayer::const_iterator node;
    for_each_(node, roots)
    {
        FL::ParseTreeNode *newNode = new FL::ParseTreeNode(
                (*node)->name, NULL, 0, (*node)->tsBegin, (*node)->tsEnd);
        markup->addNode(newNode);
    }

    return markup;
}
