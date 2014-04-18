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
#include "ui_mainwindow.h"

#include "streamcontroller.h"
#include "graphics/graphicscontroller.h"
#include "audiocontroller.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    graphics_ = GraphicsController::createInstance(ui->view_, ui->scene_tab_, ui->graphics_tab_);
    graphics_->addScene("Default");

    audio_ = new AudioController;

    stream_ = new StreamController(graphics_, audio_, ui->general_tab_, ui->encoder_tab_);

    connect(ui->show_hide_, SIGNAL(clicked()), this, SLOT(toggleShowHide()));
    connect(ui->start_stop_, SIGNAL(clicked()), this, SLOT(toggleStartStop()));
    connect(&time_updater_, SIGNAL(timeout()), this, SLOT(updateTime()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleShowHide()
{
    if(ui->tab_widget_->isHidden()) {
        ui->tab_widget_->show();
        ui->show_hide_->setText("Hide");
    } else {
        ui->tab_widget_->hide();
        ui->show_hide_->setText("Show");
    }

    QWidget *parent = ui->tab_widget_->parentWidget();
    while(parent) {
        parent->adjustSize();
        parent = parent->parentWidget();
    }
}

void MainWindow::toggleStartStop()
{
    if(stream_->isActive()) { // if in progress
        stream_->stop();
        time_updater_.stop();
        ui->start_stop_->setText("Start");
        ui->elapsed_->setText("00:00:00");
    } else {
        if(stream_->start()) {
            QTimer::singleShot(100, this, SLOT(startUpdater()));
            ui->start_stop_->setText("Stop");
        }
    }
}

void MainWindow::updateTime()
{
    int elapsed = stream_->elapsed() / 1000; // in seconds
    int sec = elapsed % 60;
    elapsed /= 60;
    int min = elapsed % 60;
    elapsed /= 60;
    int hour = elapsed;
    ui->elapsed_->setText(QString("%1:%2:%3")
                          .arg(hour, 2, 10, QLatin1Char('0'))
                          .arg(min , 2, 10, QLatin1Char('0'))
                          .arg(sec , 2, 10, QLatin1Char('0')));
}

void MainWindow::closeEvent(QCloseEvent *)
{
    // The order of deleting item is so important since stream_ accesses graphics_ and audio_.
    delete stream_;
    stream_ = 0;
    delete graphics_;
    graphics_ = 0;
    delete audio_;
    audio_ = 0;
}

void MainWindow::startUpdater()
{
    time_updater_.start(1000);
}
