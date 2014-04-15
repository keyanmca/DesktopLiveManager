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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>

class StreamController;
class GraphicsController;
class AudioController;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void toggleShowHide();
    void toggleStartStop();
    void updateTime();

protected:
    void closeEvent(QCloseEvent *) override;

private slots:
    void startUpdater();

private:
    Ui::MainWindow *ui;

    StreamController *stream_;
    GraphicsController *graphics_;
    AudioController *audio_;

    QTime time_;
    QTimer time_updater_;
    QTimer graphics_updater_;
};

#endif // MAINWINDOW_H
