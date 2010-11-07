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

#include "PatternEditor.h"
#include "ui_PatternEditor.h"

PatternEditor::PatternEditor(FL::Watcher &watcher, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PatternEditor),
    watcher(watcher)
{
    ui->setupUi(this);
    modified = isNewPattern = false;
    pattern = NULL;
    refreshPatterns();
    refreshGUI();
}

PatternEditor::~PatternEditor()
{
    delete ui;
}

void PatternEditor::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PatternEditor::refreshPatterns()
{
    ui->cbPattern->clear();
    ui->cbPattern->addItems(readPatternsNames());
    if (watcher.patterns().size() > 0)
        ui->cbPattern->setCurrentIndex(0);
}

void PatternEditor::refreshGUI()
{
    bool e = (this->pattern != NULL) || (watcher.patterns().size() > 0);
    ui->edDescription->setEnabled(e);
    ui->edGuard->setEnabled(e);
    ui->edPatternName->setEnabled(e);
    ui->bnAddSynonym->setEnabled(e);
    ui->bnRemSynonym->setEnabled(e);
    ui->listSynonyms->setEnabled(e);
    setModified(false);

    if (this->pattern != NULL)
    {
        ui->edPatternName->setText( QSTR(pattern->name()) );
        ui->edDescription->setText( QSTR(pattern->description()->sourceText()) );
        ui->edGuard->setText( QSTR(pattern->guard()->sourceText()) );
    }
}

QStringList PatternEditor::readPatternsNames()
{
    QStringList result;
    FL::PatternCollection::const_iterator pattern;
    for_each_(pattern, watcher.patterns())
        result.append(QSTR((*pattern)->name()));
    return result;
}

void PatternEditor::setModified(bool value)
{
    modified = value;
    ui->bnSavePattern->setEnabled(modified);
}

void PatternEditor::on_cbPattern_currentIndexChanged(int index)
{
    if (modified && maybeSave())
        saveCurrentPattern();
    int idx = ui->cbPattern->currentIndex();
    if (idx >= 0 && idx < (int)watcher.patterns().size())
    {
        pattern = watcher.patterns()[idx];
        isNewPattern = false;
    }
    refreshGUI();
}

bool PatternEditor::maybeSave()
{
    int ret = QMessageBox::question(this, tr("Save"), tr("Pattern was changed. Save changes?"),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    return ret == QMessageBox::Yes;
}

void PatternEditor::saveCurrentPattern()
{

}
