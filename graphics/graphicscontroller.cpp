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

#include "graphicscontroller.h"
#include "ui_view.h"
#include "itemview.h"
#include "itemmodel.h"

// graphics items
#include "item/igraphicsitem.h"
#include "item/screencapture.h"
#include "item/picture.h"
#include "item/video.h"
#include "item/camera.h"
#include "item/text.h"

#include "property/propertycontroller.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QMenu>
#include <QPainter>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QGraphicsView>
#include <QDebug>

#include <set>
#include <algorithm>

GraphicsController* GraphicsController::instance_ = 0;

GraphicsController::GraphicsController(
        QWidget *view, QWidget *graphics_tab, QObject *parent) :
    QObject(parent),
    scene_model_(new ItemModel(this)),
    current_(0),
    scene_view_(0),
    graphics_view_(0),
    null_scene_(new QGraphicsScene(this)),
    item_view_(0),
    null_model_(new ItemModel(this)),
    property_(0),
    size_(QSize()),
    scale_(1),
    scene_view_menu_(0),
    item_view_menu_(0),
    scene_menu_(0),
    reorder_(false)
{
    // init graphics tab
    scene_view_ = new ItemView(graphics_tab);
    scene_view_->setModel(scene_model_);
    scene_view_->setFixedHeight(100);
    item_view_ = new ItemView(graphics_tab);
    item_view_->setModel(null_model_);
    item_view_->setFixedHeight(100);

    QHBoxLayout *views_layout = new QHBoxLayout;
    views_layout->addWidget(scene_view_);
    views_layout->addWidget(item_view_);

    // init property
    QWidget *property = new QWidget;
    property_ = new PropertyController(property, this);

    QVBoxLayout *graphics_layout = new QVBoxLayout;
    graphics_layout->addLayout(views_layout);
    graphics_layout->addWidget(property);
    graphics_tab->setLayout(graphics_layout);

    // init graphics view
    Ui::View *view_ui = new Ui::View;
    view_ui->setupUi(view);
    graphics_view_ = view_ui->graphics_view_;
    graphics_view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphics_view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphics_view_->setBackgroundBrush(QBrush(Qt::black));
    graphics_view_->setScene(null_scene_);
    graphics_view_->setSceneRect(0, 0, size_.width(), size_.height());
    graphics_view_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    graphics_view_->installEventFilter(this);
    connect(view_ui->scale_, SIGNAL(valueChanged(int)), this, SLOT(setScale(int)));

    // scene Action
    QAction *add_scene = new QAction("add", this);
    connect(add_scene, SIGNAL(triggered()), this, SLOT(addScene()));
    QAction *remove_scene = new QAction("remove", this);
    connect(remove_scene, SIGNAL(triggered()), this, SLOT(removeSelectedScenes()));
    QAction *set_current_scene = new QAction("switch", this);
    connect(set_current_scene, SIGNAL(triggered()), this, SLOT(setAsCurrent()));

    scene_view_menu_ = new QMenu;
    scene_view_menu_->addAction(add_scene);
    scene_view_menu_->addAction(remove_scene);
    scene_view_menu_->addAction(set_current_scene);

    scene_view_->setMenu(scene_view_menu_);

    // item Action
    // add menu
    QAction *add_screencapture = new QAction("Screen Capture", this);
    connect(add_screencapture, SIGNAL(triggered()), this, SLOT(addScreenCapture()));
    QAction *add_picture = new QAction("Picture", this);
    connect(add_picture, SIGNAL(triggered()), this, SLOT(addPicture()));
    QAction *add_video = new QAction("Video", this);
    connect(add_video, SIGNAL(triggered()), this, SLOT(addVideo()));
    QAction *add_camera = new QAction("Camera", this);
    connect(add_camera, SIGNAL(triggered()), this, SLOT(addCamera()));
    QAction *add_text = new QAction("Text", this);
    connect(add_text, SIGNAL(triggered()), this, SLOT(addText()));

    QMenu *item_add_menu = new QMenu("Add");
    item_add_menu->addAction(add_screencapture);
    item_add_menu->addAction(add_picture);
    item_add_menu->addAction(add_video);
    item_add_menu->addAction(add_camera);
    item_add_menu->addAction(add_text);

    QAction *item_remove = new QAction("Remove", this);
    connect(item_remove, SIGNAL(triggered()), this, SLOT(removeSelectedItems()));

    QAction *center = new QAction("Center", this);
    connect(center, &QAction::triggered, [&]() {
        if(current_) { current_->scene()->setSelectedItemsPosition(IGraphicsItem::CENTER);}
    });
    QAction *top_left = new QAction("Top Left", this);
    connect(top_left, &QAction::triggered, [&]() {
        if(current_) { current_->scene()->setSelectedItemsPosition(IGraphicsItem::TOP_LEFT);}
    });
    QAction *top_right = new QAction("Top Right", this);
    connect(top_right, &QAction::triggered, [&]() {
        if(current_) { current_->scene()->setSelectedItemsPosition(IGraphicsItem::TOP_RIGHT);}
    });
    QAction *bottom_left = new QAction("Bottom Left", this);
    connect(bottom_left, &QAction::triggered, [&]() {
        if(current_) { current_->scene()->setSelectedItemsPosition(IGraphicsItem::BOTTOM_LEFT);}
    });
    QAction *bottom_right = new QAction("Bottom Right", this);
    connect(bottom_right, &QAction::triggered, [&]() {
        if(current_) { current_->scene()->setSelectedItemsPosition(IGraphicsItem::BOTTOM_RIGHT);}
    });

    QMenu *pos_menu = new QMenu("Position");
    pos_menu->addAction(center);
    pos_menu->addAction(top_left);
    pos_menu->addAction(top_right);
    pos_menu->addAction(bottom_left);
    pos_menu->addAction(bottom_right);

    QAction *default_size = new QAction("Default Size", this);
    connect(default_size, &QAction::triggered, [&]() {
        if(current_) { current_->scene()->setSelectedItemsDefaultSize(); }
    });

    item_view_menu_ = new QMenu;
    item_view_menu_->addMenu(item_add_menu);
    item_view_menu_->addAction(item_remove);
    item_view_menu_->addSeparator();
    item_view_menu_->addMenu(pos_menu);
    item_view_menu_->addAction(default_size);

    item_view_->setMenu(item_view_menu_);
    item_view_->installEventFilter(this);

    scene_menu_ = new QMenu;
    scene_menu_->addMenu(item_add_menu);
}

GraphicsController::~GraphicsController()
{
    scene_view_->setMenu(0);
    delete scene_view_menu_;
    item_view_->setMenu(0);
    delete item_view_menu_;

    unsetCurrentScene();
    Q_ASSERT(current_ == 0);

    // delete all scenes
    for(int i = scene_model_->rowCount() - 1; i >= 0; --i) {
        removeScene(i);
    }
    delete scene_model_;

    delete null_scene_;
    delete null_model_;
    delete property_;

    instance_ = 0;
}

GraphicsController* GraphicsController::createInstance(
        QWidget *view, QWidget *graphics_tab, QObject *parent)
{
    if(!instance_) {
        instance_ = new GraphicsController(view, graphics_tab, parent);
    }
    return instance_;
}

GraphicsController *GraphicsController::instance()
{
    return instance_;
}

void GraphicsController::setSize(const QSize &size)
{
    if(size_ == size) return;
    size_ = size;
    graphics_view_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    graphics_view_->setSceneRect(0, 0, size_.width(), size_.height());
    graphics_view_->scene()->setSceneRect(0, 0, size_.width(), size_.height());
    graphics_view_->adjustSize();
    QWidget *parent = graphics_view_->parentWidget();
    while(parent) {
        parent->adjustSize();
        parent = parent->parentWidget();
    }
    graphics_view_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void GraphicsController::render(QPainter *p)
{
    if(!current_) return;
    graphics_view_->render(p);
}

QMenu *GraphicsController::itemMenu()
{
    return item_view_menu_;
}

void GraphicsController::update()
{
    if(!current_) return;
    current_->scene()->advance();
}

bool GraphicsController::eventFilter(QObject *watched, QEvent *event)
{
    if(!current_) return false;
    if(watched != item_view_ && watched != graphics_view_) return false;

    if(event->type() == QEvent::Wheel) {
        // ctrl + mouseWheel to change the index of the selected item
        QWheelEvent *wheel_event = dynamic_cast<QWheelEvent*>(event);
        if(wheel_event && (wheel_event->modifiers() & Qt::ControlModifier)) {
            int d = wheel_event->delta();
            if(d >= 120) {
                incrementSelectedItemIndex(false);
            } else if(d <= -120) {
                incrementSelectedItemIndex(true);
            }
            return true;
        }
    } else if(event->type() == QEvent::KeyPress) {
        QKeyEvent *key_event = dynamic_cast<QKeyEvent*>(event);
        if(!key_event) return false;

        switch(key_event->key()) {
        case Qt::Key_Escape:
            current_->scene()->clearSelection();
            break;
        case Qt::Key_Delete:
            removeSelectedItems();
            break;
        case Qt::Key_A:
            if(key_event->modifiers() & Qt::ControlModifier) {
                for(auto i : current_->scene()->items()) {
                    i->setSelected(true);
                }
            }
            break;
        default:
            return false;
            break;
        }
        return true;
    }

    return false;
}

void GraphicsController::addScene(const QString &name)
{
    SceneItem *scene_item = new SceneItem;
    scene_item->scene()->setMenu(scene_menu_);

    QStandardItem *s_item = new QStandardItem(name);
    s_item->setData((quint64)scene_item);
    scene_model_->appendRow(s_item);

    setCurrentScene(scene_item);
}

void GraphicsController::removeSelectedScenes()
{
    QItemSelectionModel *sel_model = scene_view_->selectionModel();
    QModelIndexList indices = sel_model->selectedIndexes();

    // indices order is not defined. so, sort it and delete backward so that indices won't get invalid.
    std::set<int> sorted;
    for(auto& i : indices) {
        sorted.insert(i.row());
    }

    // remove selected scenes
    for(auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
        removeScene(*it);
    }
}

void GraphicsController::addScreenCapture()
{
    ScreenCapture *capture =
            new ScreenCapture(QRect(0, 0, 600, 400));
    addItem(capture, "screen capture");
    capture->advance(1);
}

void GraphicsController::addPicture()
{
    Picture *picture = new Picture;
    addItem(picture, "picture");
}

void GraphicsController::addText()
{
    Text *text = new Text;
    addItem(text, "text");
}

void GraphicsController::addVideo()
{
    Video *video = new Video;
    addItem(video, "video");
}

void GraphicsController::addCamera()
{
    Camera *camera = new Camera;
    addItem(camera, "camera");
}

void GraphicsController::incrementSelectedItemIndex(bool forward)
{
    if(!current_) return;

    ItemModel *item_model = current_->model();
    QItemSelectionModel *sel_model = item_view_->selectionModel();
    QModelIndexList indices = sel_model->selectedIndexes();
    if(indices.count() != 1) return; // do work if only one item is selected

    int index = indices.front().row();
    if(!forward && (index == 0)) return;
    if(forward && (index == item_model->rowCount() - 1)) return;

    sel_model->clearSelection();
    QStandardItem *item = item_model->takeItem(index);
    item_model->removeRow(index);

    index = forward ? index + 1 : index - 1;
    item_model->insertRow(index);
    item_model->setItem(index, item);
    sel_model->select(item_model->index(index, 0), QItemSelectionModel::Select);

    reorder();
}

void GraphicsController::removeSelectedItems()
{
    QItemSelectionModel *sel_model = item_view_->selectionModel();
    QModelIndexList indices = sel_model->selectedIndexes();

    // indices order is not defined. so, sort it and delete backward so that indices won't get invalid.
    std::set<int> sorted;
    for(auto& i : indices) {
        sorted.insert(i.row());
    }

    // remove selectet items
    sel_model->clear();
    for(auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
        removeItem(*it);
    }
}

void GraphicsController::setScale(int per)
{
    qreal factor = (qreal)per / 100;
    if(scale_ == factor) return;
    scale_ = factor;
    graphics_view_->setTransform(QTransform::fromScale(scale_, scale_));
    graphics_view_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    graphics_view_->adjustSize();
    QWidget *parent = graphics_view_->parentWidget();
    while(parent) {
        parent->adjustSize();
        parent = parent->parentWidget();
    }
    graphics_view_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void GraphicsController::onItemChanged(QStandardItem *item)
{
    Q_ASSERT(current_->model()->indexFromItem(item) != QModelIndex());
    // toggle visiblity if check state is changed.
    QGraphicsItem *g_item = (QGraphicsItem*)item->data().value<quint64>();
    if(item->checkState() == Qt::Unchecked) {
        g_item->setVisible(false);
    } else {
        g_item->setVisible(true);
    }
}

void GraphicsController::setAsCurrent()
{
    // set the selected scene active
    QItemSelectionModel *sel_model = scene_view_->selectionModel();
    QModelIndexList indices = sel_model->selectedRows();
    if(indices.count() == 1) {
        setCurrentScene(sceneItem(indices.front().row()));
    }
}

void GraphicsController::selectionChangedFromGraphics()
{
    // prevent mutual recursive call
    QItemSelectionModel *sel_model = item_view_->selectionModel();
    disconnect(sel_model, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedFromItemList()));

    // select items in item model side
    QList<QGraphicsItem*> items = current_->scene()->selectedItems();
    sel_model->clear();
    for(auto &item : items) {
        ItemModel *model = current_->model();
        int row = model->rowCount();
        for(int i = 0; i < row; ++i) {
            QGraphicsItem* item2 = graphicsItem(i);
            if(item == item2) {
                sel_model->select(model->index(i, 0), QItemSelectionModel::Select);
                break;
            }
        }
    }

    // update property view
    if(items.count() == 1) {
        property_->setItem(items.front());
    } else {
        property_->setItem(0);
    }

    connect(sel_model, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedFromItemList()));
}

void GraphicsController::selectionChangedFromItemList()
{
    // for avoiding mutual recursive call
    bool old_state = current_->scene()->blockSignals(true);

    QItemSelectionModel *sel_model = item_view_->selectionModel();
    QModelIndexList indices = sel_model->selectedRows();

    // select items in graphics side
    current_->scene()->clearSelection();
    for(auto &i : indices) {
        QGraphicsItem *item = graphicsItem(i.row());
        item->setSelected(true);
    }

    // update property view
    if(indices.count() == 1) {
        QGraphicsItem *item = graphicsItem(indices.front().row());
        property_->setItem(item);
    } else {
        property_->setItem(0);
    }

    current_->scene()->blockSignals(old_state);
}

void GraphicsController::reorder()
{
    // change zvalue to reorder graphicsitems the same order as itemview
    if(!reorder_) return;
    reorder_ = false;

    int row = current_->model()->rowCount();
    for(int i = 0; i < row; ++i) {
        graphicsItem(row - i - 1)->setZValue(i);
    }
}

GraphicsController::SceneItem *GraphicsController::sceneItem(int row)
{
    return (SceneItem*)scene_model_->item(row)->data().value<quint64>();
}

void GraphicsController::setCurrentScene(GraphicsController::SceneItem *s_item)
{
    unsetCurrentScene();

    QGraphicsScene *scene = s_item->scene();
    scene->setSceneRect(0, 0, size_.width(), size_.height());
    graphics_view_->setScene(scene);
    connect(scene, SIGNAL(selectionChanged()), this, SLOT(selectionChangedFromGraphics()));

    ItemModel *item_model = s_item->model();
    item_view_->setModel(item_model);
    QItemSelectionModel *sel_model = item_view_->selectionModel();
    connect(sel_model, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedFromItemList()));
    connect(item_model, SIGNAL(layoutChanged()), this, SLOT(onLayoutChanged()));
    connect(item_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(reorder()));
    connect(item_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));

    item_view_->setMenu(item_view_menu_);

    current_ = s_item;
}

void GraphicsController::removeScene(int index)
{
    if(index < scene_model_->rowCount()) {
        SceneItem *s_item = sceneItem(index);
        if(s_item == current_) {
            unsetCurrentScene();
        }
        delete s_item;

        scene_model_->removeRow(index);
    }
}

void GraphicsController::unsetCurrentScene()
{
    if(!current_) return;
    QGraphicsScene *scene = current_->scene();
    disconnect(scene, SIGNAL(selectionChanged()), this, SLOT(selectionChangedFromGraphics()));
    graphics_view_->setScene(null_scene_);

    ItemModel *item_model = current_->model();
    QItemSelectionModel *sel_model = item_view_->selectionModel();
    disconnect(sel_model, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedFromItemList()));
    disconnect(item_model, SIGNAL(layoutChanged()), this, SLOT(onLayoutChanged()));
    disconnect(item_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(reorder()));
    disconnect(item_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));
    item_view_->setModel(null_model_);

    property_->setItem(0);

    item_view_->setMenu(0);

    current_ = 0;
}

QGraphicsItem *GraphicsController::graphicsItem(int row)
{
    return (QGraphicsItem*)current_->model()->item(row)->data().value<quint64>();
}

void GraphicsController::addItem(IGraphicsItem *i_graphics_item, const QString &name)
{
    QGraphicsItem *graphics_item = i_graphics_item->item();
    current_->scene()->addItem(graphics_item);

    // If items are selected, a new item is inserted before the top selected item.
    // Otherwise, in the bottom of the model.
    QItemSelectionModel *sel_model = item_view_->selectionModel();
    QModelIndexList list = sel_model->selectedRows();
    QStandardItem *item = new QStandardItem(name);
    item->setCheckable(true);
    item->setCheckState(Qt::Checked);
    item->setData((quint64)(QGraphicsItem*)graphics_item);
    if(list.isEmpty()) {
        current_->model()->appendRow(item);
    } else {
        int min = list.front().row();
        for(auto &i : list) {
            min = std::min(min, i.row());
        }
        current_->model()->insertRow(min, {item});
    }

    // sort scene items in the same order as item model
    reorder_ = true;
    reorder();

    // select added item
    current_->scene()->clearSelection();
    i_graphics_item->updatePos();
    graphics_item->setSelected(true);
}

void GraphicsController::removeItem(int index)
{
    QGraphicsItem *item = graphicsItem(index);
    current_->scene()->removeItem(item);
    delete item;

    current_->model()->removeRow(index);
}
