#include "camera.h"

#include <QCamera>

Camera::Camera(QGraphicsItem *parent) :
    QGraphicsVideoItem(parent),
    IGraphicsItem(this),
    camera_(0)
{
}

Camera::~Camera()
{
    stop();
    delete camera_;
}

int Camera::type() const
{
    return GraphicsItem::CAMERA;
}

void Camera::setDevice(const QByteArray &device)
{
    stop();
    delete camera_;

    camera_ = new QCamera(device);
    camera_->setViewfinder(this);
}

void Camera::start()
{
    if(!camera_ || camera_->state() == QCamera::ActiveState) return;
    camera_->start();
}

void Camera::stop()
{
    if(!camera_) return;
    camera_->stop();
}
