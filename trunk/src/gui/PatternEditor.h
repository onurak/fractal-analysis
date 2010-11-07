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

#ifndef PATTERNEDITOR_H
#define PATTERNEDITOR_H

#include <QDialog>
#include <QMessageBox>
#include "fractallib/FL.h"
#include "fractallib/flqt.h"

namespace Ui {
    class PatternEditor;
}

class PatternEditor : public QDialog {
    Q_OBJECT
public:
    PatternEditor(FL::Watcher &watcher, QWidget *parent = 0);
    ~PatternEditor();
protected:
    void changeEvent(QEvent *e);
private:
    void refreshPatterns();
    void refreshGUI();
    QStringList readPatternsNames();
    void setModified(bool value);
    bool maybeSave();
    void saveCurrentPattern();
private:
    Ui::PatternEditor *ui;
    FL::Watcher &watcher;
    bool modified;
    FL::Pattern *pattern;
    bool isNewPattern;

private slots:
    void on_cbPattern_currentIndexChanged(int index);
};

#endif // PATTERNEDITOR_H
