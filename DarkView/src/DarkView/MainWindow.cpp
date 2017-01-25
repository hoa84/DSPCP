/*
**  Data Scalable Approach for Parallel Coordinates
**  Copyright (C) 2016 - Hoa Nguyen, Paul Rosen
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

#include <DarkView/MainWindow.h>

#include <QMenuBar>
#include <QFileDialog>
#include <QSplitter>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

#include <iostream>
#include <sstream>


MainWindow::MainWindow(QWidget *parent) : QT::QExtendedMainWindow(parent), indir_datafile(&datafile)
{
    // method to use here 1 - PCP, 2 - Kmean, 3 - SCP
    meth = 0;
    // data source: 1 - physic, 2 - car;
    dts = 1;

    // Set window title
    setWindowTitle(tr("DarkView: Parameter Space Visualization Tool"));

    // Setup File Menu
    file_menu = menuBar()->addMenu("&File");
    {
        file_menu->addAction( open = new QAction("&Open", this ) );
        file_menu->addAction( copy = new QAction("&Copy Setting", this ) );
        file_menu->addSeparator();
        recent_menu = addRecentMenu( file_menu );
        file_menu->addSeparator();
        file_menu->addAction( exit = new QAction("E&xit", this ) );

        exit->setShortcut(tr("CTRL+X"));

        connect( open, SIGNAL(triggered()), this, SLOT(openFile()) );
        connect( copy, SIGNAL(triggered()), this, SLOT(copySettings()) );
        connect( exit, SIGNAL(triggered()), qApp, SLOT(quit())     );
    }


    // Set up method
    vis_meth = menuBar()->addMenu("&VisMethod");
    {
        vis_meth->addAction(m1 = new QAction("&Trend Parallel Coordinates", this));
        vis_meth->addAction(m2 = new QAction("&Kmean", this));
        vis_meth->addAction(m3 = new QAction("&Scatter Plots", this));

        connect(m1, SIGNAL(triggered()), this, SLOT(met1()));
        connect(m2, SIGNAL(triggered()), this, SLOT(met2()));
        connect(m3, SIGNAL(triggered()), this, SLOT(met3()));

    }

    help_menu = menuBar()->addMenu("&Help");
    {
        // Setup Exit Menu
        help_menu->addAction( help_item  = new QAction("&Help", this ) );
        help_menu->addSeparator();
        help_menu->addAction( about_item = new QAction("&About", this ) );

        help_item->setShortcut(tr("F1"));

        connect( help_item,  SIGNAL(triggered()), this, SLOT(help()) );
        connect( about_item, SIGNAL(triggered()), this, SLOT(about()) );
    }

    // Hoa - TMP
    /*
    meth = 1;
    QString dir;
    // PHYSICS DATA
    //for windows
    #ifdef WIN32
        dir = "C:/OCPCP/Data/data.dat";
    #elif defined(__APPLE__)
    //for Mac
        dir = "/Users/hoa/Documents/DVproject/TPCP_Versions/TPCP_Mix/Data/perfectNeg.dat";
    #endif

    loadFile(dir);
    */
    // end Hoa -TMP
}

MainWindow::~MainWindow(){ }

void MainWindow::met1()
{
    if(meth == 1)
    {
        mw->setParent(NULL);
        dw->setParent(NULL);
        pc->setParent(NULL);
    }
    if(meth == 2)
    {
        mw->setParent(NULL);
        dw->setParent(NULL);
        km->setParent(NULL);
    }
    if(meth == 3)
    {
        mw->setParent(NULL);
        dw->setParent(NULL);
        scap->setParent(NULL);
    }

    meth = 1;
    openFile();
}

void MainWindow::met2()
{
    if(meth == 1)
    {
        mw->setParent(NULL);
        dw->setParent(NULL);
        pc->setParent(NULL);
    }
    if(meth == 2)
    {
        mw->setParent(NULL);
        dw->setParent(NULL);
        km->setParent(NULL);
    }
    if(meth == 3)
    {
        mw->setParent(NULL);
        dw->setParent(NULL);
        scap->setParent(NULL);
    }

    meth = 2;

    openFile();
}

void MainWindow::met3()
{
    if(meth == 1)
    {
        //mw->setParent(NULL);
        //dw->setParent(NULL);
        pc->setParent(NULL);
    }
    if(meth == 2)
    {
        //mw->setParent(NULL);
        //dw->setParent(NULL);
        km->setParent(NULL);
    }
    if(meth == 3)
    {
        //mw->setParent(NULL);
        //dw->setParent(NULL);
        scap->setParent(NULL);
    }

    meth = 3;

    openFile();
}

void MainWindow::help()
{
    QDesktopServices::openUrl( QUrl(tr("http://www.cspaul.com/wiki/doku.php?id=software:darkview")) );
}

void MainWindow::about(){

    QString title = tr("DarkView: Parameter Space Visualization Tool");

    QString desc;
    desc += tr("<b><big>DarkView: Parameter Space Visualization Tool</big></b>");
    desc += tr("<br>");
    desc += tr("<br>");
    desc += tr("<a href=\"http://www.cspaul.com/wiki/doku.php?id=software:darkview\">DarkView</a> is a parameter space visualization tool under <br>");
    desc += tr("development in collaboration with <a href=\"http://www.physics.utah.edu/~sandick/\">Pearl Sandick</a> for the <br>");
    desc += tr("purpose of parameter space exploration in particle <br>");
    desc += tr("physics.");
    desc += tr("<br>");
    desc += tr("<br>");
    desc += tr("Copyright (C) 2013  <a href=\"http://www.cspaul.com\">Paul Rosen</a>");
    desc += tr("<br>");
    desc += tr("<br>");
    desc += tr("The program is provided AS IS with NO WARRANTY OF <br>");
    desc += tr("ANY KIND, INCLUDING THE WARRANTY OF DESIGN, <br>");
    desc += tr("MERCHANTABILITY AND FITNESS FOR A PARTICULAR <br>");
    desc += tr("PURPOSE.");

    QMessageBox::about( this, title, desc );

}

void MainWindow::copySettings( ){
    // hoa tam
    /*
    cursorOverride(Qt::WaitCursor);
    QString fname = openDialog( tr("Open a data file"), tr("All Files (*.txt *.dat);;Text File (*.txt);;Generic Dat File (*.dat)") );
    if( fname.size() > 0 ){
        Data::PhysicsData tmpdata;
        if( tmpdata.Load( fname.toLocal8Bit().data()) ){
            datafile.CopyMeta( tmpdata );
            datafile.SaveMeta( );
        }
    }
    cursorRestore();
    */
}

void MainWindow::openFile( )
{
    cursorOverride(Qt::WaitCursor);
    QString fname = openDialog( tr("Open a data file"), tr("All Files (*.txt *.dat);;Text File (*.txt);;Generic Dat File (*.dat)") );
    if( fname.size() > 0 )
    {
        loadFile( fname );
        updateRecentMenu( recent_menu, fname );
    }
    cursorRestore();
}

void MainWindow::open_recent( QString fname )
{
    if( fname.size() > 0 )
    {
        loadFile( fname );
        updateRecentMenu( recent_menu, fname );
    }
}

void MainWindow::loadFile( QString fname ){

    if( centralWidget() != hsplit )
    {
        vsplit = new QSplitter( Qt::Vertical, 0 );
        hsplit = new QSplitter( Qt::Horizontal, 0 );

        mw = new MainWidget( 0 );
        //vsplit->addWidget(mw);

        dw = new QDimensionWidget( 0 );
        connect( dw, SIGNAL(ItemUpdated(int,QString,bool)), this, SLOT(UpdateItem(int,QString,bool)) );

        //pc = new ParallelCoordinates(mw, mw->font,0);
        //km = new Kmean(mw, mw->font,0);
        if(meth == 1)
        {
            pc = new ParallelCoordinates(mw, mw->font,0);
            connect( pc, SIGNAL(UpdatedSelection(std::pair<int,int>)), mw, SLOT(UpdateSelection(std::pair<int,int>)) );
            vsplit->addWidget(pc);
        }
        if(meth == 2)
        {
            km = new Kmean(mw, mw->font,0);
            connect( km, SIGNAL(UpdatedSelection(std::pair<int,int>)), mw, SLOT(UpdateSelection(std::pair<int,int>)) );
            vsplit->addWidget(km);
        }
        if (meth == 3)
        {
            scap = new Scatter(mw, mw->font,0);
            connect( scap, SIGNAL(UpdatedSelection(std::pair<int,int>)), mw, SLOT(UpdateSelection(std::pair<int,int>)) );
            vsplit->addWidget(scap);
        }

        hsplit->addWidget(vsplit);
        hsplit->addWidget(dw);
        setCentralWidget( hsplit );
    }


    datafile.Load( fname.toLocal8Bit().data(), 0);
    datafile.SaveMeta();

    indir_datafile.Recompute();
    //indir_datafile.SortData();

    mw->SetData( &indir_datafile );
    QTimer::singleShot( 100, mw, SLOT(Start()) );

    if(meth == 1)
    {
        pc->SetData( &indir_datafile );
        QTimer::singleShot( 100, pc, SLOT(Start()) );
    }
    if(meth == 2)
    {
        km->SetData( &indir_datafile );
        QTimer::singleShot( 100, km, SLOT(Start()) );
    }
    if (meth == 3)
    {
        scap->SetData( &indir_datafile );
        QTimer::singleShot( 100, scap, SLOT(Start()) );
    }

    dw->SetTitle( tr(datafile.GetFilename().c_str()) );
    dw->ClearItems( );
    for(int i = 0; i < datafile.GetDim(); i++)
    {
        dw->AddItem( tr(datafile.GetLabel(i).c_str()), datafile.isEnabled(i) );
    }

    indir_datafile.Recompute();
    //indir_datafile.SortData();

    // hoa sua
    // update data in Parallel Coordinates and reset PC interface
    // fix error of interactions between Dimension editor and Parallel Coordinates Interface.
    /*
    for (int i = 0; i < 5; i++)
    {
        sleep(1);

        if (i > 1)
        {
            datafile.Load( fname.toLocal8Bit().data(), i);
            datafile.SaveMeta();

            indir_datafile.Recompute();
            pc->SetData( &indir_datafile );
            pc->Reset();
        }
    }
    */
}

void MainWindow::UpdateItem( int idx, QString str, bool checked )
{
    if( checked )
    {
        datafile.Enable( idx );
    }
    else
    {
        datafile.Disable( idx );
    }
    datafile.SetLabel( idx, std::string( str.toLocal8Bit().data() ) );
    datafile.SaveMeta();
    indir_datafile.Recompute();
    //indir_datafile.SortData();

    // hoa sua    
    // update data in Parallel Coordinates and reset PC interface
    // fix error of interactions between Dimension editor and Parallel Coordinates Interface.

    if(meth == 1)
    {
        pc->SetData( &indir_datafile );
        pc->Reset();
    }
    if(meth == 2)
    {
        km->SetData( &indir_datafile );
        km->Reset();
    }
    if (meth == 3)
    {
        scap->SetData( &indir_datafile );
        scap->Reset();
    }

    //SPLOM
    mw->ProgressiveReset( );
}



