/********************************************************************************
** Form generated from reading UI file 'PatternEditor.ui'
**
** Created: Sun 5. Dec 16:43:15 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PATTERNEDITOR_H
#define UI_PATTERNEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PatternEditor
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *cbPattern;
    QPushButton *bnSavePattern;
    QSpacerItem *horizontalSpacer;
    QPushButton *bnNewPattern;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QTextEdit *edDescription;
    QListWidget *listSynonyms;
    QLabel *label_3;
    QTextEdit *edGuard;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *edPatternName;
    QWidget *widget;
    QPushButton *bnAddSynonym;
    QPushButton *bnRemSynonym;
    QFrame *line;

    void setupUi(QDialog *PatternEditor)
    {
        if (PatternEditor->objectName().isEmpty())
            PatternEditor->setObjectName(QString::fromUtf8("PatternEditor"));
        PatternEditor->resize(597, 454);
        verticalLayout = new QVBoxLayout(PatternEditor);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame_2 = new QFrame(PatternEditor);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMaximumSize(QSize(16777215, 40));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(frame_2);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        cbPattern = new QComboBox(frame_2);
        cbPattern->setObjectName(QString::fromUtf8("cbPattern"));
        cbPattern->setMinimumSize(QSize(250, 0));

        horizontalLayout->addWidget(cbPattern);

        bnSavePattern = new QPushButton(frame_2);
        bnSavePattern->setObjectName(QString::fromUtf8("bnSavePattern"));

        horizontalLayout->addWidget(bnSavePattern);

        horizontalSpacer = new QSpacerItem(96, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        bnNewPattern = new QPushButton(frame_2);
        bnNewPattern->setObjectName(QString::fromUtf8("bnNewPattern"));

        horizontalLayout->addWidget(bnNewPattern);


        verticalLayout->addWidget(frame_2);

        frame = new QFrame(PatternEditor);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        edDescription = new QTextEdit(frame);
        edDescription->setObjectName(QString::fromUtf8("edDescription"));

        gridLayout->addWidget(edDescription, 1, 0, 3, 1);

        listSynonyms = new QListWidget(frame);
        listSynonyms->setObjectName(QString::fromUtf8("listSynonyms"));
        listSynonyms->setMinimumSize(QSize(150, 0));
        listSynonyms->setMaximumSize(QSize(150, 16777215));

        gridLayout->addWidget(listSynonyms, 3, 2, 3, 3);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        edGuard = new QTextEdit(frame);
        edGuard->setObjectName(QString::fromUtf8("edGuard"));

        gridLayout->addWidget(edGuard, 5, 0, 2, 1);

        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMaximumSize(QSize(16777215, 13));

        gridLayout->addWidget(label_4, 2, 2, 1, 1);

        label_5 = new QLabel(frame);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 0, 2, 1, 1);

        edPatternName = new QLineEdit(frame);
        edPatternName->setObjectName(QString::fromUtf8("edPatternName"));
        edPatternName->setMinimumSize(QSize(150, 0));
        edPatternName->setMaximumSize(QSize(150, 16777215));

        gridLayout->addWidget(edPatternName, 1, 2, 1, 1);

        widget = new QWidget(frame);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMaximumSize(QSize(16777215, 30));
        bnAddSynonym = new QPushButton(widget);
        bnAddSynonym->setObjectName(QString::fromUtf8("bnAddSynonym"));
        bnAddSynonym->setGeometry(QRect(31, 4, 30, 23));
        bnAddSynonym->setMaximumSize(QSize(30, 16777215));
        bnRemSynonym = new QPushButton(widget);
        bnRemSynonym->setObjectName(QString::fromUtf8("bnRemSynonym"));
        bnRemSynonym->setGeometry(QRect(89, 4, 30, 23));
        bnRemSynonym->setMaximumSize(QSize(30, 16777215));

        gridLayout->addWidget(widget, 6, 2, 1, 1);

        line = new QFrame(frame);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 0, 1, 7, 1);


        verticalLayout->addWidget(frame);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(cbPattern);
        label_2->setBuddy(edDescription);
        label_3->setBuddy(edGuard);
        label_4->setBuddy(listSynonyms);
        label_5->setBuddy(edPatternName);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(cbPattern, edDescription);
        QWidget::setTabOrder(edDescription, edGuard);
        QWidget::setTabOrder(edGuard, bnSavePattern);
        QWidget::setTabOrder(bnSavePattern, bnNewPattern);

        retranslateUi(PatternEditor);

        QMetaObject::connectSlotsByName(PatternEditor);
    } // setupUi

    void retranslateUi(QDialog *PatternEditor)
    {
        PatternEditor->setWindowTitle(QApplication::translate("PatternEditor", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PatternEditor", "&Pattern", 0, QApplication::UnicodeUTF8));
        bnSavePattern->setText(QApplication::translate("PatternEditor", "Save", 0, QApplication::UnicodeUTF8));
        bnNewPattern->setText(QApplication::translate("PatternEditor", "New pattern", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PatternEditor", "&Description code:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PatternEditor", "&Guard code:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("PatternEditor", "&Synonyms:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("PatternEditor", "&Name", 0, QApplication::UnicodeUTF8));
        bnAddSynonym->setText(QApplication::translate("PatternEditor", "+", 0, QApplication::UnicodeUTF8));
        bnRemSynonym->setText(QApplication::translate("PatternEditor", "-", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PatternEditor: public Ui_PatternEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PATTERNEDITOR_H
