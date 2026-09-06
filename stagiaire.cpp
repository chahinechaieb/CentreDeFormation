#include "stagiaire.h"
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
Stagiaire::Stagiaire()
{
    id = 0;
    nom = "";
    prenom = "";
    dateNaissance = QDate::currentDate();
    genre = "";
    idFiliere = 0;
}

// Constructeur paramétré
Stagiaire::Stagiaire(int id, QString nom, QString prenom, QDate dateNaissance, QString genre, int idFiliere)
{
    this->id = id;
    this->nom = nom;
    this->prenom = prenom;
    this->dateNaissance = dateNaissance;
    this->genre = genre;
    this->idFiliere = idFiliere;
}

// Ajouter un stagiaire
bool Stagiaire::ajouter()
{
    QSqlQuery query;
    // Requête très simple, sans complication, comme dans Client
    query.prepare("INSERT INTO STAGIAIRE (ID_STAGIAIRE, NOM, PRENOM, DATE_NAISSANCE, GENRE, ID_FILIERE) "
                  "VALUES (:id, :nom, :prenom, :dateN, :genre, :idF)");

    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":dateN", dateNaissance);
    query.bindValue(":genre", genre);
    query.bindValue(":idF", idFiliere);

    if (!query.exec()) {
        qDebug() << "Erreur ajout stagiaire:" << query.lastError().text();
        return false;
    }
    return true;
}

// Afficher les stagiaires
QSqlQueryModel* Stagiaire::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();

    // On sélectionne l'ID de la filière ET son nom pour avoir 2 colonnes distinctes
    model->setQuery("SELECT ID_STAGIAIRE, NOM, PRENOM, DATE_NAISSANCE, GENRE, STAGIAIRE.ID_FILIERE, NOM_FILIERE "
                    "FROM STAGIAIRE "
                    "JOIN FILIERE ON STAGIAIRE.ID_FILIERE = FILIERE.ID_FILIERE");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Date Naissance"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Genre"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("ID Filière"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Formation"));

    return model;
}

// Supprimer un stagiaire
bool Stagiaire::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM STAGIAIRE WHERE ID_STAGIAIRE = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur suppression stagiaire:" << query.lastError().text();
        return false;
    }
    return true;
}

// Modifier un stagiaire
bool Stagiaire::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE STAGIAIRE SET NOM = :nom, PRENOM = :prenom, "
                  "DATE_NAISSANCE = :dateN, GENRE = :genre, ID_FILIERE = :idF "
                  "WHERE ID_STAGIAIRE = :id");

    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":dateN", dateNaissance);
    query.bindValue(":genre", genre);
    query.bindValue(":idF", idFiliere);

    if (!query.exec()) {
        qDebug() << "Erreur modification stagiaire:" << query.lastError().text();
        return false;
    }
    return true;
}

// Recherche multicritères simple
QSqlQueryModel* Stagiaire::rechercher(QString valeur)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT ID_STAGIAIRE, NOM, PRENOM, DATE_NAISSANCE, GENRE, STAGIAIRE.ID_FILIERE, NOM_FILIERE "
                  "FROM STAGIAIRE "
                  "JOIN FILIERE ON STAGIAIRE.ID_FILIERE = FILIERE.ID_FILIERE "
                  "WHERE PRENOM LIKE :val");

    query.bindValue(":val", "%" + valeur + "%");
    query.exec();

    model->setQuery(query);
    return model;
}

// Tri dynamique simple
QSqlQueryModel* Stagiaire::trier(QString critere)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    // On fait une jointure pour récupérer le nom de la filière (comme dans afficher)
    QString queryString = "SELECT s.ID_STAGIAIRE, s.NOM, s.PRENOM, s.DATE_NAISSANCE, s.GENRE, s.ID_FILIERE, f.NOM_FILIERE "
                          "FROM STAGIAIRE s "
                          "LEFT JOIN FILIERE f ON s.ID_FILIERE = f.ID_FILIERE "
                          "ORDER BY s." + critere + " ASC";

    model->setQuery(queryString);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Date Naissance"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Genre"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("ID Filière"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Formation"));

    return model;
}
// Génération PDF
bool Stagiaire::genererPDF()
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("Fiche_Stagiaire_" + QString::number(id) + ".pdf");

    QTextDocument doc;
    QString html = "<html><head><style>h1{color: #000033;} p{font-size: 14px;}</style></head><body>";
    html += "<h1>Fiche d'Information Stagiaire</h1>";
    html += "<hr>";
    html += "<p><b>ID :</b> " + QString::number(id) + "</p>";
    html += "<p><b>Nom complet :</b> " + nom + " " + prenom + "</p>";
    html += "<p><b>Date de Naissance :</b> " + dateNaissance.toString("dd/MM/yyyy") + "</p>";
    html += "<p><b>Genre :</b> " + genre + "</p>";
    html += "<p><b>ID Filière :</b> " + QString::number(idFiliere) + "</p>";
    html += "<hr><p><i>Généré depuis l'application Centre de Formation (2026)</i></p>";
    html += "</body></html>";

    doc.setHtml(html);
    doc.print(&printer);
    return true;
}


// --- L'EXACT MÊME CODE QR QUE CELUI DE VOTRE FICHIER CLIENT ---
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

QImage Stagiaire::genererQRCode(const QString &text)
{
    // encode
    QrCode qr = QrCode::encodeText(text.toUtf8().constData(), QrCode::Ecc::LOW);

    const int size = qr.getSize();
    const int scale = 6; // pixel size — augmente pour agrandir
    const int imgSize = size * scale;
    QImage img(imgSize, imgSize, QImage::Format_ARGB32);
    img.fill(Qt::white);

    QPainter p(&img);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            if (qr.getModule(x, y)) {
                p.drawRect(x * scale, y * scale, scale, scale);
            }
        }
    }
    p.end();
    return img;
}
