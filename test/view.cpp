#include "view.h"

view::view(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant view::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

int view::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int view::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant view::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
