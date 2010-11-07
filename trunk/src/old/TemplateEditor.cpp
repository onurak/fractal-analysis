#include "TemplateEditor.h"
#include "ui_TemplateEditor.h"

TemplateEditor::TemplateEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TemplateEditor)
{
    ui->setupUi(this);
}

TemplateEditor::~TemplateEditor()
{
    delete ui;
}

void TemplateEditor::changeEvent(QEvent *e)
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

void TemplateEditor::editTemplates(FL::Patterns *patterns)
{
    QMessageBox::information(this, "Template editor", "Sorry, don't work yet!");
    return;
    if (patterns == NULL)
        return;
    m_patterns = patterns;
    refreshTemplates();
    exec();
}

void TemplateEditor::refreshTemplates()
{
    ui->templatesList->clear();
    if (m_patterns != NULL) {
        FL::Patterns::iterator t;
        for_each_(t, *m_patterns)
            ui->templatesList->addItem(QSTR((*t)->name()));
    }
}

void TemplateEditor::on_bnAddTemplate_clicked()
{
    if (m_patterns == NULL)
        return;
    bool ok;
    QString patternName =
            QInputDialog::getText(this, "Add pattern", "Patterns name:", QLineEdit::Normal, "pattern", &ok);
    if (patternName.isEmpty() || !ok)
        return;
    FL::Pattern *p = new FL::Pattern(m_patterns->synonyms);
    p->setName(STR(patternName));
    m_patterns->push_back(p);
    refreshTemplates();
    ui->templatesList->setCurrentIndex(m_patterns->size() - 1);
}

void TemplateEditor::on_bnClose_clicked()
{
    close();
}

void TemplateEditor::on_templatesList_currentIndexChanged(int index)
{
    if (m_patterns == NULL)
        return;
    FL::Pattern *p = (*m_patterns)[index];
    ui->templateEditor->setText(QSTR(p->name()));
}
