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

#include "igraphicsitem.h"
#include "../graphicscontroller.h"

#include <QMenu>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneWheelEvent>
#include <QDebug>

CornerGrabber* IGraphicsItem::grabber_[4];
CornerGrabber* IGraphicsItem::container_ = 0;

IGraphicsItem::IGraphicsItem(QGraphicsItem *item, QObject *parent) :
    QObject(parent),
    item_(item),
    position_(IGraphicsItem::CENTER),
    moving_(false),
    hovered_(false)
{
    item_->setFlags(QGraphicsItem::ItemIsMovable |
                    QGraphicsItem::ItemIsSelectable |
                    QGraphicsItem::ItemSendsGeometryChanges);

    if(container_ == 0) { // init static members at the first call
        container_ = new CornerGrabber(-1, -1);
        grabber_[0] = new CornerGrabber(0, 0, container_);
        grabber_[1] = new CornerGrabber(0, 1, container_);
        grabber_[2] = new CornerGrabber(1, 0, container_);
        grabber_[3] = new CornerGrabber(1, 1, container_);
    }
}

IGraphicsItem::~IGraphicsItem()
{
    // release all grabber
    if(grabber_[0]->parentItem() != container_) {
        setGrabbersParent(container_);
    }
}

QGraphicsItem *IGraphicsItem::item()
{
    return item_;
}

void IGraphicsItem::setPosition(IGraphicsItem::Position pos)
{
    position_ = pos;
    updatePos();
}

IGraphicsItem::Position IGraphicsItem::position() const
{
    return position_;
}

void IGraphicsItem::updatePos()
{
    if(position_ != IGraphicsItem::MANUAL) {
        QRectF s_rect = item_->scene()->sceneRect();
        QRectF b_rect = QRectF(QPointF(0, 0), size());
        QPointF new_pos;
        switch(position_) {
        case IGraphicsItem::CENTER:
            new_pos = QPointF((s_rect.width() - b_rect.width()) / 2, (s_rect.height() - b_rect.height()) / 2);
            break;
        case IGraphicsItem::TOP_LEFT:
            new_pos = QPointF(0, 0);
            break;
        case IGraphicsItem::TOP_RIGHT:
            new_pos = QPointF(s_rect.width() - b_rect.width() ,0);
            break;
        case IGraphicsItem::BOTTOM_LEFT:
            new_pos = QPointF(0, s_rect.height() - b_rect.height());
            break;
        case IGraphicsItem::BOTTOM_RIGHT:
            new_pos = QPointF(s_rect.width() - b_rect.width(), s_rect.height() - b_rect.height());
            break;
        default:
            Q_ASSERT(!"Unknown value");
            break;
        }
        item_->setPos(new_pos);
    }
    emit itemChanged(QGraphicsItem::ItemPositionHasChanged, QVariant());
}

void IGraphicsItem::restoreDefaultSize()
{
    setAspectRatioMode(Qt::KeepAspectRatio);
    setSize(nativeSize());
    update();
    updatePos();
}

void IGraphicsItem::setupGrabbers()
{
    setGrabbersParent(item_);
    setGrabbersState(false);
    grabber_[0]->installSceneEventFilter(item_);
    grabber_[1]->installSceneEventFilter(item_);
    grabber_[2]->installSceneEventFilter(item_);
    grabber_[3]->installSceneEventFilter(item_);
    setGrabbersPosition(item_->boundingRect());
}

bool IGraphicsItem::onSceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    auto *corner = dynamic_cast<CornerGrabber*>(watched);
    if(!corner) return false;

    auto *mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if(!mevent) return false;

    switch(event->type()) {
    case QEvent::GraphicsSceneMousePress:
        moving_ = true;
        start_pos_ = mevent->scenePos();
        setGrabbersState(true);
        return true;
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseMove:
        break;
    default:
        return false;
    }

    if(!moving_) return true;

    QPointF delta = mevent->scenePos() - start_pos_;
    if(aspectRatioMode() == Qt::KeepAspectRatio) {
        QSizeF s = size();
        qreal grad = s.height() / s.width();

        if(corner->iX() ^ corner->iY()) {
            grad = -grad;
        }

        if((delta.y() > grad * delta.x()) ^ !corner->iY()) {
            delta.setX(delta.y() / grad);
        } else {
            delta.setY(grad * delta.x());
        }
    }

    QRectF rect = item_->boundingRect();

    qreal x, y, w, h;

    if(corner->iX() == 0) {
        x = rect.x() + delta.x();
        w = rect.width() - delta.x();
    } else {
        x = rect.x();
        w = rect.width() + delta.x();
    }

    if(corner->iY() == 0) {
        y = rect.y() + delta.y();
        h = rect.height() - delta.y();
    } else {
        y = rect.y();
        h = rect.height() + delta.y();
    }

    if(event->type() == QEvent::GraphicsSceneMouseRelease) {
        moving_ = false;
        setSize(QSize(w - 1, h - 1));
        update();
        setGrabbersState(false);
        item_->setPos(item_->pos() + QPointF(x + 0.5, y + 0.5));
        updatePos();
        x = rect.x();
        y = rect.y();
    }
    setGrabbersPosition(QRectF(x,y,w,h));

    return true;

}

void IGraphicsItem::onHoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    hovered_ = true;
    if(item_->isSelected()) {
        setupGrabbers();
    }
}

void IGraphicsItem::onHoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    hovered_ = false;
    setGrabbersParent(container_);
}

void IGraphicsItem::onItemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemSelectedHasChanged) {
        if(item_->isSelected() && hovered_) {
            setupGrabbers();
        }
    }

    emit itemChanged(change, value);
}


void IGraphicsItem::onWheelEvent(QGraphicsSceneWheelEvent *event)
{
    if(!item_->isSelected()) return;

    int d = event->delta();
    int w_delta;
    if(d >= 120) {
        w_delta = 10;
    } else if(d <= -120) {
        w_delta = -10;
    } else {
        return;
    }

    QSize s = size();
    int w, h;
    if(aspectRatioMode() == Qt::KeepAspectRatio) {
        QSize n = nativeSize();
        w = s.width() + w_delta;
        h = (w * n.height() + n.width()/2)/ n.width();
    } else {
        w = s.width() + w_delta;
        h = (w * s.height() + s.width()/2)/ s.width();
    }
    setSize(QSize(w, h));

    update();
    updatePos();
    setGrabbersPosition(item_->boundingRect());
}

void IGraphicsItem::setGrabbersParent(QGraphicsItem *parent)
{
    grabber_[0]->setParentItem(parent);
    grabber_[1]->setParentItem(parent);
    grabber_[2]->setParentItem(parent);
    grabber_[3]->setParentItem(parent);
}

void IGraphicsItem::setGrabbersPosition(const QRectF &rect)
{
    QRectF g_rect = grabber_[0]->boundingRect();
    qreal x0 = rect.x() - g_rect.x();
    qreal x1 = x0 + rect.width() - g_rect.width();
    qreal y0 = rect.y() - g_rect.y();
    qreal y1 = y0 + rect.height() - g_rect.height();

    grabber_[0]->setPos(x0, y0);
    grabber_[1]->setPos(x0, y1);
    grabber_[2]->setPos(x1, y0);
    grabber_[3]->setPos(x1, y1);
}

void IGraphicsItem::setGrabbersState(bool active)
{
    if(active) {
        for(auto g : grabber_) {
            g->setPen(QPen(Qt::red));
            g->setBrush(QBrush(QColor(Qt::red)));
        }
    } else {
        for(auto g : grabber_) {
            g->setPen(QPen(Qt::black));
            g->setBrush(QBrush(QColor(Qt::black)));
        }
    }
}

// CommonMenu
CommonMenu::CommonMenu(QObject *parent) :
    QObject(parent)
{
    GraphicsController* gc = GraphicsController::instance();

    menu_ = new QMenu;

    QMenu *add = new QMenu("Add", menu_);
    connect(add->addAction("Add ScreenCapture"), SIGNAL(triggered()),
            gc, SLOT(addScreenCapture()));
    connect(add->addAction("Add Picture"), SIGNAL(triggered()),
            gc, SLOT(addPicture()));
    connect(add->addAction("Add Video"), SIGNAL(triggered()),
            gc, SLOT(addVideo()));
    connect(add->addAction("Add Text"), SIGNAL(triggered()),
            gc, SLOT(addText()));
    menu_->addMenu(add);

    connect(menu_->addAction("Remove"), SIGNAL(triggered()),
            gc, SLOT(removeSelectedItems()));

    menu_->addSeparator();

    QMenu *pos = new QMenu("Position", menu_);
    connect(pos->addAction("Center"), &QAction::triggered,
            [&]() { if(item_) { item_->setPosition(IGraphicsItem::CENTER); } });
    connect(pos->addAction("TopLeft"), &QAction::triggered,
            [&]() { if(item_) { item_->setPosition(IGraphicsItem::TOP_LEFT); } });
    connect(pos->addAction("TopRight"), &QAction::triggered,
            [&]() { if(item_) { item_->setPosition(IGraphicsItem::TOP_RIGHT); } });
    connect(pos->addAction("BottomLeft"), &QAction::triggered,
            [&]() { if(item_) { item_->setPosition(IGraphicsItem::BOTTOM_LEFT); } });
    connect(pos->addAction("BottomRight"), &QAction::triggered,
            [&]() { if(item_) { item_->setPosition(IGraphicsItem::BOTTOM_RIGHT); } });
    menu_->addMenu(pos);

    connect(menu_->addAction("Default Size"), &QAction::triggered,
            [&]() { if(item_) { item_->restoreDefaultSize(); } });

}

CommonMenu::~CommonMenu()
{
    delete menu_;
}

QMenu *CommonMenu::menu()
{
    return menu_;
}

void CommonMenu::setItem(IGraphicsItem *item)
{
    item_ = item;
}

void CommonMenu::reset()
{
    item_ = 0;
}
