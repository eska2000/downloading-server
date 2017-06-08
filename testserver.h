#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QtDebug>

class TestServer : public QObject
{
    Q_OBJECT
public:
    TestServer(QObject *parent=0);

private:
    enum protocol {Waiting,FileName, FileSize, FileData};
    protocol protocolState = Waiting;
    QTcpServer server;
    QTcpSocket *tcpServerConnection;
    QString fileName;
    qint64 fileSize, totalbytes;
    QFile loadedFile;

private slots:
    void acceptConnection();
    void readFromClient();
    void disconnected();

};

#endif // TESTSERVER_H
