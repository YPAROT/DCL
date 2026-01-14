#include "componenttablemodel.h"
#include <QBrush>
#include <QSqlQuery>
#include <QSqlError>

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
    if (!index.isValid()) return false;

    // Gestion de la colonne Qté_commandée (colonne 21)
    if (index.column() == 21) {
        if (value.canConvert<int>()) {
            // 1. Enregistre la valeur dans la base via la classe parente
            bool success = QSqlRelationalTableModel::setData(index, value, role);
            if (success) {
                // 2. Met à jour m_stock pour la colonne calculée (Qté_restante)
                int composant_id = QSqlTableModel::data(this->index(index.row(), 0), Qt::EditRole).toInt();
                m_stock[index.row()] = value.toInt() - calculateUsedQuantity(composant_id);
                // 3. Rafraîchit la colonne Qté_restante (colonne 21)
                QModelIndex stockIndex = this->index(index.row(), 21);
                emit dataChanged(stockIndex, stockIndex);
            }
            return success;
        }
        return false;
    }

    // Pour les autres colonnes, utilise le comportement par défaut
    return QSqlRelationalTableModel::setData(index, value, role);
}

int ComponentTableModel::columnCount(const QModelIndex &parent) const
{
    //ajout d'une colonne en plus pour la quantité restante
    return QSqlRelationalTableModel::columnCount()+1;
}

bool ComponentTableModel::select()
{

    qDebug() << "Select est appelé sur" << this;
    bool success = QSqlRelationalTableModel::select();
    m_stock.clear();
    if (success && rowCount() > 0) {  // Vérifie que rowCount() > 0
        for(int i = 0; i < rowCount(); i++) {
            int composant_id = QSqlTableModel::data(this->index(i, 0), Qt::EditRole).toInt();
            m_stock.append(calculateDBStock(composant_id));
        }
        // Émet dataChanged() uniquement si rowCount() > 0
        QModelIndex topLeft = this->index(0, 21);
        QModelIndex bottomRight = this->index(rowCount() - 1, columnCount() - 1);
        if (topLeft.isValid() && bottomRight.isValid()) {
            emit dataChanged(topLeft, bottomRight);
        }
    }
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

QVariant ComponentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal) {
        static const QStringList headers = {
            "ID", "Famille", "Sous-groupe", "Description", "Part Number", "Valeur", "Tolérance",
            "Max. rating", "Boîtier", "Fabricant", "Spécification générique", "Spécification détaillée",
            "Niveau de Qualité", "Qualifié Vol", "Fournisseur", "ITAR / EAS", "Date Code",
            "Relifed", "Masse", "Statut", "Date de Reception", "Qté Commandée","Lieu de Stockage",
            "Qté Restante"
        };
        if (section < headers.size()) return headers[section];
    }
    return QVariant();
}

bool ComponentTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool success = QSqlRelationalTableModel::insertRows(row,count,parent);

    if(success)
    {
        for (int i = 0; i < count; ++i)
        {
            m_stock.append(0);
        }
    }

    return success;
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

int ComponentTableModel::calculateDBStock(int composant_id)
{
    //Création d'une query
    QSqlQuery orderedQuery(database());
    orderedQuery.prepare("SELECT Quantity_ordered FROM Composants WHERE ID = ?");
    orderedQuery.addBindValue(composant_id);
    orderedQuery.exec();
    orderedQuery.next();
    return orderedQuery.value(0).toInt()-calculateUsedQuantity(composant_id);

}

void ComponentTableModel::copySelectedRows(const QModelIndexList &selectedRows)
{
    m_copiedRecords.clear(); // Efface les enregistrements précédemment copiés

    for (const QModelIndex &index : selectedRows) {
        int row = index.row();
        // Récupère le QSqlRecord de la ligne (sans la colonne supplémentaire)
        QSqlRecord record = this->record(row);
        m_copiedRecords.append(record);
    }
}


void ComponentTableModel::pasteCopiedRows()
{
    if (m_copiedRecords.isEmpty()) {
        return; // Rien à coller
    }

    // Insère chaque enregistrement copié à la fin du modèle
    for (const QSqlRecord &record : std::as_const(m_copiedRecords)) {
        // Insère le record à la fin (-1)
        if (!insertRecord(-1, record)) {
            qWarning() << "Échec de l'insertion de la ligne:" << lastError().text();
            continue;
        }
    }

}
