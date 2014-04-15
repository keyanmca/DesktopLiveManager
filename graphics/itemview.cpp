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

#include "itemview.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QAction>
#include <QDebug>

ItemView::ItemView(QWidget *parent) :
    QListView(parent),
    menu_(0)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}

void ItemView::contextMenuEvent(QContextMenuEvent *event)
{
    if(menu_) {
        menu_->exec(event->globalPos());
    }
}

void ItemView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
}

void ItemView::setMenu(QMenu *menu)
{
    menu_ = menu;
}
