#include "componenttablemodel.h"
#include <QBrush>
#include <QSqlQuery>

ComponentTableModel::ComponentTableModel(QObject *parent, const QSqlDatabase &db)
    : QSqlRelationalTableModel{parent,db}
{

}

QVariant ComponentTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    //Si la valeur est changé on change le background à jaune
    if(role == Qt::BackgroundRole)
    {
        if(index.column()<columnCount()-1) // les colonnes ajoutées ne sont pas gérées par isDirty
        {
            if(isDirty(index))
                return QBrush(Qt::yellow);
        }
    }

    // Si la colonne demandée est la colonne calculée (dernière colonne)
    if (index.column() == columnCount() - 1) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {

            return m_stock[index.row()];
        }
    }

    // Pour les autres colonnes, utiliser le comportement par défaut
    return QSqlRelationalTableModel::data(index, role);
}

bool ComponentTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //On doit recalculer le nombre de composants restant
    //Cela se fait si on touche à la colonne 21
    if(index.column()==21)
    {
        if (value.canConvert<int>())
        {
            int composant_id = QSqlTableModel::data(this->index(index.row(), 0), Qt::EditRole).toInt();
            m_stock[index.row()] = value.toInt()-calculateUsedQuantity(composant_id);
        }
    }

    return QSqlRelationalTableModel::setData(index,value,role);
}

int ComponentTableModel::columnCount(const QModelIndex &parent) const
{
    //ajout d'une colonne en plus pour la quantité restante
    return QSqlRelationalTableModel::columnCount()+1;
}

bool ComponentTableModel::select()
{
    bool success = QSqlRelationalTableModel::select();
    //On prépare la colonne supplémentaire
    m_stock.resize(static_cast<qsizetype>(columnCount()));

    return success;

}

Qt::ItemFlags ComponentTableModel::flags(const QModelIndex &index) const
{
    // Récupère les flags par défaut
    Qt::ItemFlags defaultFlags = QSqlTableModel::flags(index);

    // Désactive l'édition pour la colonne 0 et la dernière (custom)
    if (index.column() == 0 || index.column() == columnCount()-1)
    {
        return defaultFlags & ~Qt::ItemIsEditable;
    }

    return defaultFlags;
}

int ComponentTableModel::calculateUsedQuantity(int composant_id)
{
    //Création d'une query
    QSqlQuery usedQuery(database());
    usedQuery.prepare("SELECT COUNT(*) FROM DCL WHERE Composant_ID = ?");
    usedQuery.addBindValue(composant_id);
    usedQuery.exec();
    usedQuery.next();
    return usedQuery.value(0).toInt();

}

