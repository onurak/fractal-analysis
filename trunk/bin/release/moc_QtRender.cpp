/****************************************************************************
** Meta object code from reading C++ file 'QtRender.h'
**
** Created: Sun 7. Nov 03:48:32 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/fractallib/QtRender.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtRender.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FL__QtScene[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   13,   12,   12, 0x05,
      61,   13,   12,   12, 0x05,
      99,   13,   12,   12, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_FL__QtScene[] = {
    "FL::QtScene\0\0mouseEvent\0"
    "mouseMove(QGraphicsSceneMouseEvent*)\0"
    "mousePress(QGraphicsSceneMouseEvent*)\0"
    "mouseRelease(QGraphicsSceneMouseEvent*)\0"
};

const QMetaObject FL::QtScene::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_FL__QtScene,
      qt_meta_data_FL__QtScene, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FL::QtScene::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FL::QtScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FL::QtScene::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FL__QtScene))
        return static_cast<void*>(const_cast< QtScene*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int FL::QtScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: mouseMove((*reinterpret_cast< QGraphicsSceneMouseEvent*(*)>(_a[1]))); break;
        case 1: mousePress((*reinterpret_cast< QGraphicsSceneMouseEvent*(*)>(_a[1]))); break;
        case 2: mouseRelease((*reinterpret_cast< QGraphicsSceneMouseEvent*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FL::QtScene::mouseMove(QGraphicsSceneMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FL::QtScene::mousePress(QGraphicsSceneMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FL::QtScene::mouseRelease(QGraphicsSceneMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
