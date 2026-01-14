#include "customrelationalmodel.h"
#include <QBrush>

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

QVariant CustomRelationalModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    //Si la valeur est changé on change le background à jaune
    if(role == Qt::BackgroundRole)
    {
        if(isDirty(index))
            return QBrush(Qt::yellow);

    }

    // Pour les autres rôles on appelle la méthode mère
    return QSqlRelationalTableModel::data(index, role);
}
