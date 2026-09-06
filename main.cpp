#include "centredeformation.h"
#include "connection.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Connection c;
    bool test = c.createconnect();  // test de la connexion à la base de données

    if (test)
    {
        QMessageBox::information(nullptr, QObject::tr("Connexion à la base de données"),
                                 QObject::tr("Connexion réussie.\n"
                                             "Cliquez sur OK pour continuer."),
                                 QMessageBox::Ok);

        // Ouvre la nouvelle fenêtre principale
        CentreDeFormation w;
        w.show();

        return a.exec(); // démarre l’application Qt
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Erreur de connexion"),
                              QObject::tr("Échec de la connexion à la base de données.\n"
                                          "L'application va se fermer."),
                              QMessageBox::Ok);

        return 1; // quitte le programme
    }
}
