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

#include "propertybase.h"
#include "ui_propertybase.h"
#include "../item/igraphicsitem.h"

#include <QDebug>

PropertyBase::PropertyBase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropertyBase),
    lock_(false),
    item_(0),
    qitem_(0)
{
    ui->setupUi(this);
    connect(ui->l_pos_, SIGNAL(currentIndexChanged(int)), this, SLOT(onLPos(int)));
    connect(ui->l_x_, SIGNAL(valueChanged(int)), this, SLOT(onLX()));
    connect(ui->l_y_, SIGNAL(valueChanged(int)), this, SLOT(onLY()));
    connect(ui->l_aspectratio_, SIGNAL(currentIndexChanged(int)), this, SLOT(onLAspectRatioMode(int)));
    connect(ui->l_width_, SIGNAL(valueChanged(int)), this, SLOT(onLWidth()));
    connect(ui->l_height_, SIGNAL(valueChanged(int)), this, SLOT(onLHeight()));
    connect(ui->l_default_, SIGNAL(clicked()), this, SLOT(onLDefaultSize()));
    connect(ui->l_opacity_, SIGNAL(valueChanged(double)), this, SLOT(onOpacity()));
}

PropertyBase::~PropertyBase()
{
    delete ui;
}

void PropertyBase::setItem(IGraphicsItem *item)
{
    reset();
    item_ = item;
    qitem_ = item->item();
    connect(item_, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant)),
            this, SLOT(updateEventFilter(QGraphicsItem::GraphicsItemChange,QVariant)));
}

void PropertyBase::reset()
{
    if(item_) {
        disconnect(item_, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant)),
                this, SLOT(updateEventFilter(QGraphicsItem::GraphicsItemChange,QVariant)));
    }
    item_ = 0;
    qitem_ = 0;
}

void PropertyBase::update()
{
    if(!item_) return;
    lockWith([this]() {
        //static int count = 0;
        //qDebug() << "update" << ++count;
        ui->l_pos_->setCurrentIndex(item_->position());
        ui->l_x_->setValue((int)qitem_->x());
        ui->l_y_->setValue((int)qitem_->y());
        QSize s = item_->size();
        ui->l_width_->setValue(s.width());
        ui->l_height_->setValue(s.height());
        ui->l_opacity_->setValue(qitem_->opacity());
    });
}

PropertyBase *PropertyBase::nullProperty()
{
    static PropertyBase *null = 0;
    if(null) { return null; }
    null = new PropertyBase;
    null->ui->toolBox->hide();
    return null;
}

void PropertyBase::onLPos(int index)
{
    if(!item_) return;
    lockWith([&]() {
        item_->setPosition((IGraphicsItem::Position)index);
    });
    update();
}

void PropertyBase::onLX()
{
    if(!item_) return;
    lockWith([this]() {
        item_->setPosition(IGraphicsItem::MANUAL);
        qitem_->setX(ui->l_x_->value());
    });
    update();
}

void PropertyBase::onLY()
{
    if(!item_) return;
    lockWith([this]() {
        item_->setPosition(IGraphicsItem::MANUAL);
        qitem_->setY(ui->l_y_->value());
    });
    update();
}

void PropertyBase::onLAspectRatioMode(int index)
{
    if(!item_) return;
    lockWith([&]() {
        if(index == 0) {
            item_->setAspectRatioMode(Qt::KeepAspectRatio);

            QSize s = item_->nativeSize();
            s.scale(item_->size(), Qt::KeepAspectRatio);
            item_->setSize(s);
        } else {
            item_->setAspectRatioMode(Qt::IgnoreAspectRatio);
        }
        item_->update();
        item_->updatePos();
    });
    update();
}

void PropertyBase::onLWidth()
{
    if(!item_) return;
    lockWith([&]() {
        int width = ui->l_width_->value();
        int height;
        if(item_->aspectRatioMode() == Qt::KeepAspectRatio) {
            QSize s = item_->nativeSize();
            height = (width * s.height() + s.height()/2) / s.width();
        } else {
            height = ui->l_height_->value();
        }
        item_->setSize(QSize(width, height));
        item_->update();
        item_->updatePos();
    });
    update();
}

void PropertyBase::onLHeight()
{
    if(!item_) return;
    lockWith([&]() {
        int width;
        int height = ui->l_height_->value();
        if(item_->aspectRatioMode() == Qt::KeepAspectRatio) {
            QSize s = item_->nativeSize();
            width = (height * s.width() + s.width()/2) / s.height();
        } else {
            width = ui->l_width_->value();
        }
        item_->setSize(QSize(width, height));
        item_->update();
        item_->updatePos();
    });
    update();
}

void PropertyBase::onLDefaultSize()
{
    if(!item_) return;
    lockWith([&]() {
        item_->restoreDefaultSize();
    });
    update();
}

void PropertyBase::onOpacity()
{
    if(!item_) return;
    lockWith([&]() {
            qitem_->setOpacity(ui->l_opacity_->value());
    });
    update();
}

void PropertyBase::updateEventFilter(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    Q_UNUSED(value);
    switch(change) {
    case QGraphicsItem::ItemPositionHasChanged:
        update();
        break;
    default:
        // do nothing
        break;
    }
}
