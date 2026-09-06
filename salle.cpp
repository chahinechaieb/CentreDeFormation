#include "salle.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QObject>
#include <QImage>
#include <QPainter>
#include <QPrinter>
#include <QTextDocument>
#include "qrcodegen.hpp" // Bibliothèque pour le QR Code

// Constructeur par défaut
Salle::Salle()
{
    idSalle = 0;
    nomSalle = "";
    capacite = 0;
    typeSalle = "";
}

// Constructeur paramétré
Salle::Salle(int id, QString nom, int cap, QString type)
{
    this->idSalle = id;
    this->nomSalle = nom;
    this->capacite = cap;
    this->typeSalle = type;
}

// ==========================================
//                     CRUD
// ==========================================

// Ajouter une salle
bool Salle::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO SALLE (ID_SALLE, NOM_SALLE, CAPACITE, TYPE_SALLE) "
                  "VALUES (:id, :nom, :cap, :type)");

    query.bindValue(":id", idSalle);
    query.bindValue(":nom", nomSalle);
    query.bindValue(":cap", capacite);
    query.bindValue(":type", typeSalle);

    if (!query.exec()) {
        qDebug() << "Erreur ajout salle:" << query.lastError().text();
        return false;
    }
    return true;
}

// Afficher les salles
QSqlQueryModel* Salle::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();

    model->setQuery("SELECT ID_SALLE, NOM_SALLE, CAPACITE, TYPE_SALLE FROM SALLE");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom Salle"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Capacité"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type Salle"));

    return model;
}

// Supprimer une salle
bool Salle::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM SALLE WHERE ID_SALLE = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur suppression salle:" << query.lastError().text();
        return false;
    }
    return true;
}

// Modifier une salle
bool Salle::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE SALLE SET NOM_SALLE = :nom, CAPACITE = :cap, TYPE_SALLE = :type "
                  "WHERE ID_SALLE = :id");

    query.bindValue(":id", idSalle);
    query.bindValue(":nom", nomSalle);
    query.bindValue(":cap", capacite);
    query.bindValue(":type", typeSalle);

    if (!query.exec()) {
        qDebug() << "Erreur modification salle:" << query.lastError().text();
        return false;
    }
    return true;
}

// ==========================================
//               MÉTIERS AVANCÉS
// ==========================================

// Recherche simple par Nom de Salle
QSqlQueryModel* Salle::rechercher(QString valeur)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT ID_SALLE, NOM_SALLE, CAPACITE, TYPE_SALLE "
                  "FROM SALLE "
                  "WHERE NOM_SALLE LIKE :val");

    query.bindValue(":val", "%" + valeur + "%");
    query.exec();

    model->setQuery(query);
    return model;
}

// Tri dynamique
QSqlQueryModel* Salle::trier(QString critere)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    QString queryString = "SELECT ID_SALLE, NOM_SALLE, CAPACITE, TYPE_SALLE "
                          "FROM SALLE ORDER BY " + critere + " ASC";

    query.prepare(queryString);
    query.exec();

    model->setQuery(query);
    return model;
}

// Statistiques (compter par type de salle)
int Salle::countSallesParType(QString t)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM SALLE WHERE TYPE_SALLE = :type");
    query.bindValue(":type", t);

    if (query.exec() && query.next())
        return query.value(0).toInt();

    return 0;
}

// Génération PDF
bool Salle::genererPDF()
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("Fiche_Salle_" + QString::number(idSalle) + ".pdf");

    QTextDocument doc;
    QString html = "<html><head><style>h1{color: #000033;} p{font-size: 14px;}</style></head><body>";
    html += "<h1>Fiche d'Information Salle</h1>";
    html += "<hr>";
    html += "<p><b>ID :</b> " + QString::number(idSalle) + "</p>";
    html += "<p><b>Nom de la Salle :</b> " + nomSalle + "</p>";
    html += "<p><b>Capacité :</b> " + QString::number(capacite) + " places</p>";
    html += "<p><b>Type :</b> " + typeSalle + "</p>";
    html += "<hr><p><i>Généré depuis l'application Centre de Formation</i></p>";
    html += "</body></html>";

    doc.setHtml(html);
    doc.print(&printer);
    return true;
}

