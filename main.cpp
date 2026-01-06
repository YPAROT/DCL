#include "mainwindow.h"

#include <QApplication>
#include <QSqlDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Initialisation de la connexion à la base de donnée SQLITE
    //Utilisation de la connexion par défaut
    QSqlDatabase::addDatabase("QSQLITE");

    //Lancement de la fenêtre principale
    MainWindow w;
    w.show();

    return a.exec();
}
