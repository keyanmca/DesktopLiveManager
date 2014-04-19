#ifndef CAMERAPROPERTY_H
#define CAMERAPROPERTY_H

#include "propertybase.h"

#include <QList>
#include <QByteArray>

class Camera;

namespace Ui {
class CameraProperty;
}

class CameraProperty : public PropertyBase
{
    Q_OBJECT
public:
    CameraProperty(QWidget *parent = 0);
    void setItem(Camera *item);
    void reset() override;
    void update() override;

private slots:
    void onStart();
    void onStop();
private:
    Ui::CameraProperty *camera_ui;
    Camera *item_;
    QList<QByteArray> devices_;
};

#endif // CAMERAPROPERTY_H
