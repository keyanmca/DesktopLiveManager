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

#include "camera.h"

#include <QCamera>
#include <QMenu>
#include <QGraphicsSceneEvent>
#include <QDebug>

CameraMenu* Camera::menu_ = 0;

Camera::Camera(QGraphicsItem *parent) :
    GraphicsItem(parent),
    camera_(0)
{
    setAcceptHoverEvents(true);

    if(!menu_) { // init static member
        menu_ = new CameraMenu;
    }
}

Camera::~Camera()
{
    stop();
    delete camera_;
}

QSize Camera::nativeSize() const
{
    QSizeF s = QGraphicsVideoItem::nativeSize();
    return QSize(s.width(), s.height());
}

void Camera::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    QGraphicsVideoItem::setAspectRatioMode(mode);
}

Qt::AspectRatioMode Camera::aspectRatioMode() const
{
    return QGraphicsVideoItem::aspectRatioMode();
}

void Camera::setSize(const QSize &size)
{
    QGraphicsVideoItem::setSize(size);
}

QSize Camera::size() const
{
    QSizeF s = QGraphicsVideoItem::size();
    return QSize(s.width(), s.height());
}

void Camera::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    menu_->setItem(this);
    menu_->menu()->exec(event->screenPos());
}

int Camera::type() const
{
    return IGraphicsItem::CAMERA;
}

void Camera::setDevice(const QByteArray &device)
{
    stop();
    delete camera_;

    camera_ = new QCamera(device);
    camera_->setViewfinder(this);
}

void Camera::start()
{
    if(!camera_ || camera_->state() == QCamera::ActiveState) return;
    camera_->start();
}

void Camera::stop()
{
    if(!camera_) return;
    camera_->stop();
}


CameraMenu::CameraMenu(QObject *parent) :
    CommonMenu(parent)
{
}

void CameraMenu::setItem(Camera *item)
{
    CommonMenu::setItem(item);
    item_ = item;
}

void CameraMenu::reset()
{
    item_ = 0;
    CommonMenu::reset();
}
