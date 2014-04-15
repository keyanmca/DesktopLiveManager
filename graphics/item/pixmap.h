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

#ifndef PIXMAP_H
#define PIXMAP_H

#include "igraphicsitem.h"

#include <QGraphicsPixmapItem>

class Pixmap : public QGraphicsPixmapItem, public IGraphicsItem
{
public:
    virtual ~Pixmap() override;
    void update() override;

    void setNativeSize(const QSize &size);
    virtual QSize nativeSize() const override;

    virtual void setAspectRatioMode(Qt::AspectRatioMode mode) override;
    virtual Qt::AspectRatioMode aspectRatioMode() const override;
    virtual void setSize(const QSize &size) override;
    virtual QSize size() const override;

protected:
    Pixmap(QGraphicsItem *parent = 0);
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event) override;
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QSize native_size_;
    Qt::AspectRatioMode mode_;
    QSize size_;
};

#endif // PIXMAP_H
