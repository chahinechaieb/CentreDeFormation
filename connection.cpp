#include "connection.h"
#include <QSqlError>
#include <QDebug>

Connection::Connection()
{
}

bool Connection::createconnect()
{
    bool test = false;

    //ODBC
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName("prj2a");
    db.setUserName("chahine");
    db.setPassword("1234");

    if (db.open()) {
        qDebug() << "Connexion à la base réussie";
        test = true;
    } else {
        qDebug() << "Échec de la connexion :" << db.lastError().text();
    }

    return test;
}
