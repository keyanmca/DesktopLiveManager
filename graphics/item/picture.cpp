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

#include "picture.h"

#include <QMenu>
#include <QFileDialog>
#include <QGraphicsSceneEvent>

PictureMenu* Picture::menu_ = 0;

Picture::Picture(QGraphicsItem *parent) :
    Pixmap(parent),
    original_(QPixmap(":/image/image/no_image.gif")),
    updated_(false)
{
    setPicture(original_);
    setSize(original_.size());

    if(!menu_) { // init static member
        menu_ = new PictureMenu;
    }
}

void Picture::update()
{
    updated_ = true;
    Pixmap::update();
}

int Picture::type() const
{
    return IGraphicsItem::PICTURE;
}

void Picture::advance(int phase)
{
    if(phase == 0 || !updated_) return;
    updated_ = false;

    if(size() == nativeSize()) {
        setPicture(original_);
    } else {
        setPixmap(original_.scaled(
                      size(), aspectRatioMode(), Qt::SmoothTransformation));
    }
}

void Picture::setPicture(const QPixmap &pixmap)
{
    original_ = pixmap;
    setNativeSize(QSize(original_.width(), original_.height()));
    setPixmap(original_.scaled(
                  size(), aspectRatioMode(), Qt::SmoothTransformation));
}

void Picture::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    menu_->setItem(this);
    menu_->menu()->exec(event->screenPos());
}

void Picture::openFile()
{
    QString file = QFileDialog::getOpenFileName(0, "Open File", QDir::homePath());
    QPixmap img = QPixmap(file);
    setPicture(img);
}


// PictureMenu Class
PictureMenu::PictureMenu(QObject *parent) :
    CommonMenu(parent)
{
}

void PictureMenu::setItem(Picture *item)
{
    CommonMenu::setItem(item);
    item_ = item;
}

void PictureMenu::reset()
{
    item_ = 0;
    CommonMenu::reset();
}
