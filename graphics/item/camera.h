#ifndef CAMERA_H
#define CAMERA_H

#include "igraphicsitem.h"

#include <QList>
#include <QByteArray>
#include <QGraphicsVideoItem>

class QCamera;

class Camera : public QGraphicsVideoItem, public IGraphicsItem
{
public:
    Camera(QGraphicsItem *parent = 0);
    ~Camera();
    virtual int type() const override;
    void setDevice(const QByteArray &device);
    void start();
    void stop();

private:
    QCamera *camera_;
};

#endif // CAMERA_H
