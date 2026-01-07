#include "tablemodelfactory.h"
#include "componenttablemodel.h"

TableModelFactory::TableModelFactory(QObject *parent)
    : QObject(parent)
{
}

TableModelFactory::~TableModelFactory()
{
    // Nettoie tous les modèles et vues avant la destruction de la factory
    clearAllModels();
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

void TableModelFactory::clearAllModels()
{
    // Détache toutes les vues de leurs modèles
    for (auto it = m_views.begin(); it != m_views.end(); ++it) {
        for (QTableView *view : it.value()) {
            view->setModel(nullptr);
        }
    }

    // Supprime tous les modèles
    qDeleteAll(m_models);
    m_models.clear();
    m_views.clear();
}

void TableModelFactory::clearModel(const QString &tableName)
{
    if (m_views.contains(tableName)) {
        for (QTableView *view : m_views[tableName]) {
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

QSqlRelationalTableModel *TableModelFactory::getModel(const QString &tableName) const
{
    return m_models.value(tableName, nullptr);
}
