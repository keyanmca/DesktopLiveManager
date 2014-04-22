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

#ifndef TEXT_H
#define TEXT_H

#include "graphicsitem.h"

#include <QGraphicsTextItem>

class TextMenu;

class Text : public GraphicsItem<QGraphicsTextItem>
{
public:
    Text(QGraphicsItem *parent = 0);
    int type() const override;
    void setSize(const QSize &size) override;
    QSize size() const;
    void restoreDefaultSize() override;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    static TextMenu *menu_;
};


class TextMenu : public CommonMenu
{
public:
    TextMenu(QObject *parent = 0);
    void setItem(Text *item);
    void reset();

private:
    Text *item_; // no ownership
};

#endif // TEXT_H
