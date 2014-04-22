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

#include "video.h"

#include <QMenu>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QGraphicsSceneEvent>

VideoMenu* Video::menu_ = 0;

Video::Video(QGraphicsItem *parent) :
    GraphicsItem(parent),
    player_(0)
{
    setAcceptHoverEvents(true);

    QGraphicsVideoItem::setAspectRatioMode(Qt::KeepAspectRatio);
    QGraphicsVideoItem::setSize(QSize(320, 240));

    player_ = new QMediaPlayer;
    player_->setVideoOutput(this);
    player_->setVolume(30);

    if(!menu_) { // init static member
        menu_ = new VideoMenu;
    }
}

Video::~Video()
{
    player_->stop();
    delete player_;
}

int Video::type() const
{
    return IGraphicsItem::VIDEO;
}

QSize Video::nativeSize() const
{
    QSizeF s = QGraphicsVideoItem::nativeSize();
    return QSize(s.width(), s.height());
}

void Video::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    QGraphicsVideoItem::setAspectRatioMode(mode);
}

Qt::AspectRatioMode Video::aspectRatioMode() const
{
    return QGraphicsVideoItem::aspectRatioMode();
}

void Video::setSize(const QSize &size)
{
    QGraphicsVideoItem::setSize(size);
}

QSize Video::size() const
{
    QSizeF s = QGraphicsVideoItem::size();
    return QSize(s.width(), s.height());
}

void Video::openFile()
{
    QUrl file = QFileDialog::getOpenFileUrl(0, "Open File", QUrl::fromLocalFile(QDir::homePath()));
    player_->setMedia(file);
}

QMediaPlayer *Video::player()
{
    return player_;
}

void Video::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    menu_->setItem(this);
    menu_->menu()->exec(event->screenPos());
}


// VideoMenu Class
VideoMenu::VideoMenu(QObject *parent) :
    CommonMenu(parent)
{
}

void VideoMenu::setItem(Video *item)
{
    CommonMenu::setItem(item);
    item_ = item;
}

void VideoMenu::reset()
{
    item_ = 0;
    CommonMenu::reset();
}
