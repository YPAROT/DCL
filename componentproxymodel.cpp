#include "componentproxymodel.h"
#include "componenttablemodel.h"

ComponentProxyModel::ComponentProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

bool ComponentProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    // Tri personnalisé pour la colonne calculée (dernière colonne)
    if (left.column() == sourceModel()->columnCount() - 1) {
        const ComponentTableModel *model = qobject_cast<const ComponentTableModel*>(sourceModel());
        if (model) {
            // int leftStock = model->getStockValue(left.row());
            // int rightStock = model->getStockValue(right.row());
            int leftStock = model->data(left,Qt::DisplayRole).toInt();
            int rightStock = model->data(right,Qt::DisplayRole).toInt();
            return leftStock < rightStock;
        }
    }
    // Pour les autres colonnes, utilise le tri par défaut
    return QSortFilterProxyModel::lessThan(left, right);
}
