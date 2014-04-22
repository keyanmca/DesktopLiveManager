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
