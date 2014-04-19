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

#ifndef PROPERTYCONTROLLER_H
#define PROPERTYCONTROLLER_H

#include <QObject>

class QLayout;
class QGraphicsItem;

class PropertyBase;
class ScreenCaptureProperty;
class PictureProperty;
class VideoProperty;
class CameraProperty;
class TextProperty;

class PropertyController : public QObject
{
    Q_OBJECT
public:
    explicit PropertyController(QWidget *property, QObject *parent = 0);
    ~PropertyController();
    void setItem(QGraphicsItem *item);

private:
    QLayout *property_; // no ownership
    PropertyBase *current_; // no ownership (pointer to current item)
    PropertyBase *null_;
    ScreenCaptureProperty *p_sc_;
    PictureProperty *p_picture_;
    VideoProperty *p_video_;
    CameraProperty *p_camera_;
    TextProperty *p_text_;
};

#endif // PROPERTYCONTROLLER_H
