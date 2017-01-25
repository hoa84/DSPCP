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

#include <QApplication>
#include <DarkView/MainWindow.h>
#include <time.h>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("SCI Institute");
    QCoreApplication::setOrganizationDomain("Visualization");
    QCoreApplication::setApplicationName("DarkView");

    SCI::PrintLicense( "DarkView: Parameter Space Visualization Tool", "Paul Rosen - Hoa Nguyen", "2013" );

    MainWindow w;
    w.show();

    return app.exec();
}
