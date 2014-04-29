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
#include "../item/screencapture.h"

#include <QCheckBox>
#include <QDebug>

ScreenCaptureProperty::ScreenCaptureProperty(QWidget *parent):
    PropertyBase(parent),
    item_(0),
    c_scalemode_(0),
    c_x_(0),
    c_y_(0),
    c_width_(0),
    c_height_(0)
{
    QWidget *capture = new QWidget;
    QGridLayout *grid = new QGridLayout(capture);

    int row = 0;

    c_include_cursor_ = new QCheckBox("Include Cursor", capture);
    grid->addWidget(c_include_cursor_, row++, 0, 1, 2);
    connect(c_include_cursor_, SIGNAL(stateChanged(int)), this, SLOT(onIncludeCursor()));

    grid->addWidget(new QLabel("Scale Mode", capture), row, 0);
    c_scalemode_  = new QComboBox(capture);
    c_scalemode_->addItems({ "Fast", "Smooth" });
    grid->addWidget(c_scalemode_, row++, 1);
    connect(c_scalemode_, SIGNAL(currentIndexChanged(int)), this, SLOT(onScaleMode()));

    QPushButton *fill_by_scaling = new QPushButton("Fill By Scaling", capture);
    connect(fill_by_scaling, SIGNAL(clicked()), this, SLOT(onFillByScaling()));
    grid->addWidget(fill_by_scaling, row, 0);
    QPushButton *fill_by_expanding_capture_area = new QPushButton("Fill By Expanding", capture);
    connect(fill_by_expanding_capture_area, SIGNAL(clicked()), this, SLOT(onFillByExpandingCaptureArea()));
    grid->addWidget(fill_by_expanding_capture_area, row++, 1);

    QPushButton *fit_by_scaling = new QPushButton("Fit By Scaling", capture);
    connect(fit_by_scaling, SIGNAL(clicked()), this, SLOT(onFitByScaling()));
    grid->addWidget(fit_by_scaling, row, 0);
    QPushButton *full_screen = new QPushButton("Full Screen", capture);
    connect(full_screen, SIGNAL(clicked()), this, SLOT(onFullScreen()));
    grid->addWidget(full_screen, row++, 1);

    QPushButton *show_capture_area = new QPushButton("Select Captured Area", capture);
    connect(show_capture_area, SIGNAL(clicked()), this, SLOT(showCaptureArea()));
    grid->addWidget(show_capture_area, row++, 0, 1, 2);

    grid->addWidget(new QLabel("X", capture), row, 0);
    c_x_  = new QSpinBox(capture);
    c_x_->setKeyboardTracking(false);
    c_x_->setMinimum(-2048);
    c_x_->setMaximum(2048);
    grid->addWidget(c_x_, row++, 1);
    connect(c_x_, SIGNAL(valueChanged(int)), this, SLOT(onCX()));

    grid->addWidget(new QLabel("Y", capture), row, 0);
    c_y_  = new QSpinBox(capture);
    c_y_->setKeyboardTracking(false);
    c_y_->setMinimum(-2048);
    c_y_->setMaximum(2048);
    grid->addWidget(c_y_, row++, 1);
    connect(c_y_, SIGNAL(valueChanged(int)), this, SLOT(onCY()));

    grid->addWidget(new QLabel("Width", capture), row, 0);
    c_width_  = new QSpinBox(capture);
    c_width_->setKeyboardTracking(false);
    c_width_->setMinimum(-2048);
    c_width_->setMaximum(2048);
    c_width_->setSingleStep(10);
    grid->addWidget(c_width_, row++, 1);
    connect(c_width_, SIGNAL(valueChanged(int)), this, SLOT(onCWidth()));

    grid->addWidget(new QLabel("Height", capture), row, 0);
    c_height_  = new QSpinBox(capture);
    c_height_->setKeyboardTracking(false);
    c_height_->setMinimum(-2048);
    c_height_->setMaximum(2048);
    c_height_->setSingleStep(10);
    grid->addWidget(c_height_, row++, 1);
    connect(c_height_, SIGNAL(valueChanged(int)), this, SLOT(onCHeight()));

    ui->toolBox->addItem(capture, QString("Capture Area"));
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
        c_include_cursor_->setChecked(item_->doesIncludeCursor());
        c_scalemode_->setCurrentIndex(item_->scaleMode());

        QRect rect = item_->capturedArea();
        c_x_->setValue(rect.x());
        c_y_->setValue(rect.y());
        c_width_->setValue(rect.width());
        c_height_->setValue(rect.height());
    });

    PropertyBase::update();
}

void ScreenCaptureProperty::onIncludeCursor()
{
    if(!item_) return;
    lockWith([this]() {
        item_->setIncludeCursor(c_include_cursor_->isChecked());
    });
}

void ScreenCaptureProperty::onScaleMode()
{
    if(!item_) return;
    lockWith([this]() {
        item_->setScaleMode((Qt::TransformationMode)c_scalemode_->currentIndex());
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

void ScreenCaptureProperty::showCaptureArea()
{
    if(!item_) return;
    item_->selectArea();
}

void ScreenCaptureProperty::onCX()
{
    if(!item_) return;
    lockWith([this]() {
        QRect rect = item_->capturedArea();
        rect.moveLeft(c_x_->value());
        item_->setCapturedArea(rect);
    });
}

void ScreenCaptureProperty::onCY()
{
    if(!item_) return;
    lockWith([this]() {
        QRect rect = item_->capturedArea();
        rect.moveTop(c_y_->value());
        item_->setCapturedArea(rect);
    });
}

void ScreenCaptureProperty::onCWidth()
{
    if(!item_) return;
    lockWith([this]() {
        QRect rect = item_->capturedArea();
        rect.setWidth(c_width_->value());
        item_->setCapturedArea(rect);
    });
    update();
}

void ScreenCaptureProperty::onCHeight()
{
    if(!item_) return;
    lockWith([this]() {
        QRect rect = item_->capturedArea();
        rect.setHeight(c_height_->value());
        item_->setCapturedArea(rect);
    });
    update();
}
