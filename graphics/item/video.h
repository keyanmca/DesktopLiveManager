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

#ifndef VIDEO_H
#define VIDEO_H

#include "igraphicsitem.h"

#include <QGraphicsVideoItem>

class QMediaPlayer;
class VideoMenu;

class Video : public QGraphicsVideoItem, public IGraphicsItem
{
public:
    Video(QGraphicsItem *parent = 0);
    ~Video() override;
    virtual int type() const override;

    virtual QSize nativeSize() const override;
    virtual void setAspectRatioMode(Qt::AspectRatioMode mode) override;
    virtual Qt::AspectRatioMode aspectRatioMode() const override;
    virtual void setSize(const QSize &size) override;
    virtual QSize size() const override;

    void openFile();
    QMediaPlayer* player();

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event) override;
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QMediaPlayer *player_;
    static VideoMenu *menu_;
};


class VideoMenu : public CommonMenu
{
public:
    VideoMenu(QObject *parent = 0);
    void setItem(Video *item);
    void reset();

public slots:

private:
    Video *item_; // no ownership
};

#endif // VIDEO_H
