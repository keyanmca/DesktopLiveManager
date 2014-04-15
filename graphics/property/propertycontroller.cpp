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

#include "propertycontroller.h"
#include "../item/igraphicsitem.h"
#include "../item/screencapture.h"
#include "../item/picture.h"
#include "../item/video.h"
#include "../item/text.h"
#include "propertybase.h"
#include "screencaptureproperty.h"
#include "pictureproperty.h"
#include "videoproperty.h"
#include "textproperty.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>

PropertyController::PropertyController(QWidget *property, QObject *parent) :
    QObject(parent),
    property_(new QVBoxLayout),
    current_(0),
    null_(0),
    p_sc_(0),
    p_picture_(0),
    p_video_(0),
    p_text_(0)
{
    property->setLayout(property_);

    null_ = PropertyBase::nullProperty();
    property_->addWidget(null_);
    current_ = null_;
    null_->show();
}

PropertyController::~PropertyController()
{
    delete null_;
    delete p_sc_;
    delete p_picture_;
    delete p_video_;
    delete p_text_;
}

void PropertyController::setItem(QGraphicsItem *item)
{
    current_->reset();
    current_->hide();
    property_->removeWidget(current_);
    current_ = null_;

    if(!item) {
        property_->addWidget(current_);
        current_->show();
        return;
    }

    switch(item->type()) {
    case GraphicsItem::SCREEN_CAPTURE:
        ScreenCapture *sc;
        if((sc = dynamic_cast<ScreenCapture*>(item))) {
            if(!p_sc_) {
                p_sc_ = new ScreenCaptureProperty;
            }

            p_sc_->setItem(sc);
            current_ = p_sc_;
        }
        break;
    case GraphicsItem::PICTURE:
        Picture *pic;
        if((pic = dynamic_cast<Picture*>(item))) {
            if(!p_picture_) {
                p_picture_ = new PictureProperty;
            }

            p_picture_->setItem(pic);
            current_ = p_picture_;
        }
        break;
    case GraphicsItem::VIDEO:
        Video *video;
        if((video = dynamic_cast<Video*>(item))) {
            if(!p_video_) {
                p_video_ = new VideoProperty;
            }

            p_video_->setItem(video);
            current_ = p_video_;
        }
        break;
    case GraphicsItem::TEXT:
        Text *text;
        if((text = dynamic_cast<Text*>(item))) {
            if(!p_text_) {
                p_text_ = new TextProperty;
            }

            p_text_->setItem(text);
            current_ = p_text_;
        }
        break;
    default:
        current_ = null_;
        break;
    }

    property_->addWidget(current_);
    current_->show();
    current_->update();
}
