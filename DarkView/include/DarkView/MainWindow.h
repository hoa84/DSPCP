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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QSplitter>

#include <QT/QExtendedMainWindow.h>

#include <DarkView/MainWidget.h>
#include <DarkView/ParallelCoordinates.h>
#include <DarkView/QDimensionWidget.h>
#include <DarkView/Kmean.h>
#include <DarkView/Scatter.h>

class MainWindow : public QT::QExtendedMainWindow {

    Q_OBJECT
    
public:

    MainWindow( QWidget * parent = 0);
    ~MainWindow();

public slots:
    void openFile( );
    void UpdateItem( int idx, QString str, bool checked );
    void copySettings( );
    void help();
    void about();
    void met1();
    void met2();
    void met3();

protected:
    virtual void open_recent( QString fname );


public:
    void loadFile( QString fname );

protected:

    Data::PhysicsData datafile;

    DataIndirector indir_datafile;

    MainWidget          * mw;    
    QDimensionWidget    * dw;
    ParallelCoordinates * pc;
    Kmean * km;
    Scatter * scap;

    QSplitter * vsplit;
    QSplitter * hsplit;

    QMenu      * file_menu;
    QAction    * open;
    QAction    * copy;
    QMenu      * recent_menu;
    QAction    * exit;
    QMenu      * help_menu;
    QAction    * help_item;
    QAction    * about_item;

    QMenu      * vis_meth;
    QAction    * m1;
    QAction    * m2;
    QAction    * m3;


    int meth;
    int dts;
};

#endif // MAINWINDOW_H
