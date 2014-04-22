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

#include "cameraproperty.h"
#include "ui_propertybase.h"
#include "ui_cameraproperty.h"
#include "../item/camera.h"

#include <QCamera>

CameraProperty::CameraProperty(QWidget *parent) :
    PropertyBase(parent),
    camera_ui(new Ui::CameraProperty),
    item_(0)
{
    QWidget *camera = new QWidget;
    ui->toolBox->addItem(camera, QString("Camera"));
    camera_ui->setupUi(camera);

    connect(camera_ui->c_start_, SIGNAL(clicked()),
            this, SLOT(onStart()));
    connect(camera_ui->c_stop_, SIGNAL(clicked()),
            this, SLOT(onStop()));
}

void CameraProperty::setItem(Camera *item)
{
    reset();
    PropertyBase::setItem(item);
    item_ = item;
    camera_ui->c_device_->clear();
    devices_ = QCamera::availableDevices();
    for(auto d : devices_) {
        camera_ui->c_device_->addItem(d);
    }
}

void CameraProperty::reset()
{
    item_ = 0;
    PropertyBase::reset();
}

void CameraProperty::update()
{
    if(!item_) return;
    lockWith([&]() {
    });

    PropertyBase::update();
}

void CameraProperty::onStart()
{
    if(!item_) return;
    item_->setDevice(devices_.at(camera_ui->c_device_->currentIndex()));
    item_->start();
}

void CameraProperty::onStop()
{
    if(!item_) return;
    item_->stop();
}
