#ifndef TABLEMODELFACTORY_H
#define TABLEMODELFACTORY_H

#include <QObject>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QMap>
#include <QList>
#include <QSqlDatabase>
#include <QStyledItemDelegate>

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

    // Définit un délégué personnalisé pour une colonne spécifique d'un modèle
    void setDelegate(const QString &tableName, int column, QStyledItemDelegate *delegate);

    // Définit une relation pour une colonne d'un modèle
    void setRelation(const QString &tableName, int column, const QSqlRelation &relation);

    // Accède à un modèle existant
    QSqlRelationalTableModel *getModel(const QString &tableName) const;

private:
    using ModelMap = QMap<QString, QSqlRelationalTableModel*>;
    using ViewMap = QMap<QString, QList<QTableView*>>;
    using DelegateMap = QMap<QString, QMap<int, QStyledItemDelegate *>>;

    ModelMap m_models;
    ViewMap m_views;
    DelegateMap m_delegates;
};

#endif // TABLEMODELFACTORY_H
