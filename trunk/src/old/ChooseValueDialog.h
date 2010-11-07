#ifndef CHOOSEVALUEDIALOG_H
#define CHOOSEVALUEDIALOG_H

#include <QDialog>

namespace Ui {
    class ChooseValueDialog;
}

class ChooseValueDialog : public QDialog {
    Q_OBJECT
public:
    ChooseValueDialog(QWidget *parent = 0);
    ~ChooseValueDialog();

    int execute(const QString &title, const QStringList &values);
protected:
    void changeEvent(QEvent *e);

private:
    Ui::ChooseValueDialog *ui;
};

#endif // CHOOSEVALUEDIALOG_H
