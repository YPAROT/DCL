#include "customtableview.h"

CustomTableView::CustomTableView(QWidget *parent) :QTableView(parent){}

void CustomTableView::setModel(QAbstractItemModel *model)
{
    QAbstractItemModel *oldModel = this->model();
    QTableView::setModel(model); // Appel de la méthode originale
    if (oldModel != model) {
        emit modelChanged(model); // Émet un signal personnalisé
    }
}
