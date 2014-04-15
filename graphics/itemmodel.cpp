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

#include "itemmodel.h"

#include <QDebug>

ItemModel::ItemModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

Qt::ItemFlags ItemModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;
    }

    return QAbstractItemModel::flags(index)
            | Qt::ItemIsUserCheckable
            | Qt::ItemIsEditable
            | Qt::ItemIsDragEnabled;
}

Qt::DropActions ItemModel::supportedDropActions() const
{
    return Qt::MoveAction;
}
