#include <QCoreApplication>
#include "testserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TestServer s;

    return a.exec();
}
