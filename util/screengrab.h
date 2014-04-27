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

#ifndef SCREENGRAB_H
#define SCREENGRAB_H

#include <QObject>
#include <QRect>
#include <QPixmap>

class MouseCursor;
class ScreenGrabPrivate;
class QScreen;

class ScreenGrab : public QObject
{
    Q_OBJECT
public:
    explicit ScreenGrab(QObject *parent = 0);
    ~ScreenGrab();

signals:
    void pixmapReady(QPixmap pixmap);

public slots:
    void capture(QScreen *screen,
                 QRect captured, QSize target, bool include_cursor,
                 Qt::AspectRatioMode AR_mode, Qt::TransformationMode TF_mode);

private:
    MouseCursor *cursor_;
    ScreenGrabPrivate *d_;
};

#endif // SCREENGRAB_H
