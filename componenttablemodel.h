#ifndef COMPONENTTABLEMODEL_H
#define COMPONENTTABLEMODEL_H

#include <QSqlRelationalTableModel>
#include <QList>

class ComponentTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit ComponentTableModel(QObject *parent = nullptr,const QSqlDatabase &db = QSqlDatabase());

    //Réécriture
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    bool select() override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    // QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    //Fonctions liées à la colonne ajoutée
    int calculateUsedQuantity(int composant_id);
    int calculateDBStock(int composant_id);


private:
    QList<int> m_stock;
};

#endif // COMPONENTTABLEMODEL_H
