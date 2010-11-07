/********************************************************************************
** Form generated from reading UI file 'ChooseValueDialog.ui'
**
** Created: Sun 16. May 21:02:09 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHOOSEVALUEDIALOG_H
#define UI_CHOOSEVALUEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_ChooseValueDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *listWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ChooseValueDialog)
    {
        if (ChooseValueDialog->objectName().isEmpty())
            ChooseValueDialog->setObjectName(QString::fromUtf8("ChooseValueDialog"));
        ChooseValueDialog->resize(319, 240);
        horizontalLayout = new QHBoxLayout(ChooseValueDialog);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        groupBox = new QGroupBox(ChooseValueDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        horizontalLayout_2 = new QHBoxLayout(groupBox);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        listWidget = new QListWidget(groupBox);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        horizontalLayout_2->addWidget(listWidget);


        horizontalLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(ChooseValueDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        retranslateUi(ChooseValueDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), ChooseValueDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), ChooseValueDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(ChooseValueDialog);
    } // setupUi

    void retranslateUi(QDialog *ChooseValueDialog)
    {
        ChooseValueDialog->setWindowTitle(QApplication::translate("ChooseValueDialog", "Choose value", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ChooseValueDialog", "GroupBox", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ChooseValueDialog: public Ui_ChooseValueDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHOOSEVALUEDIALOG_H
