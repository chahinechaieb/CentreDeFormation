#include "smtp.h"

Smtp::Smtp(const QString &user, const QString &pass, const QString &host, int port)
{
    this->user = user;
    this->pass = pass;
    this->host = host;
    this->port = port;
}

void Smtp::sendMail(const QString &from, const QString &to, const QString &subject, const QString &body)
{
    message = "To: " + to + "\nFrom: " + from + "\nSubject: " + subject + "\n";
    message.append("Mime-Version: 1.0\nContent-Type: text/html; charset=\"utf-8\"\n\n");
    message.append(body);
    message.replace(QString::fromLatin1("\n"), QString::fromLatin1("\r\n"));
    message.replace(QString::fromLatin1("\r\n.\r\n"), QString::fromLatin1("\r\n..\r\n"));

    this->from = from;
    rcpt = to;
    state = Init;

    socket = new QSslSocket(this);
    t = new QTextStream(socket);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorReceived(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    socket->connectToHostEncrypted(host, port);
}

void Smtp::stateChanged(QAbstractSocket::SocketState socketState) {}

void Smtp::errorReceived(QAbstractSocket::SocketError socketError) {
    QMessageBox::warning(nullptr, "Erreur Réseau", "Erreur : " + socket->errorString());
    this->deleteLater(); // Auto-destruction
}

void Smtp::disconnected() {}
void Smtp::connected() {}

void Smtp::readyRead()
{
    QString responseLine;
    do {
        responseLine = socket->readLine();
        response += responseLine;
    } while (socket->canReadLine() && responseLine.length() > 3 && responseLine.at(3) != ' ');

    // Sécurité anti-crash
    if (responseLine.length() < 3) return;
    responseLine.truncate(3);

    if (state == Init && responseLine == "220") {
        *t << "EHLO localhost" << "\r\n"; t->flush(); state = HandShake;
    } else if (state == HandShake && responseLine == "250") {
        *t << "AUTH LOGIN" << "\r\n"; t->flush(); state = Auth;
    } else if (state == Auth && responseLine == "334") {
        *t << QByteArray().append(user.toUtf8()).toBase64() << "\r\n"; t->flush(); state = User;
    } else if (state == User && responseLine == "334") {
        *t << QByteArray().append(pass.toUtf8()).toBase64() << "\r\n"; t->flush(); state = Pass;
    } else if (state == Pass && responseLine == "235") {
        *t << "MAIL FROM:<" << from << ">" << "\r\n"; t->flush(); state = Mail;
    } else if (state == Mail && responseLine == "250") {
        *t << "RCPT TO:<" << rcpt << ">" << "\r\n"; t->flush(); state = Rcpt;
    } else if (state == Rcpt && responseLine == "250") {
        *t << "DATA" << "\r\n"; t->flush(); state = Data;
    } else if (state == Data && responseLine == "354") {
        *t << message << "\r\n.\r\n"; t->flush(); state = Body;
    } else if (state == Body && responseLine == "250") {
        *t << "QUIT" << "\r\n"; t->flush(); state = Quit;
    } else if (state == Quit && responseLine == "221") {
        // Afficher le message SEULEMENT quand le serveur confirme la fin
        socket->close();
        QMessageBox::information(nullptr, "Succès", "L'alerte a été envoyée avec succès !");
        this->deleteLater();
    } else {
        if (state != Quit) {
            QMessageBox::warning(nullptr, "Erreur SMTP", "Problème d'envoi : " + response);
            socket->close();
            this->deleteLater();
        }
    }
    response = "";
}

Smtp::~Smtp() {
    if (t) delete t;
}
