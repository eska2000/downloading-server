#include "testserver.h"

TestServer::TestServer(QObject *parent) : QObject(parent)
{
    qDebug()<<"Starting server";

    connect(&server, SIGNAL(newConnection()), this, SLOT (acceptConnection()));

    if (!server.listen(QHostAddress::Any, 33333)) {
        qWarning() << "Unable to start server: "<<server.errorString();
        return;
    }
    qDebug() << "Server is running";
}

void TestServer::acceptConnection()
{
    qDebug()<< "New connection";
    tcpServerConnection = server.nextPendingConnection();
    connect(tcpServerConnection,SIGNAL(readyRead()), this, SLOT(readFromClient()));
    connect(tcpServerConnection,SIGNAL(disconnected()),this,SLOT(disconnected()));
}

void TestServer::readFromClient()
{
    QByteArray bytes = tcpServerConnection->readAll();

    if (QString(bytes) == "file" && protocolState == Waiting) {
        protocolState = FileName;
        tcpServerConnection->write("#server/file/name");
    }
    else if (protocolState == FileName) {
        fileName = QString(bytes);
        protocolState = FileSize;
        tcpServerConnection->write("#server/file/size");
    }
    else if (protocolState == FileSize) {
        fileSize = QString(bytes).toLongLong();
        protocolState = FileData;
        loadedFile.setFileName(fileName);
        totalbytes = 0;
        if (loadedFile.open(QIODevice::WriteOnly)) {
            qDebug()<<QString("File %1 uploading...").arg(fileName);
            tcpServerConnection->write("#server/file/data");
        }
        else {
            qWarning()<<"Wrong filename";
            protocolState = Waiting;
            tcpServerConnection->close();
        }
    }
    else if (protocolState == FileData) {
        loadedFile.write(bytes);
        totalbytes+=(qlonglong)bytes.size();
        if (totalbytes == fileSize) {
            qDebug()<<"Uploading finished";
            loadedFile.close();
            protocolState = Waiting;
            tcpServerConnection->close();
        }
    }
}

void TestServer::disconnected()
{
    if (protocolState != Waiting) {
        qWarning()<<"Something wrong!";
        protocolState = Waiting;
    }
    if (loadedFile.isOpen()) {
        loadedFile.close();
        loadedFile.remove();
    }
}
