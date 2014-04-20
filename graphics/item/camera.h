#ifndef CAMERA_H
#define CAMERA_H

#include "igraphicsitem.h"

#include <QList>
#include <QByteArray>
#include <QGraphicsVideoItem>

class QCamera;
class CameraMenu;

class Camera : public QGraphicsVideoItem, public IGraphicsItem
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
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event) override;
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

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
