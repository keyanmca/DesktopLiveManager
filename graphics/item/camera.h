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

#ifndef CAMERA_H
#define CAMERA_H

#include "graphicsitem.h"

#include <QList>
#include <QByteArray>
#include <QGraphicsVideoItem>

class QCamera;
class CameraMenu;

class Camera : public GraphicsItem<QGraphicsVideoItem>
{
public:
    Camera(QGraphicsItem *parent = 0);
    ~Camera();
    virtual int type() const override;
    virtual QSize nativeSize() const override;
    virtual void setAspectRatioMode(Qt::AspectRatioMode mode) override;
    virtual Qt::AspectRatioMode aspectRatioMode() const override;
    virtual void setSize(const QSize &size) override;
    virtual QSize size() const override;

    void setDevice(const QByteArray &device);
    void start();
    void stop();

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    QCamera *camera_;
    static CameraMenu *menu_;
};


class CameraMenu : public CommonMenu
{
public:
    CameraMenu(QObject *parent = 0);
    void setItem(Camera *item);
    void reset();

private:
    Camera *item_; // no ownership
};

#endif // CAMERA_H
