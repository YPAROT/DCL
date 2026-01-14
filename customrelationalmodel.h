#ifndef CUSTOMRELATIONALMODEL_H
#define CUSTOMRELATIONALMODEL_H

#include <QSqlRelationalTableModel>

class CustomRelationalModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    explicit CustomRelationalModel(QObject *parent = nullptr);

    //override
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

};

#endif // CUSTOMRELATIONALMODEL_H
