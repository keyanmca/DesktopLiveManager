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

#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include "igraphicsitem.h"

template <class T>
class GraphicsItem : public T, public IGraphicsItem
{
protected:
    GraphicsItem(QGraphicsItem *parent = 0) :
        T(parent),
        IGraphicsItem(this)
    {
    }

    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override
    {
        return onSceneEventFilter(watched, event);
    }

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override
    {
        onHoverEnterEvent(event);
    }

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override
    {
        onHoverLeaveEvent(event);
    }

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event) override
    {
        if(position() != IGraphicsItem::MANUAL) {
            setPosition(IGraphicsItem::MANUAL);
        }
        QGraphicsItem::mouseMoveEvent(event);
    }

    virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override
    {
        onWheelEvent(event);
    }

    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override
    {
        onItemChange(change, value);
        return T::itemChange(change, value);
    }
};

#endif // GRAPHICSITEM_H
