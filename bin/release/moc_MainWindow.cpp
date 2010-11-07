/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created: Sun 7. Nov 03:48:13 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/gui/MainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      47,   34,   11,   11, 0x0a,
      88,   81,   11,   11, 0x0a,
     130,  124,   11,   11, 0x0a,
     180,   11,   11,   11, 0x08,
     215,   11,   11,   11, 0x08,
     261,   11,   11,   11, 0x08,
     307,   11,   11,   11, 0x08,
     345,   11,   11,   11, 0x08,
     378,   11,   11,   11, 0x08,
     417,   11,   11,   11, 0x08,
     455,   11,   11,   11, 0x08,
     485,   11,   11,   11, 0x08,
     523,  514,   11,   11, 0x08,
     572,   11,   11,   11, 0x08,
     600,   11,   11,   11, 0x08,
     638,  631,   11,   11, 0x08,
     668,   11,   11,   11, 0x08,
     698,   11,   11,   11, 0x08,
     725,   11,   11,   11, 0x08,
     767,   11,   11,   11, 0x08,
     802,   11,   11,   11, 0x08,
     840,   11,   11,   11, 0x08,
     872,   11,   11,   11, 0x08,
     907,   11,   11,   11, 0x08,
     948,  941,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0workerThreadStarted()\0"
    "progress,msg\0workerThreadProgress(int,QString)\0"
    "thread\0workerThreadFinished(WorkerThread*)\0"
    "event\0workerThreadDebugParser(FL::TSParser::DebugEvent)\0"
    "on_action_Debug_window_triggered()\0"
    "on_actionShow_Analysys_properties_triggered()\0"
    "on_actionCreate_markup_from_roots_triggered()\0"
    "on_action_Fit_time_series_triggered()\0"
    "on_actionBuild_trees_triggered()\0"
    "on_action_Run_step_by_step_triggered()\0"
    "on_actionOpen_Environment_triggered()\0"
    "on_actionZoom_out_triggered()\0"
    "on_actionZoom_in_triggered()\0position\0"
    "on_timeSeriesHorizontalScroller_sliderMoved(int)\0"
    "on_bnHaltAnalysis_clicked()\0"
    "on_bnClearParseTrees_clicked()\0treeNo\0"
    "on_sbTreeNo_valueChanged(int)\0"
    "on_actionSettings_triggered()\0"
    "on_action_Quit_triggered()\0"
    "on_actionGenerate_time_series_triggered()\0"
    "on_actionOpen_Patterns_triggered()\0"
    "on_actionOpen_Time_Series_triggered()\0"
    "on_actionNext_level_triggered()\0"
    "spc_on_analysisPropertiesChanged()\0"
    "spc_on_drawingWindowSizeChanged()\0"
    "action\0spc_action_ExecuteFilter(QAction*)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: workerThreadStarted(); break;
        case 1: workerThreadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: workerThreadFinished((*reinterpret_cast< WorkerThread*(*)>(_a[1]))); break;
        case 3: workerThreadDebugParser((*reinterpret_cast< FL::TSParser::DebugEvent(*)>(_a[1]))); break;
        case 4: on_action_Debug_window_triggered(); break;
        case 5: on_actionShow_Analysys_properties_triggered(); break;
        case 6: on_actionCreate_markup_from_roots_triggered(); break;
        case 7: on_action_Fit_time_series_triggered(); break;
        case 8: on_actionBuild_trees_triggered(); break;
        case 9: on_action_Run_step_by_step_triggered(); break;
        case 10: on_actionOpen_Environment_triggered(); break;
        case 11: on_actionZoom_out_triggered(); break;
        case 12: on_actionZoom_in_triggered(); break;
        case 13: on_timeSeriesHorizontalScroller_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: on_bnHaltAnalysis_clicked(); break;
        case 15: on_bnClearParseTrees_clicked(); break;
        case 16: on_sbTreeNo_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: on_actionSettings_triggered(); break;
        case 18: on_action_Quit_triggered(); break;
        case 19: on_actionGenerate_time_series_triggered(); break;
        case 20: on_actionOpen_Patterns_triggered(); break;
        case 21: on_actionOpen_Time_Series_triggered(); break;
        case 22: on_actionNext_level_triggered(); break;
        case 23: spc_on_analysisPropertiesChanged(); break;
        case 24: spc_on_drawingWindowSizeChanged(); break;
        case 25: spc_action_ExecuteFilter((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 26;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
