/*
**  DarkView: Parameter Space Visualization Tool
**  Copyright (C) 2013  Paul Rosen
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QDIMENSIONWIDGET_H
#define QDIMENSIONWIDGET_H

#include <QTreeView>
#include <QStandardItemModel>

class QDimensionWidget : public QTreeView {

    Q_OBJECT

public:

    QDimensionWidget( QWidget * parent = 0);
    ~QDimensionWidget();

    void SetTitle( QString title );
    void ClearItems( );
    void AddItem( QString name, bool checked );

signals:
    void ItemUpdated( int idx, QString str, bool checked );

private slots:
    void updateItem( QStandardItem* item );

protected:

    QStandardItemModel * model;
    QTreeView          * view;
    QStandardItem      * item;

    std::vector<QStandardItem*> subItems;

    QFont font;


};


#endif
