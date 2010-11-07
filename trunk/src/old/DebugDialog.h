#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "fractallib/FL.h"

namespace Ui {
    class DebugDialog;
}

class DebugDialog : public QDialog {
    Q_OBJECT
public:
    DebugDialog(QWidget *parent = 0, FL::Patterns *patterns = NULL);
    ~DebugDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DebugDialog *ui;
    FL::Patterns *m_patterns;
private slots:
    void on_pbAddTemplate_clicked();
    void on_bnClose_clicked();
    void on_bnGoTemplate_clicked();
    void on_bnGoPoliz_clicked();
};

#endif // DEBUGDIALOG_H
