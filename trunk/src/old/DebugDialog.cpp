#include "DebugDialog.h"
#include "ui_DebugDialog.h"

DebugDialog::DebugDialog(QWidget *parent, FL::Patterns *patterns) :
    QDialog(parent),
    ui(new Ui::DebugDialog),
    m_patterns(patterns)
{
    ui->setupUi(this);
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

void DebugDialog::changeEvent(QEvent *e)
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

void DebugDialog::on_bnGoPoliz_clicked()
{
    using FL::POLIZ::Compiler;
    using FL::POLIZ::Program;

    Program program;
    Compiler compiler;
    //ui->lbPolizError->setEnabled(true);

    try {
        if (!compiler.compile(STR(ui->edPoliz->text()), program)) {
            QMessageBox::critical(this, "Compile error", QString("Error pos: %1").arg(compiler.errorPos));
            ui->edPoliz->setCursorPosition(compiler.errorPos);
        }

        FL::ParseContext context;
        GVariant result = program.execute(context);
        if (program.lastError() == Program::NO_ERROR) {
            QMessageBox::information(this, "Execution result", (char*)result);
            //ui->lbPolizError->setEnabled(false);
        } else
            QMessageBox::critical(this, "Execution error", QSTR(program.lastErrorDescription()));
    } catch (GException e) {
        QMessageBox::critical(this, "Exception catched", QSTR(e.msg()));
    } catch (...) {
        QMessageBox::critical(this, "Exception catched", "Unknown exception - do something!");
    }
}

void DebugDialog::on_bnGoTemplate_clicked()
{
    if (ui->edTemplate->text().isEmpty() || ui->edSequence->text().isEmpty())
        return;
    try {
        using namespace FL::EBNF;
        // разбираем входную цепочку на слова
        QStringList list = ui->edSequence->text().split(QRegExp("\\s+"));
        QStringListIterator name(list);
        Expression expr;
        while (name.hasNext())
            expr.items.push_back(new FL::ParseTreeNode(name.next().toStdString()) );

        // компилируем строчку EBNF-выражения
        Ebnf ebnf;
        FL::EBNF::Compiler compiler;
        if (!compiler.compile(STR(ui->edTemplate->text()), &ebnf))
            QMessageBox::critical(this, "Compilation error", QString("At pos %1").arg(compiler.errorPos));

        // записываем полченный EBNF в файл
        ofstream f("ebnf.txt");
        f << ebnf.print(ebnf.root());
        f.close();

        // назначаем опции
        Ebnf::CheckOptions options = Ebnf::coNothing;
        if (ui->cbParseWholeExpression->isChecked())
            options = Ebnf::coParseWholeExpression;

        FL::SynonymsTable synonyms;

        // проверяем на принадлежность
        if (ebnf.check(&expr, synonyms, options)) {
            string resultSeq;
            for (int i = 0; i < int(expr.items.size()); i++)
                resultSeq += expr.items[i]->asString() + " ";
            QMessageBox::information(this, "Check result",
                QString("Sequence checked successful!\nResulting sequence:\n") + QSTR(resultSeq));
        } else
            QMessageBox::critical(this, "Check result", "Sequence checked: invalid sequence");

        // совобождаем память
        while (expr.items.size() > 0) {
            delete expr.items[expr.items.size() - 1];
            expr.items.pop_back();
        }
    } catch (GException e) {
        QMessageBox::critical(this, "Exception catched", QSTR(e.msg()));
    } catch (...) {
        QMessageBox::critical(this, "Exception catched", "Unknown exception - do something!");
    }
}

void DebugDialog::on_bnClose_clicked()
{
    close();
}

void DebugDialog::on_pbAddTemplate_clicked()
{
    FL::FilePAT parser;
    try {
        parser.loadFromString(
            STR(ui->edAddTemplate->text()), m_patterns);
        string msg = "Template added";

        if (m_patterns && m_patterns->size())
            msg += ".\nEbnf: \n" + (*m_patterns)[m_patterns->size()-1]->bnf()->print();
        QMessageBox::information(this, "Info", QSTR(msg));
    } catch (FL::EParse e) {
        QMessageBox::critical(this, "Exception", QSTR(e.msg()));
    }
}
