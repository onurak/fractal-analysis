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

#include "DebugWindow.h"
#include "ui_DebugWindow.h"

DebugWindow::DebugWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);
    m_fakeContext = NULL;
    m_nowHightlighting = false;
}

DebugWindow::~DebugWindow()
{
    deleteFakeContext();
    delete ui;
}

void DebugWindow::changeEvent(QEvent *e)
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

void DebugWindow::createFakeCheckContext()
{
    deleteFakeContext();
    FL::ParseTree::Layer *roots = new FL::ParseTree::Layer();
    roots->push_back(new FL::ParseTreeNode("a"));
    FL::ParseTree::ConstLayer::const_iterator iRoot = roots->begin();
    FL::ParseTree::Layer *modification = new FL::ParseTree::Layer();
    FL::ParseTree::Layer *lastParsed = new FL::ParseTree::Layer();
    FL::SynonymsTable *table = new FL::SynonymsTable();
    FL::Patterns::ParameterSet *params = new FL::Patterns::ParameterSet();
    FL::TimeSeries *ts = new FL::TimeSeries();


    m_fakeContext = new FL::Patterns::CheckContext(
             roots, iRoot, *modification, *lastParsed, table, params, ts);
}

void DebugWindow::deleteFakeContext()
{
    if (m_fakeContext)
    {
        FL::ParseTree::ConstLayer::const_iterator node;
        for_each_(node, *(m_fakeContext->roots))
            delete *node;
        delete m_fakeContext->roots;
        delete &(m_fakeContext->modification);
        delete &(m_fakeContext->lastParsed);
        delete m_fakeContext->synonyms;
        delete m_fakeContext->parameters;
        delete m_fakeContext->ts;
        delete m_fakeContext;
        m_fakeContext = NULL;
    }
}

void DebugWindow::on_bnExecuteGuardTest_clicked()
{
    using namespace FL;
    using namespace FL::Patterns;
    /*
    DescriptionStructure *ds  = new EBNF::EbnfStructure();
    DescriptionCompiler *dcmp = new EBNF::EbnfCompiler();
    Description
    Description *description  = new EBNF::De

    GuardStructure *structure = new RPN::GuardStructureRPN();
    GuardCompiler *compiler   = new RPN::GuardCompilerRPN();
    GuardChecker *checker     = new RPN::GuardCheckerRPN(structure);
    FL::Patterns::Guard *guard = new FL::Patterns::Guard(fakeDescription, structure, checker, compiler);
    */

    //compiler->compile(STR)

    std::string sourceText = STR(ui->guardCodeEditor->toPlainText());
    RPN::GuardStructureRPN structure;
    RPN::GuardCompilerRPN compiler;
    RPN::GuardCheckerRPN checker(&structure);

    if (!compiler.compile(sourceText, structure))
    {
        ui->lbGuardExecutionResult->setText("<font color=#AA0000>Compilation error</font>");
        return;
    }

    createFakeCheckContext();
    checker.execute(*m_fakeContext);
    QString resultMsg;
    if (checker.lastError() == RPN::GuardCheckerRPN::NO_ERROR)
        resultMsg = QString("<font color=#00AA00>Check OK. Result: %1</font>").arg((char*)checker.lastResult());
    else
        resultMsg = QString("<font color=#AA0000>Check error: %1</font>").arg(QSTR(checker.lastErrorDescription()));
    ui->lbGuardExecutionResult->setText(resultMsg);
}

void DebugWindow::on_guardCodeEditor_textChanged()
{
    //if (!m_nowHightlighting)
      //  highlightGuardSyntax();
}

void DebugWindow::highlightGuardSyntax()
{
    m_nowHightlighting = true;
    QString text = ui->guardCodeEditor->toPlainText();
    text.replace("if", "<b>if<b>");
    text.replace("then", "<b>then<b>");
    text.replace("else", "<b>else<b>");
    ui->guardCodeEditor->setHtml(text);
    m_nowHightlighting = false;
}
