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

#include "text.h"

#include <QMenu>
#include <QGraphicsSceneEvent>
#include <QDebug>

TextMenu* Text::menu_ = 0;

Text::Text(QGraphicsItem *parent) :
    GraphicsItem(parent)
{
    setAcceptHoverEvents(false);
    setHtml("<h1> Text </h1>");

    if(!menu_) { // init static member
        menu_ = new TextMenu;
    }
}

int Text::type() const
{
    return IGraphicsItem::TEXT;
}

void Text::setSize(const QSize &size)
{
    setTextWidth(size.width());
}

QSize Text::size() const
{
    QRectF rect = boundingRect();
    return QSize(rect.width() - 1, rect.height() - 1);
}

void Text::restoreDefaultSize()
{
    setTextWidth(-1);
    updatePos();
}

void Text::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    menu_->setItem(this);
    menu_->menu()->exec(event->screenPos());
}


// TextMenu Class
TextMenu::TextMenu(QObject *parent) :
    CommonMenu(parent)
{
}

void TextMenu::setItem(Text *item)
{
    CommonMenu::setItem(item);
    item_ = item;
}

void TextMenu::reset()
{
    item_ = 0;
    CommonMenu::reset();
}
