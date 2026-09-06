#ifndef STAGIAIRE_H
#define STAGIAIRE_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QImage>

class Stagiaire
{
private:
    int id;
    QString nom;
    QString prenom;
    QDate dateNaissance;
    QString genre;
    int idFiliere;

public:
    // Constructeurs
    Stagiaire();
    Stagiaire(int, QString, QString, QDate, QString, int);

    // CRUD (Fonctionnalités de base obligatoires)
    bool ajouter();
    QSqlQueryModel* afficher();
    bool modifier();
    bool supprimer(int);

    // Fonctionnalités Avancées (Métiers)
    QSqlQueryModel* rechercher(QString valeur); // Recherche multicritères
    QSqlQueryModel* trier(QString critere);     // Tri multicritères
    int countStagiairesParGenre(QString g);     // Pour les statistiques (remplace vos countHomme/Femme)
    bool genererPDF();                          // Génération PDF personnalisée
    QImage genererQRCode(const QString &text);  // Métier libre 1 (gardé de votre ancien projet !)

    // Getters
    int getId() const { return id; }
    QString getNom() const { return nom; }
    QString getPrenom() const { return prenom; }
    QDate getDateNaissance() const { return dateNaissance; }
    QString getGenre() const { return genre; }
    int getIdFiliere() const { return idFiliere; }

    // Setters
    void setId(int i) { id = i; }
    void setNom(QString n) { nom = n; }
    void setPrenom(QString p) { prenom = p; }
    void setDateNaissance(QDate d) { dateNaissance = d; }
    void setGenre(QString g) { genre = g; }
    void setIdFiliere(int f) { idFiliere = f; }
};

#endif // STAGIAIRE_H
