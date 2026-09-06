#include "centredeformation.h"
#include "smtp.h"
#include "salle.h"
#include "ui_centredeformation.h"
#include <QMessageBox>
#include <QDate>
#include <QTableWidgetItem>
#include <QDebug>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QItemSelectionModel>
#include <QInputDialog>

// ----- Includes pour le contrôle de saisie -----
#include <QRegularExpression>
#include <QRegularExpressionValidator>

// ----- Includes pour l'export PDF -----
#include <QPdfWriter>
#include <QPainter>

// ----- Includes pour les Statistiques -----
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QVBoxLayout>

// ===========================================================
//               CONSTRUCTEUR PRINCIPAL
// ===========================================================
CentreDeFormation::CentreDeFormation(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CentreDeFormation)
{
    ui->setupUi(this);

    // ===========================================================
    //               CONTRÔLE DE SAISIE (VALIDATEURS)
    // ===========================================================

    // 1. ID Stagiaire : Uniquement des chiffres, et EXACTEMENT 8 chiffres
    QRegularExpression rxId("^[0-9]{8}$");
    QValidator *validatorId = new QRegularExpressionValidator(rxId, this);
    ui->lineEditStagiaireID->setValidator(validatorId);

    // 2. Nom et Prénom Stagiaire : Uniquement des lettres (avec accents et espaces)
    QRegularExpression rxLettres("^[a-zA-ZÀ-ÿ\\s]+$");
    QValidator *validatorNom = new QRegularExpressionValidator(rxLettres, this);
    ui->lineEditStagiaireNom->setValidator(validatorNom);
    ui->lineEditStagiairePrenom->setValidator(validatorNom);

    // ===========================================================

    // Initialisation : on affiche la page 0 (Stagiaires) et on charge les données
    ui->stackedWidget->setCurrentIndex(0);
    on_btnAfficherStagiaire_clicked();
}

CentreDeFormation::~CentreDeFormation()
{
    delete ui;
}

// ===========================================================
//               NAVIGATION (MENU ROSE)
// ===========================================================
void CentreDeFormation::on_btnGestionStagiairesNav_clicked()
{
    // On bascule sur la page des stagiaires (index 0)
    ui->stackedWidget->setCurrentIndex(0);
    ui->label_title->setText("Gestion des Stagiaires");
    on_btnAfficherStagiaire_clicked();
}

void CentreDeFormation::on_btnGestionSalles_clicked()
{
    // On bascule sur la page des salles (index 1)
    ui->stackedWidget->setCurrentIndex(1);
    ui->label_title->setText("Gestion des Salles");
    on_btnAfficherSalle_clicked();
}

void CentreDeFormation::on_btnQuitter_clicked() {
    if (QMessageBox::question(this, "Quitter", "Voulez-vous vraiment quitter ?") == QMessageBox::Yes)
        close();
}

// ===========================================================
//                     CRUD STAGIAIRES
// ===========================================================

// ----------------- AJOUTER -----------------
void CentreDeFormation::on_btnAjouterStagiaire_clicked()
{
    // Récupération de l'ID en texte d'abord pour vérifier sa longueur
    QString idTexte = ui->lineEditStagiaireID->text();

    // Contrôle de la longueur de l'ID
    if (idTexte.length() != 8) {
        QMessageBox::warning(this, "Erreur de saisie", "L'ID du stagiaire doit contenir exactement 8 chiffres !");
        return;
    }

    int id = idTexte.toInt();
    QString nom = ui->lineEditStagiaireNom->text();
    QString prenom = ui->lineEditStagiairePrenom->text();
    QDate dateN = ui->dateEditStagiaire->date();

    // ID Filière récupéré depuis la ComboBox (1, 2, 3 ou 4)
    int idFiliere = ui->comboBoxFormation->currentIndex() + 1;

    QString genre;
    if (ui->checkBoxHomme->isChecked()) {
        genre = "Homme";
    } else if (ui->checkBoxFemme->isChecked()) {
        genre = "Femme";
    }

    if (nom.isEmpty() || prenom.isEmpty() || genre.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs !");
        return;
    }

    S = Stagiaire(id, nom, prenom, dateN, genre, idFiliere);

    if (S.ajouter()) {
        QMessageBox::information(this, "Succès", "Stagiaire ajouté avec succès !");
        on_btnAfficherStagiaire_clicked();  // Rafraîchir la table

        // Vider les champs
        ui->lineEditStagiaireID->clear();
        ui->lineEditStagiaireNom->clear();
        ui->lineEditStagiairePrenom->clear();
        ui->dateEditStagiaire->setDate(QDate::currentDate());
        ui->checkBoxHomme->setChecked(false);
        ui->checkBoxFemme->setChecked(false);
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l’ajout du stagiaire !");
    }
}

// ----------------- SUPPRIMER -----------------
void CentreDeFormation::on_btnSupprimerStagiaire_clicked()
{
    int id = ui->lineEditStagiaireID->text().toInt();
    if (id <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un stagiaire ou saisir un ID valide !");
        return;
    }

    if (QMessageBox::question(this, "Confirmation",
                              "Voulez-vous vraiment supprimer ce stagiaire ?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        if (S.supprimer(id)) {
            QMessageBox::information(this, "Succès", "Stagiaire supprimé avec succès !");
            on_btnAfficherStagiaire_clicked();
            ui->lineEditStagiaireID->clear();
        } else {
            QMessageBox::warning(this, "Erreur", "Échec de la suppression !");
        }
    }
}

// ----------------- MODIFIER -----------------
void CentreDeFormation::on_btnModifierStagiaire_clicked()
{
    QString idTexte = ui->lineEditStagiaireID->text();

    // Contrôle de la longueur de l'ID
    if (idTexte.length() != 8) {
        QMessageBox::warning(this, "Erreur de saisie", "L'ID du stagiaire doit contenir exactement 8 chiffres !");
        return;
    }

    int id = idTexte.toInt();
    QString nom = ui->lineEditStagiaireNom->text();
    QString prenom = ui->lineEditStagiairePrenom->text();
    QDate dateN = ui->dateEditStagiaire->date();
    int idFiliere = ui->comboBoxFormation->currentIndex() + 1;

    QString genre;
    if (ui->checkBoxHomme->isChecked()) {
        genre = "Homme";
    } else if (ui->checkBoxFemme->isChecked()) {
        genre = "Femme";
    }

    if (nom.isEmpty() || prenom.isEmpty() || genre.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs !");
        return;
    }

    S = Stagiaire(id, nom, prenom, dateN, genre, idFiliere);

    if (S.modifier()) {
        QMessageBox::information(this, "Succès", "Stagiaire modifié avec succès !");
        on_btnAfficherStagiaire_clicked();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification !");
    }
}

// ----------------- AFFICHER (Remplir Tableau - 7 Colonnes) -----------------
void CentreDeFormation::on_btnAfficherStagiaire_clicked()
{
    QSqlQueryModel* model = S.afficher();

    if (!model || model->rowCount() == 0) {
        ui->tableWidgetStagiaires->setRowCount(0);
        return;
    }

    ui->tableWidgetStagiaires->clearContents();
    ui->tableWidgetStagiaires->setRowCount(model->rowCount());
    ui->tableWidgetStagiaires->setColumnCount(7); // 7 Colonnes distinctes

    QStringList headers = {"ID", "Nom", "Prénom", "Date Naissance", "Genre", "ID Filière", "Formation"};
    ui->tableWidgetStagiaires->setHorizontalHeaderLabels(headers);

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QString value = model->data(model->index(row, col)).toString();

            // Formatage de la date (colonne 3)
            if (col == 3) {
                QDate d = model->data(model->index(row, col)).toDate();
                value = d.toString("dd/MM/yyyy");
            }

            ui->tableWidgetStagiaires->setItem(row, col, new QTableWidgetItem(value));
        }
    }

    ui->tableWidgetStagiaires->resizeColumnsToContents();
    delete model;
}

// ----------------- CLIC SUR LE TABLEAU -----------------
void CentreDeFormation::on_tableWidgetStagiaires_cellClicked(int row, int column)
{
    ui->lineEditStagiaireID->setText(ui->tableWidgetStagiaires->item(row, 0)->text());
    ui->lineEditStagiaireNom->setText(ui->tableWidgetStagiaires->item(row, 1)->text());
    ui->lineEditStagiairePrenom->setText(ui->tableWidgetStagiaires->item(row, 2)->text());

    QDate dateN = QDate::fromString(ui->tableWidgetStagiaires->item(row, 3)->text(), "dd/MM/yyyy");
    ui->dateEditStagiaire->setDate(dateN);

    QString genre = ui->tableWidgetStagiaires->item(row, 4)->text();
    if (genre.toLower() == "homme") {
        ui->checkBoxHomme->setChecked(true);
        ui->checkBoxFemme->setChecked(false);
    } else {
        ui->checkBoxFemme->setChecked(true);
        ui->checkBoxHomme->setChecked(false);
    }

    int idFil = ui->tableWidgetStagiaires->item(row, 5)->text().toInt();
    if (idFil > 0 && idFil <= ui->comboBoxFormation->count()) {
        ui->comboBoxFormation->setCurrentIndex(idFil - 1);
    }
}

// ===========================================================
//                     MÉTIERS AVANCÉS STAGIAIRES
// ===========================================================

// ----------------- RECHERCHE PAR PRÉNOM -----------------
void CentreDeFormation::on_btnRechercherStagiaire_clicked()
{
    QString valeur = ui->lineEditRechercherStagiaire->text();

    if (valeur.isEmpty()) {
        QMessageBox::warning(this, "Attention", "Veuillez entrer un prénom à chercher !");
        on_btnAfficherStagiaire_clicked();
        return;
    }

    QSqlQueryModel* model = S.rechercher(valeur);
    ui->tableWidgetStagiaires->setRowCount(0);

    if (model->rowCount() == 0) {
        QMessageBox::information(this, "Recherche", "Aucun stagiaire trouvé avec ce prénom !");
        delete model;
        return;
    }

    ui->tableWidgetStagiaires->setColumnCount(7); // 7 Colonnes
    QStringList headers = {"ID", "Nom", "Prénom", "Date Naissance", "Genre", "ID Filière", "Formation"};
    ui->tableWidgetStagiaires->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < model->rowCount(); i++) {
        ui->tableWidgetStagiaires->insertRow(i);
        for (int j = 0; j < model->columnCount(); j++) {
            QString data = model->data(model->index(i, j)).toString();
            if (j == 3) {
                QDate d = model->data(model->index(i, j)).toDate();
                data = d.toString("dd/MM/yyyy");
            }
            ui->tableWidgetStagiaires->setItem(i, j, new QTableWidgetItem(data));
        }
    }
    delete model;
}

// ----------------- TRI PAR ID STAGIAIRE -----------------
void CentreDeFormation::on_btnTrierStagiaires_clicked()
{
    QSqlQueryModel* model = S.trier("ID_STAGIAIRE");

    ui->tableWidgetStagiaires->clearContents();
    ui->tableWidgetStagiaires->setRowCount(0);

    if (!model || model->rowCount() == 0) {
        if (model) delete model;
        return;
    }

    ui->tableWidgetStagiaires->setColumnCount(7); // Bien 7 colonnes pour tout afficher
    QStringList headers = {"ID", "Nom", "Prénom", "Date Naissance", "Genre", "ID Filière", "Formation"};
    ui->tableWidgetStagiaires->setHorizontalHeaderLabels(headers);

    ui->tableWidgetStagiaires->setRowCount(model->rowCount());

    for (int i = 0; i < model->rowCount(); ++i) {
        for (int j = 0; j < model->columnCount(); ++j) {
            QString value = model->data(model->index(i, j)).toString();
            if (j == 3) {
                QDate d = model->data(model->index(i, j)).toDate();
                value = d.toString("dd/MM/yyyy");
            }
            ui->tableWidgetStagiaires->setItem(i, j, new QTableWidgetItem(value));
        }
    }
    ui->tableWidgetStagiaires->resizeColumnsToContents();
    delete model;
}
// ----------------- EXPORT PDF -----------------
void CentreDeFormation::on_btnExportStagiairesPDF_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (fileName.isEmpty()) return;
    if (QFileInfo(fileName).suffix().isEmpty()) fileName.append(".pdf");

    QPdfWriter pdf(fileName);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setResolution(300);

    QPainter painter(&pdf);
    QRect pageRect = pdf.pageLayout().paintRectPixels(pdf.resolution());
    int margin = 10;

    // Titre
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    QRect titleRect(pageRect.left(), pageRect.top(), pageRect.width(), 70);
    painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignTop, "Liste des Stagiaires");

    // Table
    int rows = ui->tableWidgetStagiaires->rowCount();
    int cols = ui->tableWidgetStagiaires->columnCount();
    int x = pageRect.left() + margin;
    int y = pageRect.top() + 90;
    int tableWidth = pageRect.width() - 2 * margin;
    int colWidth = tableWidth / cols;
    int rowHeight = 120;

    // En-tête
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.setPen(Qt::black);
    for (int c = 0; c < cols; ++c) {
        painter.fillRect(x + c*colWidth, y, colWidth, rowHeight, QColor(180, 180, 250));
        painter.drawRect(x + c*colWidth, y, colWidth, rowHeight);
        painter.drawText(x + c*colWidth, y, colWidth, rowHeight, Qt::AlignCenter,
                         ui->tableWidgetStagiaires->horizontalHeaderItem(c)->text());
    }
    y += rowHeight;

    // Contenu
    painter.setFont(QFont("Arial", 8));
    for (int r = 0; r < rows; ++r) {
        if (y + rowHeight > pageRect.bottom() - margin) {
            pdf.newPage();
            y = pageRect.top() + 90;
        }

        QColor lineColor = (r % 2 == 0) ? QColor(240,240,240) : QColor(255,255,255);
        for (int c = 0; c < cols; ++c) {
            painter.fillRect(x + c*colWidth, y, colWidth, rowHeight, lineColor);
            painter.drawRect(x + c*colWidth, y, colWidth, rowHeight);
            QString data = ui->tableWidgetStagiaires->item(r, c)->text();
            painter.drawText(x + c*colWidth, y, colWidth, rowHeight, Qt::AlignCenter | Qt::AlignVCenter, data);
        }
        y += rowHeight;
    }
    painter.end();
    QMessageBox::information(this, "PDF", "Exportation réussie !");
}

// ----------------- STATISTIQUES -----------------
void CentreDeFormation::on_btnStatistiqueStagiaires_clicked()
{
    int nbHomme = 0;
    int nbFemme = 0;

    QAbstractItemModel *model = ui->tableWidgetStagiaires->model();

    if (model) {
        for (int row = 0; row < model->rowCount(); ++row) {
            // La colonne 4 correspond au Genre
            QString genre = model->data(model->index(row, 4)).toString().trimmed().toUpper();

            if (genre == "HOMME")        nbHomme++;
            else if (genre == "FEMME")   nbFemme++;
        }
    }

    QPieSeries *series = new QPieSeries();
    QPieSlice *sliceHomme = series->append("Homme", nbHomme);
    QPieSlice *sliceFemme = series->append("Femme", nbFemme);

    sliceHomme->setBrush(QColor("#2196F3"));  // Bleu
    sliceFemme->setBrush(QColor("#FF4C93"));  // Rose

    for (QPieSlice *slice : series->slices()) {
        slice->setLabelVisible(true);
        double percent = 0.0;
        if (nbHomme + nbFemme > 0) {
            percent = (slice->value() / (nbHomme + nbFemme)) * 100.0;
        }

        slice->setLabel(QString("%1 : %2 (%3%)")
                            .arg(slice->label())
                            .arg(slice->value())
                            .arg(QString::number(percent, 'f', 1)));

        slice->setLabelColor(Qt::black);
        slice->setLabelFont(QFont("Arial", 10, QFont::Bold));
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des Stagiaires");
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QChartView *ancienGraphique = ui->page_stagiaires->findChild<QChartView*>("monGraphique");
    if (ancienGraphique) {
        delete ancienGraphique;
    }

    chartView->setParent(ui->page_stagiaires); // Parent = page_stagiaires
    chartView->setObjectName("monGraphique");
    chartView->setGeometry(ui->textEditStagiaireInfo->geometry());

    ui->textEditStagiaireInfo->hide();
    chartView->show();
}

// ----------------- QR CODE -----------------
void CentreDeFormation::on_btnQRStagiaire_clicked()
{
    QAbstractItemModel *model = ui->tableWidgetStagiaires->model();

    QItemSelectionModel *selection = ui->tableWidgetStagiaires->selectionModel();
    if (!selection->hasSelection()) {
        QMessageBox::warning(this, "QR Code", "Veuillez sélectionner un stagiaire dans le tableau.");
        return;
    }

    int row = selection->currentIndex().row();

    // 7 Colonnes : 0=ID, 1=Nom, 2=Prénom, 3=Date, 4=Genre, 5=ID Filière, 6=Formation
    QString idStag  = model->data(model->index(row, 0)).toString();
    QString nom     = model->data(model->index(row, 1)).toString();
    QString prenom  = model->data(model->index(row, 2)).toString();
    QString dateN   = model->data(model->index(row, 3)).toString();
    QString genre   = model->data(model->index(row, 4)).toString();
    QString idFil   = model->data(model->index(row, 5)).toString();
    QString form    = model->data(model->index(row, 6)).toString();

    QString texteNote = "=== FICHE STAGIAIRE ===\n";
    texteNote += "ID : " + idStag + "\n";
    texteNote += "Nom : " + nom + "\n";
    texteNote += "Prénom : " + prenom + "\n";
    texteNote += "Date Nais. : " + dateN.remove("T00:00:00") + "\n";
    texteNote += "Genre : " + genre + "\n";
    texteNote += "ID Filière : " + idFil + "\n";
    texteNote += "Formation : " + form;

    Stagiaire s;
    QImage qrImage = s.genererQRCode(texteNote);

    if (qrImage.isNull()) {
        QMessageBox::warning(this, "QR Code", "Impossible de générer le QR Code.");
        return;
    }

    ui->label_logoQR->setPixmap(QPixmap::fromImage(qrImage).scaled(
        ui->label_logoQR->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
}


// ===========================================================
//                     CRUD SALLES
// ===========================================================

void CentreDeFormation::on_btnAjouterSalle_clicked()
{
    int id = ui->lineEditSalleID->text().toInt();
    QString nom = ui->lineEditSalleNom->text();
    int cap = ui->lineEditSalleCapacite->text().toInt();
    QString type = ui->comboBoxSalleType->currentText();

    if (id <= 0 || nom.isEmpty() || cap <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs correctement !");
        return;
    }

    Sa = Salle(id, nom, cap, type);

    if (Sa.ajouter()) {
        QMessageBox::information(this, "Succès", "Salle ajoutée avec succès !");
        on_btnAfficherSalle_clicked();

        ui->lineEditSalleID->clear();
        ui->lineEditSalleNom->clear();
        ui->lineEditSalleCapacite->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "L'ajout a échoué (ID existe peut-être déjà).");
    }
}

void CentreDeFormation::on_btnModifierSalle_clicked()
{
    int id = ui->lineEditSalleID->text().toInt();
    QString nom = ui->lineEditSalleNom->text();
    int cap = ui->lineEditSalleCapacite->text().toInt();
    QString type = ui->comboBoxSalleType->currentText();

    if (id <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une salle pour la modifier !");
        return;
    }

    Sa = Salle(id, nom, cap, type);

    if (Sa.modifier()) {
        QMessageBox::information(this, "Succès", "Salle modifiée avec succès !");
        on_btnAfficherSalle_clicked();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification !");
    }
}

void CentreDeFormation::on_btnSupprimerSalle_clicked()
{
    int id = ui->lineEditSalleID->text().toInt();
    if (id <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une salle à supprimer !");
        return;
    }

    if (QMessageBox::question(this, "Confirmation", "Voulez-vous vraiment supprimer cette salle ?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (Sa.supprimer(id)) {
            QMessageBox::information(this, "Succès", "Salle supprimée avec succès !");
            on_btnAfficherSalle_clicked();
            ui->lineEditSalleID->clear();
        } else {
            QMessageBox::warning(this, "Erreur", "Échec de la suppression !");
        }
    }
}

void CentreDeFormation::on_btnAfficherSalle_clicked()
{
    // 1. Nettoyer le graphique des statistiques s'il est présent et réafficher le cadre du bas
    QChartView *ancienGraphique = ui->page_salles->findChild<QChartView*>("graphiqueSalles");
    if (ancienGraphique) {
        delete ancienGraphique;
    }
    ui->textEditSalleInfo->show();

    // 2. Récupérer les données de la base via la classe Salle
    QSqlQueryModel* model = Sa.afficher();

    if (!model || model->rowCount() == 0) {
        ui->tableWidgetSalles->setRowCount(0);
        if (model) delete model;
        return;
    }

    // 3. Remplir le tableau des salles
    ui->tableWidgetSalles->clearContents();
    ui->tableWidgetSalles->setRowCount(model->rowCount());
    ui->tableWidgetSalles->setColumnCount(4);

    QStringList headers = {"ID", "Nom Salle", "Capacité", "Type"};
    ui->tableWidgetSalles->setHorizontalHeaderLabels(headers);

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QString value = model->data(model->index(row, col)).toString();
            ui->tableWidgetSalles->setItem(row, col, new QTableWidgetItem(value));
        }
    }

    ui->tableWidgetSalles->resizeColumnsToContents();
    delete model;
}
// ----------------- TRI PAR ID SALLE -----------------
void CentreDeFormation::on_btnTrierSalles_clicked()
{
    QSqlQueryModel* model = Sa.trier("ID_SALLE");

    ui->tableWidgetSalles->clearContents();
    ui->tableWidgetSalles->setRowCount(0);

    if (model->rowCount() == 0) {
        delete model;
        return;
    }

    ui->tableWidgetSalles->setColumnCount(4);
    ui->tableWidgetSalles->setHorizontalHeaderLabels({"ID", "Nom Salle", "Capacité", "Type"});

    for (int i = 0; i < model->rowCount(); ++i) {
        ui->tableWidgetSalles->insertRow(i);
        for (int j = 0; j < model->columnCount(); ++j) {
            QString value = model->data(model->index(i, j)).toString();
            ui->tableWidgetSalles->setItem(i, j, new QTableWidgetItem(value));
        }
    }
    ui->tableWidgetSalles->resizeColumnsToContents();
    delete model;
}

// ----------------- EXPORT PDF SALLES -----------------
void CentreDeFormation::on_btnExportSallesPDF_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (fileName.isEmpty()) return;
    if (QFileInfo(fileName).suffix().isEmpty()) fileName.append(".pdf");

    QPdfWriter pdf(fileName);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setResolution(300);

    QPainter painter(&pdf);
    QRect pageRect = pdf.pageLayout().paintRectPixels(pdf.resolution());
    int margin = 10;

    // Titre
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    QRect titleRect(pageRect.left(), pageRect.top(), pageRect.width(), 70);
    painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignTop, "Liste des Salles");

    // Dimensions de la Table
    int rows = ui->tableWidgetSalles->rowCount();
    int cols = ui->tableWidgetSalles->columnCount();
    int x = pageRect.left() + margin;
    int y = pageRect.top() + 90;
    int tableWidth = pageRect.width() - 2 * margin;
    int colWidth = tableWidth / cols;
    int rowHeight = 120;

    // En-tête
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.setPen(Qt::black);
    for (int c = 0; c < cols; ++c) {
        painter.fillRect(x + c*colWidth, y, colWidth, rowHeight, QColor(180, 250, 180)); // Vert clair pour les Salles
        painter.drawRect(x + c*colWidth, y, colWidth, rowHeight);
        painter.drawText(x + c*colWidth, y, colWidth, rowHeight, Qt::AlignCenter,
                         ui->tableWidgetSalles->horizontalHeaderItem(c)->text());
    }
    y += rowHeight;

    // Contenu
    painter.setFont(QFont("Arial", 8));
    for (int r = 0; r < rows; ++r) {
        if (y + rowHeight > pageRect.bottom() - margin) {
            pdf.newPage();
            y = pageRect.top() + 90;
        }

        QColor lineColor = (r % 2 == 0) ? QColor(240,240,240) : QColor(255,255,255);
        for (int c = 0; c < cols; ++c) {
            painter.fillRect(x + c*colWidth, y, colWidth, rowHeight, lineColor);
            painter.drawRect(x + c*colWidth, y, colWidth, rowHeight);
            QString data = ui->tableWidgetSalles->item(r, c)->text();
            painter.drawText(x + c*colWidth, y, colWidth, rowHeight, Qt::AlignCenter | Qt::AlignVCenter, data);
        }
        y += rowHeight;
    }
    painter.end();
    QMessageBox::information(this, "PDF", "Exportation des salles réussie !");
}

// ----------------- STATISTIQUES PAR TYPE -----------------
void CentreDeFormation::on_btnStatistiqueSalles_clicked()
{
    int nbCours = 0;
    int nbTP = 0;
    int nbReunion = 0;

    QAbstractItemModel *model = ui->tableWidgetSalles->model();

    if (model) {
        for (int row = 0; row < model->rowCount(); ++row) {
            // La colonne 3 correspond au Type de la salle
            QString type = model->data(model->index(row, 3)).toString().trimmed();

            if (type == "Salle de Cours") nbCours++;
            else if (type == "Laboratoire (TP)") nbTP++;
            else if (type == "Salle de Réunion") nbReunion++;
        }
    }

    QPieSeries *series = new QPieSeries();
    series->append("Cours", nbCours)->setBrush(QColor("#4CAF50")); // Vert
    series->append("Laboratoire", nbTP)->setBrush(QColor("#FF9800")); // Orange
    series->append("Réunion", nbReunion)->setBrush(QColor("#9C27B0")); // Violet

    int total = nbCours + nbTP + nbReunion;
    for (QPieSlice *slice : series->slices()) {
        slice->setLabelVisible(true);
        double percent = (total > 0) ? (slice->value() / total) * 100.0 : 0.0;

        slice->setLabel(QString("%1 : %2 (%3%)")
                            .arg(slice->label())
                            .arg(slice->value())
                            .arg(QString::number(percent, 'f', 1)));

        slice->setLabelColor(Qt::black);
        slice->setLabelFont(QFont("Arial", 10, QFont::Bold));
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des Salles par Type");
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Supprimer l'ancien graphique s'il existe déjà
    QChartView *ancienGraphique = ui->page_salles->findChild<QChartView*>("graphiqueSalles");
    if (ancienGraphique) {
        delete ancienGraphique;
    }

    // IMPORTANT : On fixe le parent à ui->page_salles (et non centralwidget)
    chartView->setParent(ui->page_salles);
    chartView->setObjectName("graphiqueSalles");

    // Il prendra exactement la géométrie du cadre blanc du bas
    chartView->setGeometry(ui->textEditSalleInfo->geometry());

    ui->textEditSalleInfo->hide(); // On cache le cadre blanc
    chartView->show();             // Le graphique s'affiche pile à sa place en bas
}

// ----------------- CLIC SUR LE TABLEAU DES SALLES -----------------
void CentreDeFormation::on_tableWidgetSalles_cellClicked(int row, int column)
{
    ui->lineEditSalleID->setText(ui->tableWidgetSalles->item(row, 0)->text());
    ui->lineEditSalleNom->setText(ui->tableWidgetSalles->item(row, 1)->text());
    ui->lineEditSalleCapacite->setText(ui->tableWidgetSalles->item(row, 2)->text());
    ui->comboBoxSalleType->setCurrentText(ui->tableWidgetSalles->item(row, 3)->text());
}

// ----------------- RECHERCHE PAR NOM DE SALLE -----------------
void CentreDeFormation::on_btnRechercherSalle_clicked()
{
    QString valeur = ui->lineEditRechercherSalle->text();

    if (valeur.isEmpty()) {
        QMessageBox::warning(this, "Attention", "Veuillez entrer un nom de salle à chercher !");
        on_btnAfficherSalle_clicked();
        return;
    }

    QSqlQueryModel* model = Sa.rechercher(valeur);
    ui->tableWidgetSalles->setRowCount(0);

    if (model->rowCount() == 0) {
        QMessageBox::information(this, "Recherche", "Aucune salle trouvée avec ce nom !");
        delete model;
        return;
    }

    ui->tableWidgetSalles->setColumnCount(4);
    ui->tableWidgetSalles->setHorizontalHeaderLabels({"ID", "Nom Salle", "Capacité", "Type"});

    for (int i = 0; i < model->rowCount(); i++) {
        ui->tableWidgetSalles->insertRow(i);
        for (int j = 0; j < model->columnCount(); j++) {
            QString data = model->data(model->index(i, j)).toString();
            ui->tableWidgetSalles->setItem(i, j, new QTableWidgetItem(data));
        }
    }
    delete model;
}

//--------------------------MAILING-------------------------------------------------------------------
void CentreDeFormation::on_btnMailingSalle_clicked()
{
    QItemSelectionModel *selection = ui->tableWidgetSalles->selectionModel();
    if (!selection->hasSelection()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une salle dans le tableau !");
        return;
    }

    int row = selection->currentIndex().row();

    // SÉCURITÉ : Empêcher le crash si la ligne est vide
    QTableWidgetItem *item = ui->tableWidgetSalles->item(row, 1);
    if (!item) return;

    QString nomSalle = item->text();

    // ==========================================================
    // 1. CHOIX DU TYPE D'ALERTE PAR L'UTILISATEUR
    // ==========================================================
    QStringList optionsDeMaintenance;
    optionsDeMaintenance << "Problème matériel (Vidéoprojecteur, PC, etc.)"
                         << "Problème électrique / Climatisation"
                         << "Besoin d'intervention de nettoyage"
                         << "INFO : Salle libérée et prête à l'usage"
                         << "INFO : Salle fermée pour travaux";

    bool ok;
    QString choixMaintenance = QInputDialog::getItem(this,
                                                     "Type d'alerte",
                                                     "Sélectionnez le statut ou la panne pour cette salle :",
                                                     optionsDeMaintenance,
                                                     0,
                                                     false,
                                                     &ok);

    // Si l'utilisateur clique sur "Annuler" ou ferme la fenêtre, on arrête tout
    if (!ok || choixMaintenance.isEmpty()) {
        return;
    }

    // ==========================================================
    // 2. CONFIGURATION SMTP
    // ==========================================================
    QString monEmail = "chahinechaiebistheboss@gmail.com";
    QString monMotDePasse = "dyhs ytlm vxdl fwoq";

    Smtp* smtp = new Smtp(monEmail, monMotDePasse, "smtp.gmail.com", 465);

    QString destinataire = "chahinechaiebistheboss@gmail.com";
    QString sujet = "[Alerte] Statut de la salle : " + nomSalle;

    // ==========================================================
    // 3. DESIGN HTML PROFESSIONNEL DE L'E-MAIL
    // ==========================================================
    QString messageHTML = "<div style='font-family: Arial, sans-serif; border: 1px solid #ccc; border-radius: 5px; max-width: 600px;'>"
                          "<div style='background-color: #0b3c68; padding: 15px;'>"
                          "<h2 style='color: white; margin: 0;'>Notification - Infrastructure</h2>"
                          "</div>"
                          "<div style='padding: 20px;'>"
                          "<p>Bonjour,</p>"
                          "<p>Une mise à jour concernant l'infrastructure du Centre de Formation vient d'être signalée :</p>"
                          "<table style='width: 100%; border-collapse: collapse; margin-top: 15px;'>"
                          "<tr>"
                          "<td style='padding: 10px; border: 1px solid #ddd; font-weight: bold; background-color: #f9f9f9; width: 40%;'>Salle concernée :</td>"
                          "<td style='padding: 10px; border: 1px solid #ddd;'>" + nomSalle + "</td>"
                                       "</tr>"
                                       "<tr>"
                                       "<td style='padding: 10px; border: 1px solid #ddd; font-weight: bold; background-color: #f9f9f9;'>Statut / Signalement :</td>"
                                       "<td style='padding: 10px; border: 1px solid #ddd; color: #d9534f; font-weight: bold;'>" + choixMaintenance + "</td>"
                                               "</tr>"
                                               "</table>"
                                               "<p style='margin-top: 20px;'>Merci de prendre les dispositions nécessaires ou d'en prendre bonne note.</p>"
                                               "</div>"
                                               "<div style='background-color: #f1f1f1; padding: 10px; text-align: center; font-size: 11px; color: #777; border-top: 1px solid #ccc;'>"
                                               "<i>Ceci est un message généré de manière automatique par l'application Qt du Centre de Formation. Veuillez ne pas y répondre directement.</i>"
                                               "</div>"
                                               "</div>";

    smtp->sendMail(monEmail, destinataire, sujet, messageHTML);
}
