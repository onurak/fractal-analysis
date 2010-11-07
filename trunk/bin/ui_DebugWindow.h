/********************************************************************************
** Form generated from reading UI file 'DebugWindow.ui'
**
** Created: Sun 7. Nov 00:57:42 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGWINDOW_H
#define UI_DEBUGWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DebugWindow
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tabGuardTesting;
    QFormLayout *formLayout;
    QLabel *label;
    QComboBox *comboBox;
    QLabel *label_2;
    QTextEdit *guardCodeEditor;
    QPushButton *bnExecuteGuardTest;
    QLabel *lbGuardExecutionResult;

    void setupUi(QDialog *DebugWindow)
    {
        if (DebugWindow->objectName().isEmpty())
            DebugWindow->setObjectName(QString::fromUtf8("DebugWindow"));
        DebugWindow->setWindowModality(Qt::ApplicationModal);
        DebugWindow->resize(458, 308);
        verticalLayout = new QVBoxLayout(DebugWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(DebugWindow);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabGuardTesting = new QWidget();
        tabGuardTesting->setObjectName(QString::fromUtf8("tabGuardTesting"));
        formLayout = new QFormLayout(tabGuardTesting);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(tabGuardTesting);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        comboBox = new QComboBox(tabGuardTesting);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setEnabled(false);

        formLayout->setWidget(0, QFormLayout::FieldRole, comboBox);

        label_2 = new QLabel(tabGuardTesting);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        guardCodeEditor = new QTextEdit(tabGuardTesting);
        guardCodeEditor->setObjectName(QString::fromUtf8("guardCodeEditor"));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier"));
        font.setPointSize(10);
        guardCodeEditor->setFont(font);

        formLayout->setWidget(2, QFormLayout::SpanningRole, guardCodeEditor);

        bnExecuteGuardTest = new QPushButton(tabGuardTesting);
        bnExecuteGuardTest->setObjectName(QString::fromUtf8("bnExecuteGuardTest"));
        bnExecuteGuardTest->setMaximumSize(QSize(100, 16777215));

        formLayout->setWidget(3, QFormLayout::LabelRole, bnExecuteGuardTest);

        lbGuardExecutionResult = new QLabel(tabGuardTesting);
        lbGuardExecutionResult->setObjectName(QString::fromUtf8("lbGuardExecutionResult"));
        lbGuardExecutionResult->setTextFormat(Qt::RichText);

        formLayout->setWidget(3, QFormLayout::FieldRole, lbGuardExecutionResult);

        tabWidget->addTab(tabGuardTesting, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(DebugWindow);

        tabWidget->setCurrentIndex(0);
        comboBox->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(DebugWindow);
    } // setupUi

    void retranslateUi(QDialog *DebugWindow)
    {
        DebugWindow->setWindowTitle(QApplication::translate("DebugWindow", "Debug Window", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DebugWindow", "Guard type", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DebugWindow", "Expression:", 0, QApplication::UnicodeUTF8));
        bnExecuteGuardTest->setText(QApplication::translate("DebugWindow", "Execute", 0, QApplication::UnicodeUTF8));
        lbGuardExecutionResult->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabGuardTesting), QApplication::translate("DebugWindow", "Guard testing", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DebugWindow: public Ui_DebugWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGWINDOW_H
