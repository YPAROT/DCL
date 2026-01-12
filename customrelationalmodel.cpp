#include "customrelationalmodel.h"

CustomRelationalModel::CustomRelationalModel(QObject *parent)
    : QSqlRelationalTableModel{parent}
{}

Qt::ItemFlags CustomRelationalModel::flags(const QModelIndex &index) const
{
    // Récupère les flags par défaut
    Qt::ItemFlags defaultFlags = QSqlTableModel::flags(index);

    // Désactive l'édition pour la colonne 0 (ID)
    if (index.column() == 0)
    {
        return defaultFlags & ~Qt::ItemIsEditable;
    }

    return defaultFlags;
}
