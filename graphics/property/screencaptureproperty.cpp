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

#include "screencaptureproperty.h"
#include "ui_propertybase.h"
#include "ui_screencaptureproperty.h"
#include "../item/screencapture.h"

#include <QCheckBox>
#include <QDebug>

ScreenCaptureProperty::ScreenCaptureProperty(QWidget *parent):
    PropertyBase(parent),
    sc_ui(new Ui::ScreenCaptureProperty),
    item_(0)
{
    QWidget *capture = new QWidget;
    ui->tabWidget->addTab(capture, QString("Screen Capture"));
    sc_ui->setupUi(capture);

    connect(sc_ui->include_cursor_, SIGNAL(stateChanged(int)), this, SLOT(onIncludeCursor()));
    connect(sc_ui->scale_mode_, SIGNAL(currentIndexChanged(int)), this, SLOT(onScaleMode()));
    connect(sc_ui->fill_by_scaling_, SIGNAL(clicked()), this, SLOT(onFillByScaling()));
    connect(sc_ui->fill_by_expanding_, SIGNAL(clicked()), this, SLOT(onFillByExpandingCaptureArea()));
    connect(sc_ui->fit_by_scaling_, SIGNAL(clicked()), this, SLOT(onFitByScaling()));
    connect(sc_ui->full_screen_, SIGNAL(clicked()), this, SLOT(onFullScreen()));
    connect(sc_ui->select_captured_area_, SIGNAL(clicked()), this, SLOT(onSelectCapturedArea()));
    connect(sc_ui->x_, SIGNAL(valueChanged(int)), this, SLOT(onCX()));
    connect(sc_ui->y_, SIGNAL(valueChanged(int)), this, SLOT(onCY()));
    connect(sc_ui->width_, SIGNAL(valueChanged(int)), this, SLOT(onCWidth()));
    connect(sc_ui->height_, SIGNAL(valueChanged(int)), this, SLOT(onCHeight()));
}

void ScreenCaptureProperty::setItem(ScreenCapture *item)
{
    PropertyBase::setItem(item);
    item_ = item;
}

void ScreenCaptureProperty::reset()
{
    item_ = 0;
    PropertyBase::reset();
}

void ScreenCaptureProperty::update()
{
    if(!item_) return;
    lockWith([this]() {
        sc_ui->include_cursor_->setChecked(item_->doesIncludeCursor());
        sc_ui->scale_mode_->setCurrentIndex(item_->scaleMode());

        QRect rect = item_->capturedArea();
        sc_ui->x_->setValue(rect.x());
        sc_ui->y_->setValue(rect.y());
        sc_ui->width_->setValue(rect.width());
        sc_ui->height_->setValue(rect.height());
    });

    PropertyBase::update();
}

void ScreenCaptureProperty::onIncludeCursor()
{
    if(!item_) return;
    lockWith([this]() {
        item_->setIncludeCursor(sc_ui->include_cursor_->isChecked());
    });
}

void ScreenCaptureProperty::onScaleMode()
{
    if(!item_) return;
    lockWith([this]() {
        item_->setScaleMode((Qt::TransformationMode)sc_ui->scale_mode_->currentIndex());
    });
}

void ScreenCaptureProperty::onFillByScaling()
{
    if(!item_) return;
    lockWith([this]() {
        item_->fillByScaling();
    });
    update();
}

void ScreenCaptureProperty::onFillByExpandingCaptureArea()
{
    if(!item_) return;
    lockWith([this]() {
        item_->fillByExpandingCaptureArea();
    });
    update();
}

void ScreenCaptureProperty::onFitByScaling()
{
    if(!item_) return;
    lockWith([this]() {
        item_->fitByScaling();
    });
    update();
}

void ScreenCaptureProperty::onFullScreen()
{
    if(!item_) return;
    lockWith([this]() {
        item_->fullScreen();
    });
    update();
}

void ScreenCaptureProperty::onSelectCapturedArea()
{
    if(!item_) return;
    item_->selectArea();
}

void ScreenCaptureProperty::onCX()
{
    if(!item_) return;
    lockWith([this]() {
        QRect rect = item_->capturedArea();
        rect.moveLeft(sc_ui->x_->value());
        item_->setCapturedArea(rect);
    });
}

void ScreenCaptureProperty::onCY()
{
    if(!item_) return;
    lockWith([this]() {
        QRect rect = item_->capturedArea();
        rect.moveTop(sc_ui->y_->value());
        item_->setCapturedArea(rect);
    });
}

void ScreenCaptureProperty::onCWidth()
{
    if(!item_) return;
    lockWith([this]() {
        QRect rect = item_->capturedArea();
        rect.setWidth(sc_ui->width_->value());
        item_->setCapturedArea(rect);
    });
    update();
}

void ScreenCaptureProperty::onCHeight()
{
    if(!item_) return;
    lockWith([this]() {
        QRect rect = item_->capturedArea();
        rect.setHeight(sc_ui->height_->value());
        item_->setCapturedArea(rect);
    });
    update();
}
