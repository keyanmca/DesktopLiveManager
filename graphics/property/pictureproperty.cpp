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

#include "pictureproperty.h"
#include "ui_propertybase.h"
#include "ui_pictureproperty.h"
#include "../item/picture.h"

PictureProperty::PictureProperty(QWidget *parent) :
    PropertyBase(parent),
    picture_ui(new Ui::PictureProperty),
    item_(0)
{
    QWidget *picture = new QWidget;

    ui->toolBox->addItem(picture, QString("Picture"));
    picture_ui->setupUi(picture);

    connect(picture_ui->p_open_, SIGNAL(clicked()), this, SLOT(onOpen()));
}

void PictureProperty::setItem(Picture *item)
{
    reset();
    PropertyBase::setItem(item);
    item_ = item;
}

void PictureProperty::onOpen()
{
    if(!item_) return;
    lockWith([&]() {
        item_->openFile();
    });
}
