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

#ifndef GRAPHICSCONTROLLER_H
#define GRAPHICSCONTROLLER_H

#include <QObject>
#include <QList>

#include "itemmodel.h"
#include "graphicsscene.h"

class QLayout;
class QMenu;
class QPaintDevice;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsItem;
class QStandardItemModel;

class ItemView;
class PropertyController;
class IGraphicsItem;

// This class is a facade of all graphics operations.
class GraphicsController : public QObject
{
    Q_OBJECT
public:
    // SINGLETON PATTERN
    static GraphicsController* createInstance(
        QWidget *view, QWidget *scene_tab, QWidget *graphics_tab, QObject *parent = 0);
    static GraphicsController* instance();
    GraphicsController(const GraphicsController&) = delete;
    GraphicsController& operator=(const GraphicsController&) = delete;
    ~GraphicsController();

    void setSize(const QSize &size);
    void render(QPainter *p);
    QMenu* itemMenu();
    //QMenu* scneMenu();

public slots:
    // graphics controller
    void update();
    bool eventFilter(QObject *watched, QEvent *event) override;

    // scene
    void addScene(const QString &name = QString("scene"));
    void setAsCurrent();
    void removeScene(int index);
    void removeSelectedScenes();

    // graphcis item
    void addScreenCapture();
    void addPicture();
    void addText();
    void addVideo();
    void incrementSelectedItemIndex(bool forward); // decrement if forward == false
    void removeItem(int index);
    void removeSelectedItems();

private slots:
    void setScale(int per);
    void onItemChanged(QStandardItem *item);

    void selectionChangedFromGraphics();
    void selectionChangedFromItemList();
    void onLayoutChanged() { reorder_ = true; }
    void reorder();

private:
    class SceneItem;

    GraphicsController(QWidget *view, QWidget *scene_tab, QWidget *graphics_tab, QObject *parent = 0);

    // scene
    SceneItem* sceneItem(int row);
    void setCurrentScene(SceneItem *s_item);
    void unsetCurrentScene();

    // graphics item
    QGraphicsItem* graphicsItem(int row);
    void addItem(IGraphicsItem *i_graphics_item, const QString &name);

    class SceneItem
    {
    public:
        SceneItem() :
            scene_(new GraphicsScene), model_(new ItemModel) {}
        ~SceneItem()
        {
            delete scene_;
            delete model_;
        }

        GraphicsScene* scene() { return scene_; }
        ItemModel* model() { return model_; }
    private:
        GraphicsScene *scene_;
        ItemModel *model_;
    };

    static GraphicsController *instance_;

    ItemModel *scene_model_;
    SceneItem *current_;
    ItemView *scene_view_; // doesn't have ownership

    QGraphicsView *graphics_view_; // doesn't have ownership
    QGraphicsScene *null_scene_;

    ItemView *item_view_; // doesn't have ownership
    ItemModel *null_model_;

    PropertyController *property_;

    QSize size_;
    qreal scale_;

    QMenu *scene_menu_;
    QMenu *item_menu_;

    // QAbstractItemModel emits layoutChanged() before removing old items.
    // so, set reorder_ when layoutChanged() is emitted ,and then do work when removeRows() is emitted;
    bool reorder_;
};

#endif // GRAPHICSCONTROLLER_H
