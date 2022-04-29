#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QObject>
#include <QDateTime>
#include "inspi2cdevice.hpp"

InspI2CDevice *pI2C = NULL;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "start reading data from the roloc ?";
    QTimer *pTimer = new QTimer();
    pI2C = new InspI2CDevice(1, 0x08);

    QObject::connect(pTimer, &QTimer::timeout, [&]()
    {
        auto b = QDateTime::currentMSecsSinceEpoch();
        int res = pI2C->readWord(0xDC);
        auto a = QDateTime::currentMSecsSinceEpoch();
        auto diff = a - b;

        if (res < 0)
        {
            qDebug("xxx time to read id = %llu. Error = %s", diff, InspI2CDevice::getErrString(res).toStdString().c_str());
        }
        else
        {
            qDebug("xxx time to read id = %llu. ID = 0x%0X", diff, res);
        }
    });

    pTimer->start(2000);

    return a.exec();
}
