#ifndef CUSTOMRELATIONALMODEL_H
#define CUSTOMRELATIONALMODEL_H

#include <QSqlRelationalTableModel>

class CustomRelationalModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    explicit CustomRelationalModel(QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex& index) const override;
};

#endif // CUSTOMRELATIONALMODEL_H
