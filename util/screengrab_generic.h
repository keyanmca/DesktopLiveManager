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

#include "screengrab.h"
#include "mousecursor.h"

#include <QImage>
#include <QPainter>
#include <QScreen>
#include <QTime>
#include <QMetaType>
#include <QDebug>

struct ScreenGrabPrivate
{
};

Q_DECLARE_METATYPE(Qt::AspectRatioMode)
Q_DECLARE_METATYPE(Qt::TransformationMode)

ScreenGrab::ScreenGrab(QObject *parent) :
    QObject(parent),
    cursor_(new MouseCursor),
    d_(new ScreenGrabPrivate)
{
    qRegisterMetaType<Qt::AspectRatioMode>();
    qRegisterMetaType<Qt::TransformationMode>();
    qDebug() << "using screengrab_generic.cpp";
}

ScreenGrab::~ScreenGrab()
{
    delete d_;
    delete cursor_;
}

void ScreenGrab::capture(
        QScreen *screen,
        QRect captured, QSize target, bool include_cursor,
        Qt::AspectRatioMode AR_mode, Qt::TransformationMode TF_mode)
{
    QPixmap pixmap = screen->grabWindow(
                0, captured.x(), captured.y(),
                captured.width(), captured.height());

    if(include_cursor) {
        QPainter p(&pixmap);
        cursor_->update();
        p.drawImage(cursor_->pos() - captured.topLeft(), cursor_->image());
    }

    if(target == captured.size()) {
        emit pixmapReady(pixmap);
    } else {
        emit pixmapReady(pixmap.scaled(target, AR_mode, TF_mode));
    }
}
