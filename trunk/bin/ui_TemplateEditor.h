/********************************************************************************
** Form generated from reading UI file 'TemplateEditor.ui'
**
** Created: Sun 16. May 21:02:09 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEMPLATEEDITOR_H
#define UI_TEMPLATEEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TemplateEditor
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QComboBox *templatesList;
    QTextEdit *templateEditor;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *cbGuardsElement;
    QPushButton *bnAddGuardsElement;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout;
    QPushButton *bnAdd;
    QPushButton *bnAddFromFile;
    QPushButton *bnCompile;
    QPushButton *bnRemove;
    QSpacerItem *verticalSpacer_2;
    QPushButton *bnCompileAll;
    QSpacerItem *verticalSpacer;
    QPushButton *bnClose;

    void setupUi(QDialog *TemplateEditor)
    {
        if (TemplateEditor->objectName().isEmpty())
            TemplateEditor->setObjectName(QString::fromUtf8("TemplateEditor"));
        TemplateEditor->resize(454, 349);
        horizontalLayout = new QHBoxLayout(TemplateEditor);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        groupBox = new QGroupBox(TemplateEditor);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(328, 0));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        templatesList = new QComboBox(groupBox);
        templatesList->setObjectName(QString::fromUtf8("templatesList"));

        verticalLayout_2->addWidget(templatesList);

        templateEditor = new QTextEdit(groupBox);
        templateEditor->setObjectName(QString::fromUtf8("templateEditor"));

        verticalLayout_2->addWidget(templateEditor);

        groupBox_2 = new QGroupBox(groupBox);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(0, 30));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        cbGuardsElement = new QComboBox(groupBox_2);
        cbGuardsElement->setObjectName(QString::fromUtf8("cbGuardsElement"));

        horizontalLayout_2->addWidget(cbGuardsElement);

        bnAddGuardsElement = new QPushButton(groupBox_2);
        bnAddGuardsElement->setObjectName(QString::fromUtf8("bnAddGuardsElement"));
        bnAddGuardsElement->setMaximumSize(QSize(42, 16777215));

        horizontalLayout_2->addWidget(bnAddGuardsElement);


        verticalLayout_2->addWidget(groupBox_2);


        horizontalLayout->addWidget(groupBox);

        frame_2 = new QFrame(TemplateEditor);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame_2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        bnAdd = new QPushButton(frame_2);
        bnAdd->setObjectName(QString::fromUtf8("bnAdd"));

        verticalLayout->addWidget(bnAdd);

        bnAddFromFile = new QPushButton(frame_2);
        bnAddFromFile->setObjectName(QString::fromUtf8("bnAddFromFile"));

        verticalLayout->addWidget(bnAddFromFile);

        bnCompile = new QPushButton(frame_2);
        bnCompile->setObjectName(QString::fromUtf8("bnCompile"));

        verticalLayout->addWidget(bnCompile);

        bnRemove = new QPushButton(frame_2);
        bnRemove->setObjectName(QString::fromUtf8("bnRemove"));

        verticalLayout->addWidget(bnRemove);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        bnCompileAll = new QPushButton(frame_2);
        bnCompileAll->setObjectName(QString::fromUtf8("bnCompileAll"));

        verticalLayout->addWidget(bnCompileAll);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        bnClose = new QPushButton(frame_2);
        bnClose->setObjectName(QString::fromUtf8("bnClose"));

        verticalLayout->addWidget(bnClose);


        horizontalLayout->addWidget(frame_2);


        retranslateUi(TemplateEditor);

        QMetaObject::connectSlotsByName(TemplateEditor);
    } // setupUi

    void retranslateUi(QDialog *TemplateEditor)
    {
        TemplateEditor->setWindowTitle(QApplication::translate("TemplateEditor", "TemplateEditor", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("TemplateEditor", "Templates", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("TemplateEditor", "Guard's elements", 0, QApplication::UnicodeUTF8));
        bnAddGuardsElement->setText(QApplication::translate("TemplateEditor", "Add", 0, QApplication::UnicodeUTF8));
        bnAdd->setText(QApplication::translate("TemplateEditor", "Add", 0, QApplication::UnicodeUTF8));
        bnAddFromFile->setText(QApplication::translate("TemplateEditor", "Add From File", 0, QApplication::UnicodeUTF8));
        bnCompile->setText(QApplication::translate("TemplateEditor", "Compile", 0, QApplication::UnicodeUTF8));
        bnRemove->setText(QApplication::translate("TemplateEditor", "Remove", 0, QApplication::UnicodeUTF8));
        bnCompileAll->setText(QApplication::translate("TemplateEditor", "Compile All", 0, QApplication::UnicodeUTF8));
        bnClose->setText(QApplication::translate("TemplateEditor", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TemplateEditor: public Ui_TemplateEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEMPLATEEDITOR_H
