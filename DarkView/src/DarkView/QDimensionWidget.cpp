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

#include <DarkView/QDimensionWidget.h>

QDimensionWidget::QDimensionWidget( QWidget * parent ) : QTreeView( parent ) {

    font = QFont( "Arial" );

    item = new QStandardItem(  );
    item->setEditable(false);
    item->setFont( font );

    model = new QStandardItemModel();
    model->setItem(0, item);
    model->setHeaderData( 0, Qt::Horizontal, "Dimension Editor" );

    setModel(model);

    connect( model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateItem(QStandardItem*)) );

}

QDimensionWidget::~QDimensionWidget(){ }

void QDimensionWidget::updateItem( QStandardItem* item ){
    for(int i = 0; i < (int)subItems.size(); i++ ){
        if( item == subItems[i] ){
            emit ItemUpdated( i, item->text(), item->checkState() == Qt::Checked );
        }
    }
}

void QDimensionWidget::SetTitle( QString title ){
    item->setText( title );
}

void QDimensionWidget::ClearItems( ){
    if(subItems.size()>0){
        item->removeRows(0,item->rowCount());
        for(int i = 0; i < (int)subItems.size(); i++){
            delete subItems[i];
        }
        subItems.clear();
    }
}

void QDimensionWidget::AddItem( QString name, bool checked ){
    subItems.push_back( new QStandardItem( name ) );
    subItems.back()->setFont( font );
    subItems.back()->setEditable(true);
    subItems.back()->setCheckable(true);
    subItems.back()->setCheckState( checked ? Qt::Checked : Qt::Unchecked );
    item->appendRow(subItems.back());
}

