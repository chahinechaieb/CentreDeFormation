#ifndef SMTP_H
#define SMTP_H

#include <QObject>
#include <QtNetwork/QSslSocket>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>

class Smtp : public QObject
{
    Q_OBJECT
public:
    Smtp(const QString &user, const QString &pass, const QString &host, int port = 465);
    ~Smtp();
    void sendMail(const QString &from, const QString &to, const QString &subject, const QString &body);

signals:
    void status(const QString &);

private slots:
    void stateChanged(QAbstractSocket::SocketState socketState);
    void errorReceived(QAbstractSocket::SocketError socketError);
    void disconnected();
    void connected();
    void readyRead();

private:
    QString message, from, rcpt, response, user, pass, host;
    int port, state;
    QTextStream *t;
    QSslSocket *socket;
    enum states{Tls, HandShake ,Auth, User, Pass, Rcpt, Mail, Data, Init, Body, Quit, Close};
};
#endif // SMTP_H
