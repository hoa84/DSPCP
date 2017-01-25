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

#ifndef QT_QEXTENDEDMAINWINDOW_H
#define QT_QEXTENDEDMAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include <string>

namespace QT {

    class QExtendedMainWindow : public QMainWindow {

        Q_OBJECT

    public:

        QExtendedMainWindow( QWidget * parent = 0 );
        ~QExtendedMainWindow();

        virtual QSize minimumSizeHint() const { return QSize(   50,   50 ); }
        virtual QSize sizeHint()        const { return QSize( 1600, 1200 ); }

        virtual void ScreenShot( );
        virtual void ScreenShot( QString fname );
        virtual void ScreenShot( std::string fname );

        static QString     openDialog(      QString title, QString ftypes );
        static QString     directoryDialog( QString title );
        static QStringList openListDialog(  QString title, QString ftypes );
        static QString     saveDialog(      QString title, QString ftypes );

        static void        cursorOverride( Qt::CursorShape shape );
        static void        cursorRestore( );

    private slots:
        void open_recent( );

    protected:

        QMenu *      addRecentMenu( QMenu * parent_menu );
        void         updateRecentMenu( QMenu * recent_menu, QString fname );
        virtual void open_recent( QString fname );

        virtual void keyPressEvent( QKeyEvent   * event );
        virtual void closeEvent   ( QCloseEvent * event );

    protected:

        QSettings settings;

        int autosave_id;

    };
}


#endif // QT_QEXTENDEDMAINWINDOW_H
