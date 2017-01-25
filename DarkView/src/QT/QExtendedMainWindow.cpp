/*
**  Common QT Extensions Library
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

#include <QT/QExtendedMainWindow.h>

#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenu>

#include <iostream>

using namespace QT;

QExtendedMainWindow::QExtendedMainWindow(QWidget *parent) : QMainWindow(parent){

    autosave_id = 0;

    // load window geometry
    restoreGeometry(settings.value("geometry").toByteArray());

}

QExtendedMainWindow::~QExtendedMainWindow(){ }

void QExtendedMainWindow::closeEvent   ( QCloseEvent *    ){
    // save window geometry
    settings.setValue("geometry", saveGeometry());
}

void QExtendedMainWindow::keyPressEvent( QKeyEvent * evnt ){

    if( ( evnt->key() == 's' || evnt->key() == 'S' ) && !evnt->isAutoRepeat()){
        ScreenShot();
        return;
    }

}


QMenu * QExtendedMainWindow::addRecentMenu(QMenu * parent_menu){

    // List of recently accessed files
    QMenu * recent_menu = parent_menu->addMenu("Recent Files");
    {
        for(int i = 0; i < 10; i++){
            QAction * tmp = new QAction( this );
            tmp->setVisible(false);
            recent_menu->addAction( tmp );
            connect( tmp, SIGNAL(triggered()), this, SLOT(open_recent()) );
        }
        QList<QAction*> alist = recent_menu->actions();
        //QSettings settings;
        QStringList files = settings.value("RecentFileList").toStringList();
        for(int i = 0; i < alist.size() && i < files.size(); i++){
            alist[i]->setText( files[i] );
            alist[i]->setData( files[i] );
            alist[i]->setVisible( true );
        }
    }
    return recent_menu;
}

void QExtendedMainWindow::open_recent( ){
    QAction *action = qobject_cast<QAction *>(sender());
    open_recent( action->data().toString() );
}

void QExtendedMainWindow::open_recent( QString ){ }


void QExtendedMainWindow::updateRecentMenu( QMenu * recent_menu, QString fname ){

    QStringList files = settings.value("RecentFileList").toStringList();
    files.removeAll( fname );
    files.prepend( fname );

    while (files.size() > 10){
        files.removeLast();
    }

    settings.setValue("RecentFileList", files);
    QList<QAction*> alist = recent_menu->actions();
    for(int i = 0; i < alist.size(); i++){
        if( i < files.size() ){
            alist[i]->setText( files[i] );
            alist[i]->setData( files[i] );
            alist[i]->setVisible( true );
        }
        else{
            alist[i]->setVisible( false );
        }
    }
}


void QExtendedMainWindow::ScreenShot( ){
    char buf[1024];
    sprintf(buf,"autosave%1.3i.png",autosave_id++);
    ScreenShot( QString(buf) );
}

void QExtendedMainWindow::ScreenShot( std::string fname ){
    ScreenShot( tr(fname.c_str()) );
}

void QExtendedMainWindow::ScreenShot( QString fname ){
    std::cout << "Grabbing desktop" << std::endl; fflush(stdout);
    QImage capt = QPixmap::grabWindow(QApplication::desktop()->winId()).toImage();

    std::cout << "Cropping window" << std::endl; fflush(stdout);
    QImage save = capt.copy( QRect(QWidget::mapToGlobal(contentsRect().topLeft()), QWidget::mapToGlobal(contentsRect().bottomRight())) );

    if( save.save(fname) ){
        std::cout << "Save OK" << std::endl; fflush(stdout);
    }
    else{
        std::cout << "Save FAILED" << std::endl; fflush(stdout);
    }
}


#if WIN32
    void QExtendedMainWindow::cursorOverride( Qt::CursorShape shape ){
        QApplication::setOverrideCursor(shape);
    }
#else
    void QExtendedMainWindow::cursorOverride( Qt::CursorShape ){ }
#endif

void QExtendedMainWindow::cursorRestore( ){
    QApplication::restoreOverrideCursor();
}

QString QExtendedMainWindow::openDialog( QString title, QString ftypes ){

    QFileDialog::Options options = QFileDialog::DontResolveSymlinks;
    #if defined(__APPLE__) || defined(linux)
        options |= QFileDialog::DontUseNativeDialog;
    #endif

    QString dir   = QSettings().value("workingDirectory").toString();
    QString fname = QFileDialog::getOpenFileName( 0, title, dir, ftypes, 0, options );

    if( fname.size() > 0 ){
        QSettings().setValue("workingDirectory", QFileInfo( fname ).canonicalPath() );
    }

    return fname;
}

QStringList QExtendedMainWindow::openListDialog( QString title, QString ftypes ){
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks;
    #if defined(__APPLE__) || defined(linux)
        options |= QFileDialog::DontUseNativeDialog;
    #endif

    QString     dir        = QSettings().value("workingDirectory").toString();
    QStringList fname_list = QFileDialog::getOpenFileNames( 0, title, dir, ftypes, 0, options );

    if(fname_list.size() > 0 ){
        QSettings().setValue("workingDirectory", QFileInfo( fname_list.at(0) ).canonicalPath() );
    }

    return fname_list;
}

QString QExtendedMainWindow::directoryDialog( QString title ){
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks;
    #if defined(__APPLE__) || defined(linux)
        options |= QFileDialog::DontUseNativeDialog;
    #endif

    QString dir   = QSettings().value("workingDirectory").toString();
    QString fname = QFileDialog::getExistingDirectory( 0, title, dir, options );

    if( fname.size() > 0 ){
        QSettings().setValue("workingDirectory", QFileInfo( fname ).canonicalPath() );
    }

    return fname;
}


QString QExtendedMainWindow::saveDialog( QString title, QString ftypes ){

    QFileDialog::Options options = QFileDialog::DontResolveSymlinks;
    #if defined(__APPLE__) || defined(linux)
        options |= QFileDialog::DontUseNativeDialog;
    #endif

    QString dir   = QSettings().value("workingDirectory").toString();
    QString fname = QFileDialog::getSaveFileName( 0, title, dir, ftypes, 0, options );

    if( fname.size() > 0 ){
        QSettings().setValue("workingDirectory", QFileInfo( fname ).canonicalPath() );
    }

    return fname;
}

