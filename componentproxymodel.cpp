#include "componentproxymodel.h"
#include "componenttablemodel.h"

ComponentProxyModel::ComponentProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

void ComponentProxyModel::setFilters(QStringList filters)
{
    beginFilterChange();
    m_filters = filters;
    endFilterChange();
}

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

bool ComponentProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_filters.isEmpty()) return true;  // Aucun filtre = tout accepter

    for (int i = 0; i < m_filters.size(); ++i) {
        if (m_filters[i].isEmpty()) continue;  // Ignorer les filtres vides

        QModelIndex index = sourceModel()->index(source_row, i, source_parent);
        QString data = sourceModel()->data(index).toString();

        if (!data.contains(m_filters[i], Qt::CaseInsensitive)) {
            return false;  // La ligne ne correspond pas à ce filtre
        }
    }
    return true;  // La ligne correspond à tous les filtres
}
