#include "ChooseValueDialog.h"
#include "ui_ChooseValueDialog.h"

ChooseValueDialog::ChooseValueDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseValueDialog)
{
    ui->setupUi(this);
}

ChooseValueDialog::~ChooseValueDialog()
{
    delete ui;
}

void ChooseValueDialog::changeEvent(QEvent *e)
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

int ChooseValueDialog::execute(const QString &title, const QStringList &values)
{
    ui->groupBox->setTitle(title);
    ui->listWidget->clear();
    ui->listWidget->addItems(values);
    if (exec())
        return ui->listWidget->currentRow();
    else
        return -1;
}
