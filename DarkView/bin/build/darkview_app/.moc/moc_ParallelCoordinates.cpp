/****************************************************************************
** Meta object code from reading C++ file 'ParallelCoordinates.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/DarkView/ParallelCoordinates.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ParallelCoordinates.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ParallelCoordinates[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   57,   57,   57, 0x05,

 // slots: signature, parameters, type, tag, flags
      58,   57,   57,   57, 0x0a,
      66,   57,   57,   57, 0x0a,
      74,   57,   57,   57, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ParallelCoordinates[] = {
    "ParallelCoordinates\0"
    "UpdatedSelection(std::pair<int,int>)\0"
    "\0Reset()\0Start()\0saveScreenshot()\0"
};

void ParallelCoordinates::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ParallelCoordinates *_t = static_cast<ParallelCoordinates *>(_o);
        switch (_id) {
        case 0: _t->UpdatedSelection((*reinterpret_cast< std::pair<int,int>(*)>(_a[1]))); break;
        case 1: _t->Reset(); break;
        case 2: _t->Start(); break;
        case 3: _t->saveScreenshot(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ParallelCoordinates::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ParallelCoordinates::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_ParallelCoordinates,
      qt_meta_data_ParallelCoordinates, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ParallelCoordinates::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ParallelCoordinates::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ParallelCoordinates::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ParallelCoordinates))
        return static_cast<void*>(const_cast< ParallelCoordinates*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int ParallelCoordinates::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ParallelCoordinates::UpdatedSelection(std::pair<int,int> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
