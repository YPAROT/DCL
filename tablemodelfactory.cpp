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

    //Génération de la clef
    QPair<QString,QString> key = QPair<QString,QString>(tableName,vue_trouvee->objectName());

    //On prévient les delegates
    auto delegates = m_delegates[key];
    for(auto delegate : delegates)
    {
        delegate->setProxy(proxy);
    }

    //Si il y avait un proxy on l'efface
    if(m_proxies.contains(key))
    {
        delete m_proxies[key];
    }

    m_proxies[key]=proxy;


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

    //Génération de la clef
    QPair<QString,QString> key = QPair<QString,QString>(tableName,vue_trouvee->objectName());

    //La vue ne possède pas de proxy
    if (!m_proxies.contains(key)) return;

    //On empêche la vue de se rafraîchir
    vue_trouvee->setUpdatesEnabled(false);

    //On enlève le proxy
    vue_trouvee->setModel(m_models[tableName]);
    //On prévient les delegates
    auto delegates = m_delegates[key];
    for(auto delegate : delegates)
    {
         delegate->setProxy(nullptr);
    }


    //on peut effacer le proxy
    delete m_proxies[key];
    m_proxies.remove(key);

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

            //Génération de la clef
            QPair<QString,QString> key = QPair<QString,QString>(it.key(),view->objectName());

            //QString view_name = view->objectName();
            if (m_proxies.contains(key))
            {
                view->setModel(m_proxies[key]->sourceModel());

                //on prévient les delegates
                auto delegates = m_delegates[key];
                for(auto delegate : delegates)
                {
                    delegate->setProxy(nullptr);
                }

                delete m_proxies[key];
                m_proxies.remove(key); //On pourrait faire un clear de la liste à la fin mais je préfère faire au fur et à mesure
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
            //On enlève les proxies
            detachSortFilterProxyModel(tableName,view->objectName());
            //Génération de la clef
            QPair<QString,QString> key = QPair<QString,QString>(tableName,view->objectName());
            //on efface les delegates
            auto delegates = m_delegates[key];
            for(auto delegate : delegates)
            {
                delete delegate;
            }
            m_delegates.remove(key);
            view->setModel(nullptr);
        }
        m_views.remove(tableName);
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

void TableModelFactory::setDelegate(const QString &tableName, const QString &viewName, int column, ProxyDelegate *delegate)
{
    if (!m_models.contains(tableName)) {
        qWarning() << "Modèle non trouvé pour la table" << tableName;
        return;
    }

    //Si le modèle n'a pas de vues, le delegate sert à rien on ne le met pas
    if(!m_views.contains(tableName))
    {
        qWarning() << "Pas de vues associées à cette table" << tableName;
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

    //Génération de la clef
    QPair<QString,QString> key = QPair<QString,QString>(tableName,vue_trouvee->objectName());

    delegate->setProxy(m_proxies[key]);


    //ajout du delegate
    vue_trouvee->setItemDelegateForColumn(column, delegate);


    // for (QTableView *view : m_views[tableName])
    // {
    //     QPair<QString,QString> key = QPair<QString,QString>(tableName,view->objectName());
    //     // Stocke le délégué pour la colonne spécifiée
    //     m_delegates[key][column] = delegate;
    //     if(m_proxies.contains(key))
    //         delegate->setProxy(m_proxies[key]);
    //     view->setItemDelegateForColumn(column, delegate);
    // }

}

void TableModelFactory::setDelegateForAllViews(const QString &tableName, int column, ProxyDelegate *delegate)
{
    if (!m_models.contains(tableName)) {
        qWarning() << "Modèle non trouvé pour la table" << tableName;
        return;
    }

    //Si le modèle n'a pas de vues, le delegate sert à rien on ne le met pas
    if(!m_views.contains(tableName))
    {
        qWarning() << "Pas de vues associées à cette table" << tableName;
        return;
    }

    for (QTableView *view : m_views[tableName])
    {
        ProxyDelegate* delegate_copy = new ProxyDelegate(delegate);
        QPair<QString,QString> key = QPair<QString,QString>(tableName,view->objectName());
        // Stocke le délégué pour la colonne spécifiée
        m_delegates[key][column] = delegate_copy;
        if(m_proxies.contains(key))
            delegate_copy->setProxy(m_proxies[key]);
        view->setItemDelegateForColumn(column, delegate_copy);
    }

    delete delegate;
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
