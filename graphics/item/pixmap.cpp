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

#include "pixmap.h"

#include <QGraphicsSceneWheelEvent>
#include <QDebug>

Pixmap::Pixmap(QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent),
    IGraphicsItem(this),
    mode_(Qt::KeepAspectRatio)
{
    setAcceptHoverEvents(true);
}

Pixmap::~Pixmap()
{
}

void Pixmap::update()
{
    setPixmap(pixmap().scaled(size(), aspectRatioMode(), Qt::FastTransformation));
}

void Pixmap::setNativeSize(const QSize &size)
{
    native_size_ = size;
}

QSize Pixmap::nativeSize() const
{
    return native_size_;
}

void Pixmap::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    mode_ = mode;
}

Qt::AspectRatioMode Pixmap::aspectRatioMode() const
{
    return mode_;
}

void Pixmap::setSize(const QSize &size)
{
    size_ = size;
}

QSize Pixmap::size() const
{
    return size_;
}

bool Pixmap::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    return onSceneEventFilter(watched, event);
}

void Pixmap::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    onHoverEnterEvent(event);
}

void Pixmap::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    onHoverLeaveEvent(event);
}

void Pixmap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(position() != GraphicsItem::MANUAL) {
        setPosition(GraphicsItem::MANUAL);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void Pixmap::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    onWheelEvent(event);
}

QVariant Pixmap::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    onItemChange(change, value);
    return QGraphicsPixmapItem::itemChange(change, value);
}
