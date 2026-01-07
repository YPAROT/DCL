#ifndef TABLEMODELFACTORY_H
#define TABLEMODELFACTORY_H

#include <QObject>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QMap>
#include <QList>
#include <QSqlDatabase>

class TableModelFactory : public QObject
{
    Q_OBJECT
public:
    explicit TableModelFactory(QObject *parent = nullptr);
    ~TableModelFactory();

    // Crée un modèle pour une table donnée avec la base de données spécifiée
    QSqlRelationalTableModel *createModel(const QString &tableName, const QSqlDatabase &db);

    // Associe une vue à un modèle existant
    void attachView(const QString &tableName, QTableView *view);

    // Supprime tous les modèles et détache toutes les vues
    void clearAllModels();

    // Supprime un modèle spécifique et détache ses vues
    void clearModel(const QString &tableName);

    //effectue un select sur tous les modèles
    bool selectOnAllModels();

    //effectue un select sur un modèle
    bool selectOnModel(const QString &tableName);

    // Accède à un modèle existant
    QSqlRelationalTableModel *getModel(const QString &tableName) const;

private:
    using ModelMap = QMap<QString, QSqlRelationalTableModel*>;
    using ViewMap = QMap<QString, QList<QTableView*>>;
    ModelMap m_models;
    ViewMap m_views;
};

#endif // TABLEMODELFACTORY_H
