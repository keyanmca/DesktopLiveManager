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

#include "graphicsscene.h"
#include "graphicscontroller.h"

#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>

GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent),
    scene_menu_(0)
{
}

void GraphicsScene::setMenu(QMenu *menu)
{
    scene_menu_ = menu;
}

void GraphicsScene::setSelectedItemsPosition(IGraphicsItem::Position pos)
{
    for(auto i : selectedItems()) {
        IGraphicsItem *item = dynamic_cast<IGraphicsItem*>(i);
        if(item) {
            item->setPosition(pos);
        }
    }
}

void GraphicsScene::setSelectedItemsDefaultSize()
{
    for(auto i : selectedItems()) {
        IGraphicsItem *item = dynamic_cast<IGraphicsItem*>(i);
        if(item) {
            item->restoreDefaultSize();
        }
    }
}

void GraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    auto under_cursor = items(event->scenePos());
    if(under_cursor.count() > 0) {
        if(under_cursor.front()->isSelected()) {
            if(selectedItems().count() == 1) {
                // unique menu
                QGraphicsScene::contextMenuEvent(event);
            } else {
                // common item menu
                GraphicsController::instance()->itemMenu()->exec(event->screenPos());
            }
        } else {
            // select new item and show unique menu
            clearSelection();
            under_cursor.front()->setSelected(true);
            QGraphicsScene::contextMenuEvent(event);
        }
    } else {
        // scene menu
        clearSelection();
        if(scene_menu_) {
            scene_menu_->exec(event->screenPos());
        }
    }
}

void GraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent)
{
    if(wheelEvent->modifiers() & Qt::ControlModifier) return;

    for(auto p : selectedItems()) {
        sendEvent(p, (QEvent*)wheelEvent);
    }
}
