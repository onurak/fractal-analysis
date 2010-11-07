#ifndef TEMPLATEEDITOR_H
#define TEMPLATEEDITOR_H

#include <QDialog>
#include <QInputDialog>
#include <QMessageBox>
#include "fractallib/FL.h"

namespace Ui {
    class TemplateEditor;
}

class TemplateEditor : public QDialog {
    Q_OBJECT
public:
    TemplateEditor(QWidget *parent = 0);
    ~TemplateEditor();

    void editTemplates(FL::Patterns *patterns);
protected:
    void changeEvent(QEvent *e);

private:
    Ui::TemplateEditor *ui;
    FL::Patterns *m_patterns;

    void refreshTemplates();
private slots:
    void on_templatesList_currentIndexChanged(int index);
    void on_bnClose_clicked();
    void on_bnAddTemplate_clicked();
};

#endif // TEMPLATEEDITOR_H
