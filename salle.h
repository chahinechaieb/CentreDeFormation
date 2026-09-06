#ifndef SALLE_H
#define SALLE_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QImage>

class Salle
{
private:
    int idSalle;
    QString nomSalle;
    int capacite;
    QString typeSalle;

public:
    // Constructeurs
    Salle();
    Salle(int, QString, int, QString);

    // CRUD (Fonctionnalités de base obligatoires)
    bool ajouter();
    QSqlQueryModel* afficher();
    bool modifier();
    bool supprimer(int);

    // Fonctionnalités Avancées (Métiers)
    QSqlQueryModel* rechercher(QString valeur); // Recherche multicritères
    QSqlQueryModel* trier(QString critere);     // Tri multicritères
    int countSallesParType(QString t);          // Pour les statistiques
    bool genererPDF();                          // Génération PDF


    // Getters
    int getIdSalle() const { return idSalle; }
    QString getNomSalle() const { return nomSalle; }
    int getCapacite() const { return capacite; }
    QString getTypeSalle() const { return typeSalle; }

    // Setters
    void setIdSalle(int i) { idSalle = i; }
    void setNomSalle(QString n) { nomSalle = n; }
    void setCapacite(int c) { capacite = c; }
    void setTypeSalle(QString t) { typeSalle = t; }
};

#endif // SALLE_H
