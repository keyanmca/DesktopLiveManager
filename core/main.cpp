/*
 * This file is part of DLM.
 *
 * DLM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DLM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DLM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

#ifdef Q_OS_LINUX
    #include <X11/Xlib.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    XInitThreads();
#endif

    QApplication app(argc, argv);

    //QFont font = QApplication::font();
    //font.setPointSize(10);
    //QApplication::setFont(font);

    MainWindow win;
    win.show();

    return app.exec();
}
