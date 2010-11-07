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

#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include "fractallib/PatternGuardRPN.h"
#include "fractallib/GuardCompilerRPN.h"
#include "fractallib/PatternDescriptionEbnf.h"
#include "fractallib/flqt.h"

namespace Ui {
    class DebugWindow;
}

class DebugWindow : public QDialog {
    Q_OBJECT
public:
    DebugWindow(QWidget *parent = 0);
    ~DebugWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DebugWindow *ui;
    void createFakeCheckContext();
    void deleteFakeContext();
    FL::Patterns::CheckContext* m_fakeContext;
    void highlightGuardSyntax();
    bool m_nowHightlighting;
private slots:
    void on_guardCodeEditor_textChanged();
    void on_bnExecuteGuardTest_clicked();
};

#endif // DEBUGWINDOW_H
