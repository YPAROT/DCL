#include "tablemodelfactory.h"
#include "componenttablemodel.h"

TableModelFactory::TableModelFactory(QObject *parent)
    : QObject(parent)
{
}

TableModelFactory::~TableModelFactory()
{
    // Nettoie tous les modèles et vues avant la destruction de la factory
    //clearAllModels();
    //pas besoin car tout hérite de qObject et il est des cas où l'objet view est delete sans que le pointeur soit nullptr
}

QSqlRelationalTableModel *TableModelFactory::createModel(const QString &tableName, const QSqlDatabase &db)
{
    // Si le modèle existe déjà, on le supprime pour en recréer un nouveau
    if (m_models.contains(tableName)) {
        delete m_models[tableName];
        m_models.remove(tableName);
    }

    // Instancie le modèle avec la factory comme parent et la base de données spécifiée
    //selon la table un modèle est choisit
    QSqlRelationalTableModel* model = nullptr;
    if (tableName == "Composants")
    {
        model = new ComponentTableModel(this);
    }else
    {
        model = new QSqlRelationalTableModel(this);
    }


    //QSqlRelationalTableModel *model = new QSqlRelationalTableModel(this, db);
    model->setTable(tableName);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    // Stocke le modèle
    m_models[tableName] = model;
    return model;
}

void TableModelFactory::attachView(const QString &tableName, QTableView *view)
{
    if (!m_models.contains(tableName)) {
        qWarning() << "Modèle non trouvé pour la table" << tableName;
        return;
    }

    // Associe la vue au modèle
    m_views[tableName].append(view);
    view->setModel(m_models[tableName]);
}


void TableModelFactory::attachSortFilterProxyModel(const QString &tableName, const QString &viewName, QSortFilterProxyModel *proxy)
{
    //existance du modèle
    if (!m_models.contains(tableName)) {
        qWarning() << "Modèle non trouvé pour la table" << tableName;
        return;
    }

    //existance de la vue
    QTableView* vue_trouvee = nullptr;
    for(QTableView* current_obj : m_views[tableName])
    {
        if(current_obj->objectName() == viewName)
        {
            vue_trouvee = current_obj;
            break;
        }
    }

    //Mise en place du proxy
    if(proxy==nullptr || vue_trouvee==nullptr)
    {
        return;
    }

    //On empêche la vue de se rafraîchir
    vue_trouvee->setUpdatesEnabled(false);

    proxy->setSourceModel(m_models[tableName]);
    vue_trouvee->setModel(proxy);
    m_proxies[viewName]=proxy;

    //On prévient les delegates
    auto delegate_map = m_delegates[tableName];
    for(ProxyDelegate* delegate : delegate_map)
    {
        delegate->setProxy(proxy);
    }

    //La vue peut recommencer à se rafraîchir
    vue_trouvee->setUpdatesEnabled(true);
    vue_trouvee->update();

}

void TableModelFactory::detachSortFilterProxyModel(const QString &tableName, const QString &viewName)
{
    //existance du modèle
    if (!m_models.contains(tableName)) {
        qWarning() << "Modèle non trouvé pour la table" << tableName;
        return;
    }

    //existance de la vue
    QTableView* vue_trouvee = nullptr;
    for(QTableView* current_obj : m_views[tableName])
    {
        if(current_obj->objectName() == viewName)
        {
            vue_trouvee = current_obj;
            break;
        }
    }

    //Pas de vue trouvée
    if(vue_trouvee == nullptr) return;

    //La vue ne possède pas de proxy
    if (!m_proxies.contains(viewName)) return;

    //On empêche la vue de se rafraîchir
    vue_trouvee->setUpdatesEnabled(false);

    //On enlève le proxy
    vue_trouvee->setModel(m_models[tableName]);
    //On prévient les delegates
    auto delegate_map = m_delegates[tableName];
    for(ProxyDelegate* delegate : delegate_map)
    {
        delegate->setProxy(nullptr);
    }
    //on peut effacer le proxy
    delete m_proxies[viewName];
    m_proxies.remove(viewName);

    //La vue peut recommencer à se rafraîchir
    vue_trouvee->setUpdatesEnabled(true);
    vue_trouvee->update();

}

void TableModelFactory::detachAllSortFilterProxyModel()
{

    //On bloucle sur les table views et on détache les proxy un à un
    for (auto it = m_views.begin();it!=m_views.end();it++)
    {
        for(QTableView* view : it.value())
        {
            //On empêche la vue de se rafraîchir
            view->setUpdatesEnabled(false);

            QString view_name = view->objectName();
            if (m_proxies.contains(view_name))
            {
                view->setModel(m_proxies[view_name]->sourceModel());

                //on prévient les delegates
                auto delegate_map = m_delegates[retrieveTableNameFromView(view)];
                for(ProxyDelegate* delegate : std::as_const(delegate_map))
                {
                    delegate->setProxy(nullptr); // On fait le job plusieurs fois. Autre soucis les delegates sont uniques par modèles et non par vues.... Revoir philo des delegates!!! Un doit être créé par vue.
                }

                delete m_proxies[view_name];
                m_proxies.remove(view_name); //On pourrait faire un clear de la liste à la fin mais je préfère faire au fur et à mesure
            }

            //La vue peut recommencer à se rafraîchir
            view->setUpdatesEnabled(true);
            view->update();
        }
    }
}

void TableModelFactory::clearAllModels()
{
    //détache les tous les proxy
    detachAllSortFilterProxyModel();

    // Détache toutes les vues de leurs modèles
    for (auto it = m_views.begin(); it != m_views.end(); ++it) {
        for (QTableView *view : it.value()) {
            qDebug()<<"Détache la vue "<<view<<" du modèle";
            view->setModel(nullptr);
        }
    }

    // Supprime tous les délégués
    for (auto &delegateMap : m_delegates) {
        qDeleteAll(delegateMap);
    }
    m_delegates.clear();

    // Supprime tous les modèles
    qDeleteAll(m_models);
    m_models.clear();
    m_views.clear();
}

void TableModelFactory::clearModel(const QString &tableName)
{
    if (m_views.contains(tableName)) {
        for (QTableView *view : m_views[tableName]) {
            detachSortFilterProxyModel(tableName,view->objectName());
            view->setModel(nullptr);
        }
        m_views.remove(tableName);
    }

    if (m_delegates.contains(tableName)) {
        qDeleteAll(m_delegates[tableName]);
        m_delegates.remove(tableName);
    }

    if (m_models.contains(tableName)) {
        delete m_models[tableName];
        m_models.remove(tableName);
    }
}

bool TableModelFactory::selectOnAllModels()
{
    bool success = true;
    for (auto it = m_models.begin(); it != m_models.end(); ++it)
    {
        success = success && it.value()->select();
    }
    return success;
}

bool TableModelFactory::selectOnModel(const QString &tableName)
{
    if(m_models.contains(tableName))
    {
        return m_models[tableName]->select();
    }
    return false;
}

void TableModelFactory::setDelegate(const QString &tableName, int column, ProxyDelegate *delegate)
{
    if (!m_models.contains(tableName)) {
        qWarning() << "Modèle non trouvé pour la table" << tableName;
        return;
    }

    // Stocke le délégué pour la colonne spécifiée
    m_delegates[tableName][column] = delegate;

    // Applique le délégué à toutes les vues associées au modèle
    if (m_views.contains(tableName)) {
        for (QTableView *view : m_views[tableName]) {
            //Si il y a un proxy on l'applique au délégué
            if(m_proxies.contains(view->objectName()))
                delegate->setProxy(m_proxies[view->objectName()]);
            view->setItemDelegateForColumn(column, delegate);
        }
    }
}

void TableModelFactory::setRelation(const QString &tableName, int column, const QSqlRelation &relation)
{
    if (!m_models.contains(tableName)) {
        qWarning() << "Modèle non trouvé pour la table" << tableName;
        return;
    }

    // Définit la relation pour la colonne spécifiée
    m_models[tableName]->setRelation(column, relation);
}

QSqlRelationalTableModel *TableModelFactory::getModel(const QString &tableName) const
{
    return m_models.value(tableName, nullptr);
}

QString TableModelFactory::retrieveTableNameFromView(QTableView *view)
{
    for (auto it = m_views.constBegin(); it != m_views.constEnd(); ++it) {
        const QString &currentKey = it.key();  // Clé QString actuelle
        const QList<QTableView*> &objectList = it.value();

        // Parcourt la liste
        for (QTableView *obj : std::as_const(objectList)) {
            if (obj == view) {
                return currentKey;  // Retourne la clé QString trouvée
            }
        }
    }
    return QString();  // Retourne une QString vide si non trouvé
}
