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

#ifndef IGRAPHICSITEM_H
#define IGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>

class QMenu;

class CornerGrabber : public QGraphicsRectItem
{
public:
    explicit CornerGrabber(int ix, int iy, QGraphicsItem *parent = 0) :
        QGraphicsRectItem(parent), ix_(ix), iy_(iy)
    {
        setRect(0, 0, 16, 16);
    }

    int iX() const { return ix_; }
    int iY() const { return iy_; }

private:
    int ix_;
    int iy_;
};

class IGraphicsItem : public QObject
{
     Q_OBJECT
public:
    enum Type
    {
        SCREEN_CAPTURE = QGraphicsItem::UserType + 1,
        PICTURE,
        VIDEO,
        CAMERA,
        TEXT
    };

    enum Position
    {
        MANUAL = 0,
        CENTER,
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
    };

    virtual ~IGraphicsItem();
    QGraphicsItem* item();
    virtual void update() {}
    virtual QMenu* menu() { return 0; }

    // position
    void setPosition(IGraphicsItem::Position pos);
    IGraphicsItem::Position position() const;
    void updatePos(); // updatePos emits itemChanged to update property

    // size
    virtual void setAspectRatioMode(Qt::AspectRatioMode mode) { Q_UNUSED(mode); }
    virtual Qt::AspectRatioMode aspectRatioMode() const { return Qt::KeepAspectRatio; } // keep or ignore
    virtual QSize nativeSize() const { return QSize(); }
    virtual void setSize(const QSize &size) { Q_UNUSED(size); }
    virtual QSize size() const { return QSize(); }
    virtual void restoreDefaultSize();

signals:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & value);

protected:
    IGraphicsItem(QGraphicsItem *item, QObject *parent = 0);

    // grabbers
    bool onSceneEventFilter(QGraphicsItem *watched, QEvent *event);
    void onHoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void onHoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void onItemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

    // wheel resizing
    void onWheelEvent(QGraphicsSceneWheelEvent *event);

private:
    QGraphicsItem *item_; // doesn't have ownership

    IGraphicsItem::Position position_;

    // for grabbers
    void setupGrabbers();
    void setGrabbersParent(QGraphicsItem* parent);
    void setGrabbersPosition(const QRectF &rect);
    void setGrabbersState(bool active);

    QPointF start_pos_;
    bool moving_;
    bool hovered_;
    static CornerGrabber *grabber_[4];
    static CornerGrabber *container_;
};


class CommonMenu : public QObject
{
public:
    CommonMenu(QObject *parent = 0);
    virtual ~CommonMenu();
    QMenu* menu();
    void setItem(IGraphicsItem *item);
    void reset();

protected:
    QMenu *menu_;

private:
    IGraphicsItem *item_; // no ownership
};

#endif // IGRAPHICSITEM_H
