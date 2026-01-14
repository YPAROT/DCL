#ifndef COMPONENTTABLEMODEL_H
#define COMPONENTTABLEMODEL_H

#include <QSqlRelationalTableModel>
#include <QList>
#include <QSqlRecord>

class ComponentTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit ComponentTableModel(QObject *parent = nullptr,const QSqlDatabase &db = QSqlDatabase());

    //Réécriture
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    bool select() override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    //Fonctions liées à la colonne ajoutée
    int calculateUsedQuantity(int composant_id);
    int calculateDBStock(int composant_id);

    //copier/coller
    void copySelectedRows(const QModelIndexList &selectedRows);
    void pasteCopiedRows();


private:
    QList<int> m_stock;
    QList<QSqlRecord> m_copiedRecords; // Pour stocker les lignes copiées
};

#endif // COMPONENTTABLEMODEL_H
