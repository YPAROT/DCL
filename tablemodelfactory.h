#ifndef TABLEMODELFACTORY_H
#define TABLEMODELFACTORY_H

#include <QObject>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QMap>
#include <QList>
#include <QSqlDatabase>
#include <QSortFilterProxyModel>
#include "proxydelegate.h"


//Définition de la classe TableModelFactory permettant de créer les modèles à partir de la DB,
//D'y associer des vues, des délégués, des proxys etc..
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

    //Attache un proxy
    void attachSortFilterProxyModel(const QString &tableName,const QString &viewName, QSortFilterProxyModel *proxy);

    //detache un proxy
    void detachSortFilterProxyModel(const QString &tableName,const QString &viewName);

    //detache tous les proxy
    void detachAllSortFilterProxyModel();

    // Supprime tous les modèles et détache toutes les vues
    void clearAllModels();

    // Supprime un modèle spécifique et détache ses vues
    void clearModel(const QString &tableName);

    //effectue un select sur tous les modèles
    bool selectOnAllModels();

    //effectue un select sur un modèle
    bool selectOnModel(const QString &tableName);

    // Définit un délégué personnalisé pour une colonne spécifique d'un modèle
    void setDelegate(const QString &tableName, const QString &viewName, int column, ProxyDelegate *delegate);

    void setDelegateForAllViews(const QString &tableName, int column, ProxyDelegate *delegate);

    // Définit une relation pour une colonne d'un modèle
    void setRelation(const QString &tableName, int column, const QSqlRelation &relation);

    // Accède à un modèle existant
    QSqlRelationalTableModel *getModel(const QString &tableName) const;

private:
    QString retrieveTableNameFromView(QTableView* view);

private:
    using ModelMap = QMap<QString, QSqlRelationalTableModel*>; //La clef est le nom de la table
    using ViewMap = QMap<QString, QList<QTableView*>>; //La clef est le nom de la table
    using DelegateMap = QMap<QPair<QString,QString>,QMap<int,ProxyDelegate*>>; // la clef est tablename,viewname. Le int design la colonne associé au delegate
    using ProxyMap = QMap<QPair<QString,QString>,QSortFilterProxyModel*>; // la clef est tablename,viewname.

    ModelMap m_models;
    ViewMap m_views;
    DelegateMap m_delegates;
    ProxyMap m_proxies;
};

#endif // TABLEMODELFACTORY_H
