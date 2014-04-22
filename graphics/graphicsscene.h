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

#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include "item/igraphicsitem.h"

#include <QGraphicsScene>

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicsScene(QObject *parent = 0);

public slots:
    void setSelectedItemsPosition(IGraphicsItem::Position pos);
    void setSelectedItemsDefaultSize();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent) override;
};

#endif // GRAPHICSSCENE_H
