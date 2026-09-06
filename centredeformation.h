#ifndef CENTREDEFORMATION_H
#define CENTREDEFORMATION_H

#include <QMainWindow>
#include "stagiaire.h"
#include "salle.h" // Inclusion de la classe Salle

QT_BEGIN_NAMESPACE
namespace Ui { class CentreDeFormation; }
QT_END_NAMESPACE

class CentreDeFormation : public QMainWindow
{
    Q_OBJECT

public:
    CentreDeFormation(QWidget *parent = nullptr);
    ~CentreDeFormation();

private slots:
    // ================= NAVIGATION =================
    void on_btnGestionStagiairesNav_clicked();
    void on_btnGestionSalles_clicked();
    void on_btnQuitter_clicked();
    // ================= STAGIAIRES =================
    void on_btnAjouterStagiaire_clicked();
    void on_btnModifierStagiaire_clicked();
    void on_btnSupprimerStagiaire_clicked();
    void on_btnAfficherStagiaire_clicked();
    void on_tableWidgetStagiaires_cellClicked(int row, int column);

    // Métiers Stagiaires
    void on_btnRechercherStagiaire_clicked();
    void on_btnTrierStagiaires_clicked();
    void on_btnExportStagiairesPDF_clicked();
    void on_btnStatistiqueStagiaires_clicked();
    void on_btnQRStagiaire_clicked();
    // ================= SALLES =================
    void on_btnAjouterSalle_clicked();
    void on_btnModifierSalle_clicked();
    void on_btnSupprimerSalle_clicked();
    void on_btnAfficherSalle_clicked();
    void on_tableWidgetSalles_cellClicked(int row, int column);
    void on_btnMailingSalle_clicked();

    // NOUVELLES FONCTIONS AVANCÉES POUR LES SALLES
    void on_btnRechercherSalle_clicked();
    void on_btnTrierSalles_clicked();
    void on_btnExportSallesPDF_clicked();
    void on_btnStatistiqueSalles_clicked();
private:
    Ui::CentreDeFormation *ui;
    Stagiaire S; // Objet pour gérer les requêtes Stagiaire
    Salle Sa;    // Objet pour gérer les requêtes Salle
};

#endif // CENTREDEFORMATION_H
