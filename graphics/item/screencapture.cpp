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

#include "screencapture.h"
#include "screenareaselector.h"

#include <QApplication>
#include <QScreen>
#include <QMenu>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QDebug>

ScreenCaptureMenu* ScreenCapture::menu_ = 0;

ScreenCapture::ScreenCapture(const QRect &area, QGraphicsItem* parent) :
    Pixmap(parent),
    screen_(QApplication::primaryScreen()),
    topleft_(area.topLeft()),
    mode_(Qt::FastTransformation),
    area_selector_(0)
{
    setNativeSize(area.size());
    setSize(area.size());

    if(!menu_) { // init static member
        menu_ = new ScreenCaptureMenu;
    }
}

ScreenCapture::~ScreenCapture()
{
    delete area_selector_;
}

int ScreenCapture::type() const
{
    return GraphicsItem::SCREEN_CAPTURE;
}

void ScreenCapture::advance(int phase)
{
    if(phase == 0 || !isVisible()) return;

    if(area_selector_) {
        setCapturedArea(area_selector_->geometry());
    }

    QPixmap pixmap = screen_->grabWindow(0, topleft_.x(), topleft_.y(),
                                         nativeSize().width(), nativeSize().height());

    if(size() == nativeSize()) {
        // setPixmap will results in calling PropertyBase::update()
        setPixmap(pixmap);
    } else {
        setPixmap(pixmap.scaled(size(), aspectRatioMode(), mode_));
    }
}

void ScreenCapture::setCapturedArea(const QRect &area)
{
    if(area_selector_) {
        area_selector_->setGeometry(area);
    }
    topleft_ = area.topLeft();
    setNativeSize(area.size());
    if(aspectRatioMode() == Qt::KeepAspectRatio) {
        setSize(area.size().scaled(size(), Qt::KeepAspectRatio));
    }
    updatePos();
}

QRect ScreenCapture::capturedArea() const
{
    return QRect(topleft_, nativeSize());
}

void ScreenCapture::setScaleMode(Qt::TransformationMode mode)
{
    mode_ = mode;
}

Qt::TransformationMode ScreenCapture::scaleMode() const
{
    return mode_;
}

void ScreenCapture::fillByScaling()
{
    QRectF s_rect = scene()->sceneRect();
    QSizeF old = size();
    QRect c_rect = capturedArea();

    QRect c_new_rect = c_rect;
    QSizeF test = old.scaled(s_rect.size(), Qt::KeepAspectRatioByExpanding);
    if(test.width() == s_rect.width()) {
        qreal c_h = c_rect.height() * s_rect.height() / test.height();
        c_new_rect.setHeight(int(c_h + 0.5));
        qreal delta_y = (c_rect.height() - c_h) / 2;
        c_new_rect.moveTop(c_rect.y() + int(delta_y + 0.5));
    } else {
        qreal c_w = c_rect.width() * s_rect.width() / test.width();
        c_new_rect.setWidth(int(c_w + 0.5));
        qreal delta_x = (c_rect.width() - c_w) / 2;
        c_new_rect.moveLeft(c_rect.x() + int(delta_x + 0.5));
    }
    setCapturedArea(c_new_rect);

    setSize(s_rect.size().toSize());
    setPosition(GraphicsItem::CENTER);
}

void ScreenCapture::fillByExpandingCaptureArea()
{
    QRectF s_rect = scene()->sceneRect();
    QSizeF old = size();
    QRect c_rect = capturedArea();
    QPointF old_pos = pos();

    qreal w = c_rect.width() * s_rect.width() / old.width();
    qreal h = c_rect.height() * s_rect.height() / old.height();
    qreal x = c_rect.x() - (old_pos.x() * c_rect.width() / old.width());
    qreal y = c_rect.y() - (old_pos.y() * c_rect.height() / old.height());
    setCapturedArea(QRect(int(x + 0.5), int(y + 0.5), int(w + 0.5), int(h + 0.5)));

    setSize(s_rect.size().toSize());
    setPosition(GraphicsItem::CENTER);
}

void ScreenCapture::fitByScaling()
{
    QRectF s_rect = scene()->sceneRect();
    QSizeF old = size();

    setSize(old.scaled(s_rect.size(), Qt::KeepAspectRatio).toSize());
    setPosition(GraphicsItem::CENTER);
}

void ScreenCapture::fullScreen()
{
    setCapturedArea(QRect(QPoint(0,0),screen_->size()));
    fitByScaling();
}

void ScreenCapture::showAreaSelector()
{
    if(area_selector_) return;
    area_selector_ = new ScreenAreaSelector;
    area_selector_->setGeometry(topleft_.x(), topleft_.y(),
                                nativeSize().width(), nativeSize().height());
    area_selector_->show();
    connect(area_selector_, &ScreenAreaSelector::windowClosed, [this]() { area_selector_ = 0; });
}

void ScreenCapture::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    menu_->setItem(this);
    menu_->menu()->exec(event->screenPos());
}


// PictureMenu Class
ScreenCaptureMenu::ScreenCaptureMenu(QObject *parent) :
    CommonMenu(parent)
{
    connect(menu_->addAction("Show Area Selector"), &QAction::triggered,
            [&]() { if(item_) { item_->showAreaSelector(); } });

    QMenu *fit_and_fill = new QMenu("Fit And Fill", menu_);
    connect(fit_and_fill->addAction("Fill By Scaling"), &QAction::triggered,
            [&]() { if(item_) { item_->fillByScaling(); } });
    connect(fit_and_fill->addAction("Fill By Expanding Capture Area"), &QAction::triggered,
            [&]() { if(item_) { item_->fillByExpandingCaptureArea(); } });
    connect(fit_and_fill->addAction("Fit By Scaling"), &QAction::triggered,
            [&]() { if(item_) { item_->fitByScaling(); } });
    connect(fit_and_fill->addAction("Full Screen"), &QAction::triggered,
            [&]() { if(item_) { item_->fullScreen(); } });
    menu_->addMenu(fit_and_fill);
}

void ScreenCaptureMenu::setItem(ScreenCapture *item)
{
    CommonMenu::setItem(item);
    item_ = item;
}

void ScreenCaptureMenu::reset()
{
    item_ = 0;
    CommonMenu::reset();
}