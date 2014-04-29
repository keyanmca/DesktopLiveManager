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

#include "screenareaselector.h"

#include <QRubberBand>
#include <QEvent>
#include <QMouseEvent>

ScreenAreaSelector::ScreenAreaSelector(QWidget *parent) :
    QWidget(parent),
    rubber_band_(0)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    showFullScreen();
}

ScreenAreaSelector::~ScreenAreaSelector()
{
}

void ScreenAreaSelector::mousePressEvent(QMouseEvent *event)
{
    origin_ = event->pos();
    if (!rubber_band_)
        rubber_band_ = new QRubberBand(QRubberBand::Rectangle, this);
    rubber_band_->setGeometry(QRect(origin_, QSize()));
    rubber_band_->show();
}

void ScreenAreaSelector::mouseMoveEvent(QMouseEvent *event)
{
    rubber_band_->setGeometry(QRect(origin_, event->pos()).normalized());
}

void ScreenAreaSelector::mouseReleaseEvent(QMouseEvent *event)
{
    rubber_band_->hide();
    emit areaSelected(QRect(origin_, event->pos()).normalized());
}
