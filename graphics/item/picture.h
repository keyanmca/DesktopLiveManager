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

#ifndef PICTURE_H
#define PICTURE_H

#include "pixmap.h"

class PictureMenu;

class Picture : public Pixmap
{
public:
    Picture(QGraphicsItem* parent = 0);
    void update() override;
    int type() const override;
    void advance(int phase) override;
    void setPicture(const QPixmap &pixmap); // hide QPixmap::setPixmap()
    void openFile();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    QPixmap original_;
    bool updated_;
    static PictureMenu *menu_;
};


class PictureMenu : public CommonMenu
{
public:
    PictureMenu(QObject *parent = 0);
    void setItem(Picture *item);
    void reset();

public slots:

private:
    Picture *item_; // no ownership
};

#endif // PICTURE_H
