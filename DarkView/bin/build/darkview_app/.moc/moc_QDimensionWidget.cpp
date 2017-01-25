/****************************************************************************
** Meta object code from reading C++ file 'QDimensionWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/DarkView/QDimensionWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QDimensionWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDimensionWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   47,   63,   63, 0x05,

 // slots: signature, parameters, type, tag, flags
      64,   91,   63,   63, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QDimensionWidget[] = {
    "QDimensionWidget\0ItemUpdated(int,QString,bool)\0"
    "idx,str,checked\0\0updateItem(QStandardItem*)\0"
    "item\0"
};

void QDimensionWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QDimensionWidget *_t = static_cast<QDimensionWidget *>(_o);
        switch (_id) {
        case 0: _t->ItemUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->updateItem((*reinterpret_cast< QStandardItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QDimensionWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QDimensionWidget::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_QDimensionWidget,
      qt_meta_data_QDimensionWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QDimensionWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QDimensionWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QDimensionWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDimensionWidget))
        return static_cast<void*>(const_cast< QDimensionWidget*>(this));
    return QTreeView::qt_metacast(_clname);
}

int QDimensionWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QDimensionWidget::ItemUpdated(int _t1, QString _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
