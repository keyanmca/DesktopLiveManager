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

#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include "pixmap.h"

class QScreen;
class QRect;
class QDragMoveEvent;
class ScreenAreaSelector;
class ScreenCaptureMenu;
class MouseCursor;

class ScreenCapture : public Pixmap
{
    Q_OBJECT
public:
    ScreenCapture(const QRect &area, QGraphicsItem* parent = 0);
    ~ScreenCapture() override;
    int type() const override;
    void advance(int phase) override;

    void setIncludeCursor(bool state);
    bool doesIncludeCursor() const;
    void setCapturedArea(const QRect &area);
    QRect capturedArea() const;
    void setScaleMode(Qt::TransformationMode mode);
    Qt::TransformationMode scaleMode() const;
    void fillByScaling();
    void fillByExpandingCaptureArea();
    void fitByScaling();
    void fullScreen();
    void showAreaSelector();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    QScreen *screen_; // take ownership ?
    QPoint topleft_;
    Qt::TransformationMode mode_;
    ScreenAreaSelector *area_selector_;
    MouseCursor *cursor_;
    bool include_cursor_;
    static ScreenCaptureMenu *menu_;
};


class ScreenCaptureMenu : public CommonMenu
{
public:
    ScreenCaptureMenu(QObject *parent = 0);
    void setItem(ScreenCapture *item);
    void reset();

public slots:

private:
    ScreenCapture *item_; // no ownership
};

#endif // SCREENCAPTURE_H
