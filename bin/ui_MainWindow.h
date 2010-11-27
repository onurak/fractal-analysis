/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created: Sun 21. Nov 18:40:12 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen_Time_Series;
    QAction *actionOpen_Patterns;
    QAction *actionOpen_Environment;
    QAction *actionNext_level;
    QAction *actionBuild_trees;
    QAction *actionGenerate_time_series;
    QAction *actionSettings;
    QAction *action_Quit;
    QAction *actionZoom_in;
    QAction *actionZoom_out;
    QAction *action_Debug_window;
    QAction *action_Fit_time_series;
    QAction *actionCreate_markup_from_roots;
    QAction *actionShow_Analysys_properties;
    QAction *actionOpen_DTS_from_file;
    QAction *actionDynamic_Time_Series_Run;
    QAction *actionDynamic_Time_Series_Stop;
    QAction *actionDynamic_Time_Series_Step;
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QScrollBar *timeSeriesHorizontalScroller;
    QGroupBox *gbInfo;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *sbTreeNo;
    QLabel *label_2;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QLabel *label_3;
    QLabel *lbTimeSeriesSize;
    QLabel *label_4;
    QLabel *lbPatternsCount;
    QLabel *label_5;
    QLabel *lbState;
    QPushButton *bnClearParseTrees;
    QLabel *lbTreesCount;
    QLabel *label_6;
    QLabel *lbMemory;
    QPushButton *bnHaltAnalysis;
    QSpacerItem *horizontalSpacer_2;
    QScrollBar *timeSeriesVerticalScroller;
    QStatusBar *statusbar;
    QToolBar *toolBar;
    QDockWidget *analysisPropertiesWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QPushButton *bnEditPatterns;
    QPushButton *bnClearPatterns;
    QLabel *label_7;
    QComboBox *cbTSMarker;
    QLabel *label_8;
    QComboBox *cbTSParser;
    QGroupBox *gbDrawing;
    QVBoxLayout *verticalLayout;
    QCheckBox *cbShowParseTreeNodes;
    QCheckBox *cbShowParseTreeTimeSeries;
    QCheckBox *cbShowParseTreeRoots;
    QSpacerItem *verticalSpacer_2;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menuOpen_dynamic_time_series;
    QMenu *menu_Analysis;
    QMenu *menu_Tests;
    QMenu *menu_View;
    QMenu *menuFilters;
    QMenu *menuTime_series;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(771, 469);
        MainWindow->setMinimumSize(QSize(0, 0));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/resources/logotype.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        actionOpen_Time_Series = new QAction(MainWindow);
        actionOpen_Time_Series->setObjectName(QString::fromUtf8("actionOpen_Time_Series"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/resources/open_time_series.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_Time_Series->setIcon(icon1);
        actionOpen_Patterns = new QAction(MainWindow);
        actionOpen_Patterns->setObjectName(QString::fromUtf8("actionOpen_Patterns"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/resources/open_patterns.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_Patterns->setIcon(icon2);
        actionOpen_Environment = new QAction(MainWindow);
        actionOpen_Environment->setObjectName(QString::fromUtf8("actionOpen_Environment"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/resources/open_environment.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_Environment->setIcon(icon3);
        actionNext_level = new QAction(MainWindow);
        actionNext_level->setObjectName(QString::fromUtf8("actionNext_level"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/resources/next_fractal_level.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNext_level->setIcon(icon4);
        actionBuild_trees = new QAction(MainWindow);
        actionBuild_trees->setObjectName(QString::fromUtf8("actionBuild_trees"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/resources/build_trees.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionBuild_trees->setIcon(icon5);
        actionGenerate_time_series = new QAction(MainWindow);
        actionGenerate_time_series->setObjectName(QString::fromUtf8("actionGenerate_time_series"));
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName(QString::fromUtf8("actionSettings"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/resources/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSettings->setIcon(icon6);
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        actionZoom_in = new QAction(MainWindow);
        actionZoom_in->setObjectName(QString::fromUtf8("actionZoom_in"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/resources/zoom_in.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_in->setIcon(icon7);
        actionZoom_out = new QAction(MainWindow);
        actionZoom_out->setObjectName(QString::fromUtf8("actionZoom_out"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/resources/zoom_out.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_out->setIcon(icon8);
        action_Debug_window = new QAction(MainWindow);
        action_Debug_window->setObjectName(QString::fromUtf8("action_Debug_window"));
        action_Fit_time_series = new QAction(MainWindow);
        action_Fit_time_series->setObjectName(QString::fromUtf8("action_Fit_time_series"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/resources/fit_time_series.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Fit_time_series->setIcon(icon9);
        actionCreate_markup_from_roots = new QAction(MainWindow);
        actionCreate_markup_from_roots->setObjectName(QString::fromUtf8("actionCreate_markup_from_roots"));
        actionShow_Analysys_properties = new QAction(MainWindow);
        actionShow_Analysys_properties->setObjectName(QString::fromUtf8("actionShow_Analysys_properties"));
        actionShow_Analysys_properties->setCheckable(true);
        actionOpen_DTS_from_file = new QAction(MainWindow);
        actionOpen_DTS_from_file->setObjectName(QString::fromUtf8("actionOpen_DTS_from_file"));
        actionDynamic_Time_Series_Run = new QAction(MainWindow);
        actionDynamic_Time_Series_Run->setObjectName(QString::fromUtf8("actionDynamic_Time_Series_Run"));
        actionDynamic_Time_Series_Stop = new QAction(MainWindow);
        actionDynamic_Time_Series_Stop->setObjectName(QString::fromUtf8("actionDynamic_Time_Series_Stop"));
        actionDynamic_Time_Series_Step = new QAction(MainWindow);
        actionDynamic_Time_Series_Step->setObjectName(QString::fromUtf8("actionDynamic_Time_Series_Step"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        timeSeriesHorizontalScroller = new QScrollBar(centralwidget);
        timeSeriesHorizontalScroller->setObjectName(QString::fromUtf8("timeSeriesHorizontalScroller"));
        timeSeriesHorizontalScroller->setEnabled(false);
        timeSeriesHorizontalScroller->setMaximum(0);
        timeSeriesHorizontalScroller->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(timeSeriesHorizontalScroller, 1, 0, 1, 1);

        gbInfo = new QGroupBox(centralwidget);
        gbInfo->setObjectName(QString::fromUtf8("gbInfo"));
        gbInfo->setMinimumSize(QSize(0, 100));
        gbInfo->setMaximumSize(QSize(16777215, 100));
        gridLayout = new QGridLayout(gbInfo);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(gbInfo);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        sbTreeNo = new QSpinBox(gbInfo);
        sbTreeNo->setObjectName(QString::fromUtf8("sbTreeNo"));
        sbTreeNo->setMinimumSize(QSize(70, 0));
        sbTreeNo->setMinimum(1);

        gridLayout->addWidget(sbTreeNo, 0, 1, 1, 1);

        label_2 = new QLabel(gbInfo);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setScaledContents(false);
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 0, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 7, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 5, 6, 1, 1);

        label_3 = new QLabel(gbInfo);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 8, 1, 1);

        lbTimeSeriesSize = new QLabel(gbInfo);
        lbTimeSeriesSize->setObjectName(QString::fromUtf8("lbTimeSeriesSize"));

        gridLayout->addWidget(lbTimeSeriesSize, 0, 9, 1, 1);

        label_4 = new QLabel(gbInfo);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 5, 8, 1, 1);

        lbPatternsCount = new QLabel(gbInfo);
        lbPatternsCount->setObjectName(QString::fromUtf8("lbPatternsCount"));

        gridLayout->addWidget(lbPatternsCount, 5, 9, 1, 1);

        label_5 = new QLabel(gbInfo);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        lbState = new QLabel(gbInfo);
        lbState->setObjectName(QString::fromUtf8("lbState"));

        gridLayout->addWidget(lbState, 5, 1, 1, 1);

        bnClearParseTrees = new QPushButton(gbInfo);
        bnClearParseTrees->setObjectName(QString::fromUtf8("bnClearParseTrees"));
        bnClearParseTrees->setMaximumSize(QSize(40, 18));

        gridLayout->addWidget(bnClearParseTrees, 0, 4, 1, 1);

        lbTreesCount = new QLabel(gbInfo);
        lbTreesCount->setObjectName(QString::fromUtf8("lbTreesCount"));

        gridLayout->addWidget(lbTreesCount, 0, 3, 1, 1);

        label_6 = new QLabel(gbInfo);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 6, 0, 1, 1);

        lbMemory = new QLabel(gbInfo);
        lbMemory->setObjectName(QString::fromUtf8("lbMemory"));

        gridLayout->addWidget(lbMemory, 6, 1, 1, 1);

        bnHaltAnalysis = new QPushButton(gbInfo);
        bnHaltAnalysis->setObjectName(QString::fromUtf8("bnHaltAnalysis"));
        bnHaltAnalysis->setMaximumSize(QSize(40, 18));

        gridLayout->addWidget(bnHaltAnalysis, 5, 4, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 5, 5, 1, 1);


        gridLayout_2->addWidget(gbInfo, 2, 0, 1, 2);

        timeSeriesVerticalScroller = new QScrollBar(centralwidget);
        timeSeriesVerticalScroller->setObjectName(QString::fromUtf8("timeSeriesVerticalScroller"));
        timeSeriesVerticalScroller->setEnabled(false);
        timeSeriesVerticalScroller->setMaximum(0);
        timeSeriesVerticalScroller->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(timeSeriesVerticalScroller, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        analysisPropertiesWidget = new QDockWidget(MainWindow);
        analysisPropertiesWidget->setObjectName(QString::fromUtf8("analysisPropertiesWidget"));
        analysisPropertiesWidget->setMinimumSize(QSize(200, 308));
        analysisPropertiesWidget->setMaximumSize(QSize(200, 524287));
        analysisPropertiesWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        verticalLayout_2 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(0, 0));
        groupBox->setMaximumSize(QSize(200, 16777215));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        bnEditPatterns = new QPushButton(groupBox);
        bnEditPatterns->setObjectName(QString::fromUtf8("bnEditPatterns"));
        bnEditPatterns->setEnabled(true);

        horizontalLayout->addWidget(bnEditPatterns);

        bnClearPatterns = new QPushButton(groupBox);
        bnClearPatterns->setObjectName(QString::fromUtf8("bnClearPatterns"));

        horizontalLayout->addWidget(bnClearPatterns);


        verticalLayout_2->addWidget(groupBox);

        label_7 = new QLabel(dockWidgetContents);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setFrameShape(QFrame::NoFrame);

        verticalLayout_2->addWidget(label_7);

        cbTSMarker = new QComboBox(dockWidgetContents);
        cbTSMarker->setObjectName(QString::fromUtf8("cbTSMarker"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cbTSMarker->sizePolicy().hasHeightForWidth());
        cbTSMarker->setSizePolicy(sizePolicy);
        cbTSMarker->setMinimumSize(QSize(150, 0));
        cbTSMarker->setMaximumSize(QSize(200, 16777215));

        verticalLayout_2->addWidget(cbTSMarker);

        label_8 = new QLabel(dockWidgetContents);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setFrameShape(QFrame::NoFrame);

        verticalLayout_2->addWidget(label_8);

        cbTSParser = new QComboBox(dockWidgetContents);
        cbTSParser->setObjectName(QString::fromUtf8("cbTSParser"));
        sizePolicy.setHeightForWidth(cbTSParser->sizePolicy().hasHeightForWidth());
        cbTSParser->setSizePolicy(sizePolicy);
        cbTSParser->setMinimumSize(QSize(150, 0));
        cbTSParser->setMaximumSize(QSize(200, 16777215));

        verticalLayout_2->addWidget(cbTSParser);

        gbDrawing = new QGroupBox(dockWidgetContents);
        gbDrawing->setObjectName(QString::fromUtf8("gbDrawing"));
        gbDrawing->setMinimumSize(QSize(0, 90));
        gbDrawing->setMaximumSize(QSize(200, 16777215));
        verticalLayout = new QVBoxLayout(gbDrawing);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        cbShowParseTreeNodes = new QCheckBox(gbDrawing);
        cbShowParseTreeNodes->setObjectName(QString::fromUtf8("cbShowParseTreeNodes"));
        cbShowParseTreeNodes->setMaximumSize(QSize(200, 16777215));

        verticalLayout->addWidget(cbShowParseTreeNodes);

        cbShowParseTreeTimeSeries = new QCheckBox(gbDrawing);
        cbShowParseTreeTimeSeries->setObjectName(QString::fromUtf8("cbShowParseTreeTimeSeries"));

        verticalLayout->addWidget(cbShowParseTreeTimeSeries);

        cbShowParseTreeRoots = new QCheckBox(gbDrawing);
        cbShowParseTreeRoots->setObjectName(QString::fromUtf8("cbShowParseTreeRoots"));

        verticalLayout->addWidget(cbShowParseTreeRoots);


        verticalLayout_2->addWidget(gbDrawing);

        verticalSpacer_2 = new QSpacerItem(20, 97, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        analysisPropertiesWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), analysisPropertiesWidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 771, 21));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menuOpen_dynamic_time_series = new QMenu(menu_File);
        menuOpen_dynamic_time_series->setObjectName(QString::fromUtf8("menuOpen_dynamic_time_series"));
        menu_Analysis = new QMenu(menubar);
        menu_Analysis->setObjectName(QString::fromUtf8("menu_Analysis"));
        menu_Tests = new QMenu(menubar);
        menu_Tests->setObjectName(QString::fromUtf8("menu_Tests"));
        menu_View = new QMenu(menubar);
        menu_View->setObjectName(QString::fromUtf8("menu_View"));
        menuFilters = new QMenu(menubar);
        menuFilters->setObjectName(QString::fromUtf8("menuFilters"));
        menuTime_series = new QMenu(menubar);
        menuTime_series->setObjectName(QString::fromUtf8("menuTime_series"));
        MainWindow->setMenuBar(menubar);

        toolBar->addAction(actionOpen_Environment);
        toolBar->addAction(actionOpen_Time_Series);
        toolBar->addAction(actionOpen_Patterns);
        toolBar->addSeparator();
        toolBar->addAction(actionNext_level);
        toolBar->addAction(actionBuild_trees);
        toolBar->addSeparator();
        toolBar->addAction(actionZoom_in);
        toolBar->addAction(actionZoom_out);
        toolBar->addAction(action_Fit_time_series);
        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_View->menuAction());
        menubar->addAction(menu_Analysis->menuAction());
        menubar->addAction(menuTime_series->menuAction());
        menubar->addAction(menuFilters->menuAction());
        menubar->addAction(menu_Tests->menuAction());
        menu_File->addAction(actionOpen_Environment);
        menu_File->addAction(actionOpen_Patterns);
        menu_File->addAction(actionOpen_Time_Series);
        menu_File->addAction(menuOpen_dynamic_time_series->menuAction());
        menu_File->addSeparator();
        menu_File->addAction(action_Quit);
        menuOpen_dynamic_time_series->addAction(actionOpen_DTS_from_file);
        menu_Analysis->addAction(actionNext_level);
        menu_Analysis->addAction(actionBuild_trees);
        menu_Analysis->addAction(actionCreate_markup_from_roots);
        menu_Tests->addAction(actionGenerate_time_series);
        menu_View->addAction(actionZoom_in);
        menu_View->addAction(actionZoom_out);
        menu_View->addAction(action_Fit_time_series);
        menu_View->addSeparator();
        menu_View->addAction(actionShow_Analysys_properties);
        menu_View->addAction(action_Debug_window);
        menuTime_series->addAction(actionDynamic_Time_Series_Run);
        menuTime_series->addAction(actionDynamic_Time_Series_Step);
        menuTime_series->addAction(actionDynamic_Time_Series_Stop);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Fractal Analyser", 0, QApplication::UnicodeUTF8));
        actionOpen_Time_Series->setText(QApplication::translate("MainWindow", "Open &time series", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionOpen_Time_Series->setToolTip(QApplication::translate("MainWindow", "Open file with time series", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOpen_Time_Series->setShortcut(QApplication::translate("MainWindow", "Ctrl+T", 0, QApplication::UnicodeUTF8));
        actionOpen_Patterns->setText(QApplication::translate("MainWindow", "Open &patterns", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionOpen_Patterns->setToolTip(QApplication::translate("MainWindow", "Open file with patterns", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOpen_Patterns->setShortcut(QApplication::translate("MainWindow", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        actionOpen_Environment->setText(QApplication::translate("MainWindow", "Open &environment", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionOpen_Environment->setToolTip(QApplication::translate("MainWindow", "Open file with environment", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOpen_Environment->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionNext_level->setText(QApplication::translate("MainWindow", "Next &level", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionNext_level->setToolTip(QApplication::translate("MainWindow", "Look for next fractal level", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionNext_level->setShortcut(QApplication::translate("MainWindow", "Ctrl+L", 0, QApplication::UnicodeUTF8));
        actionBuild_trees->setText(QApplication::translate("MainWindow", "Build &trees", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionBuild_trees->setToolTip(QApplication::translate("MainWindow", "Build all fractal trees", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionBuild_trees->setShortcut(QApplication::translate("MainWindow", "Ctrl+B", 0, QApplication::UnicodeUTF8));
        actionGenerate_time_series->setText(QApplication::translate("MainWindow", "Generate time &series", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionGenerate_time_series->setToolTip(QApplication::translate("MainWindow", "Generate time series with fractal structure", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionGenerate_time_series->setShortcut(QApplication::translate("MainWindow", "Ctrl+G", 0, QApplication::UnicodeUTF8));
        actionSettings->setText(QApplication::translate("MainWindow", "Settings", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSettings->setToolTip(QApplication::translate("MainWindow", "Settings dialog", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionSettings->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+P", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        action_Quit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionZoom_in->setText(QApplication::translate("MainWindow", "Zoom &in", 0, QApplication::UnicodeUTF8));
        actionZoom_in->setShortcut(QApplication::translate("MainWindow", "Ctrl+=", 0, QApplication::UnicodeUTF8));
        actionZoom_out->setText(QApplication::translate("MainWindow", "Zoom &out", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionZoom_out->setToolTip(QApplication::translate("MainWindow", "Zoom out", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionZoom_out->setShortcut(QApplication::translate("MainWindow", "Ctrl+-", 0, QApplication::UnicodeUTF8));
        action_Debug_window->setText(QApplication::translate("MainWindow", "&Debug window", 0, QApplication::UnicodeUTF8));
        action_Fit_time_series->setText(QApplication::translate("MainWindow", "&Fit time series", 0, QApplication::UnicodeUTF8));
        actionCreate_markup_from_roots->setText(QApplication::translate("MainWindow", "Create markup from roots", 0, QApplication::UnicodeUTF8));
        actionShow_Analysys_properties->setText(QApplication::translate("MainWindow", "Analysys properties", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionShow_Analysys_properties->setToolTip(QApplication::translate("MainWindow", "Show/hide analysis properties window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOpen_DTS_from_file->setText(QApplication::translate("MainWindow", "From &file...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionOpen_DTS_from_file->setToolTip(QApplication::translate("MainWindow", "Open dynamic time series from file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOpen_DTS_from_file->setShortcut(QApplication::translate("MainWindow", "Alt+D", 0, QApplication::UnicodeUTF8));
        actionDynamic_Time_Series_Run->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        actionDynamic_Time_Series_Run->setShortcut(QApplication::translate("MainWindow", "Alt+R", 0, QApplication::UnicodeUTF8));
        actionDynamic_Time_Series_Stop->setText(QApplication::translate("MainWindow", "Stop", 0, QApplication::UnicodeUTF8));
        actionDynamic_Time_Series_Stop->setShortcut(QApplication::translate("MainWindow", "Alt+S", 0, QApplication::UnicodeUTF8));
        actionDynamic_Time_Series_Step->setText(QApplication::translate("MainWindow", "Step", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionDynamic_Time_Series_Step->setToolTip(QApplication::translate("MainWindow", "Get next single value of dynamic time series", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionDynamic_Time_Series_Step->setShortcut(QApplication::translate("MainWindow", "Alt+N", 0, QApplication::UnicodeUTF8));
        gbInfo->setTitle(QApplication::translate("MainWindow", "Info", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Parse Trees:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "of", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Time Series size:", 0, QApplication::UnicodeUTF8));
        lbTimeSeriesSize->setText(QApplication::translate("MainWindow", "ts size", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Patterns count:", 0, QApplication::UnicodeUTF8));
        lbPatternsCount->setText(QApplication::translate("MainWindow", "patterns count", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "State:", 0, QApplication::UnicodeUTF8));
        lbState->setText(QApplication::translate("MainWindow", "state", 0, QApplication::UnicodeUTF8));
        bnClearParseTrees->setText(QApplication::translate("MainWindow", "Clear", 0, QApplication::UnicodeUTF8));
        lbTreesCount->setText(QApplication::translate("MainWindow", "trees count", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Memory:", 0, QApplication::UnicodeUTF8));
        lbMemory->setText(QApplication::translate("MainWindow", "memory", 0, QApplication::UnicodeUTF8));
        bnHaltAnalysis->setText(QApplication::translate("MainWindow", "Halt", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
        analysisPropertiesWidget->setWindowTitle(QApplication::translate("MainWindow", "Properties", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Patterns", 0, QApplication::UnicodeUTF8));
        bnEditPatterns->setText(QApplication::translate("MainWindow", "Edit", 0, QApplication::UnicodeUTF8));
        bnClearPatterns->setText(QApplication::translate("MainWindow", "Clear", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Marker", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Parser", 0, QApplication::UnicodeUTF8));
        gbDrawing->setTitle(QApplication::translate("MainWindow", "Drawing", 0, QApplication::UnicodeUTF8));
        cbShowParseTreeNodes->setText(QApplication::translate("MainWindow", "Show parse tree nodes", 0, QApplication::UnicodeUTF8));
        cbShowParseTreeTimeSeries->setText(QApplication::translate("MainWindow", "Show parse tree time series", 0, QApplication::UnicodeUTF8));
        cbShowParseTreeRoots->setText(QApplication::translate("MainWindow", "Show parse tree roots", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menuOpen_dynamic_time_series->setTitle(QApplication::translate("MainWindow", "Open &dynamic time series", 0, QApplication::UnicodeUTF8));
        menu_Analysis->setTitle(QApplication::translate("MainWindow", "&Analysis", 0, QApplication::UnicodeUTF8));
        menu_Tests->setTitle(QApplication::translate("MainWindow", "T&ests", 0, QApplication::UnicodeUTF8));
        menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
        menuFilters->setTitle(QApplication::translate("MainWindow", "Fi&lters", 0, QApplication::UnicodeUTF8));
        menuTime_series->setTitle(QApplication::translate("MainWindow", "&Time series", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
