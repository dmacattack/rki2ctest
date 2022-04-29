#include <QCoreApplication>
#include <QDebug>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <QTimer>
#include <QObject>
#include <QDateTime>
#include <unistd.h>
#include <fcntl.h>

extern "C"
{
    #include "include/smbus.h"
}

int i2cbus = 1;                  // i2c bus number
int address = 0x08;              // i2c address
int daddress = 0xDC;             // data address
int datasize = I2C_SMBUS_WORD_DATA;  // size of data to read
#define MISSING_FUNC_FMT	"Error: Adapter does not have %s capability\n"

static int check_funcs(int file, int size, int daddress, int pec)
{
    unsigned long funcs;

    /* check adapter functionality */
    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        fprintf(stderr, "Error: Could not get the adapter "
            "functionality matrix: %s\n", strerror(errno));
        return -1;
    }

    switch (size) {
    case I2C_SMBUS_BYTE:
        if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE)) {
            fprintf(stderr, MISSING_FUNC_FMT, "SMBus receive byte");
            return -1;
        }
        if (daddress >= 0
         && !(funcs & I2C_FUNC_SMBUS_WRITE_BYTE)) {
            fprintf(stderr, MISSING_FUNC_FMT, "SMBus send byte");
            return -1;
        }
        break;

    case I2C_SMBUS_BYTE_DATA:
        if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE_DATA)) {
            fprintf(stderr, MISSING_FUNC_FMT, "SMBus read byte");
            return -1;
        }
        break;

    case I2C_SMBUS_WORD_DATA:
        if (!(funcs & I2C_FUNC_SMBUS_READ_WORD_DATA)) {
            fprintf(stderr, MISSING_FUNC_FMT, "SMBus read word");
            return -1;
        }
        break;
    }

    if (pec
     && !(funcs & (I2C_FUNC_SMBUS_PEC | I2C_FUNC_I2C))) {
        fprintf(stderr, "Warning: Adapter does "
            "not seem to support PEC\n");
    }

    return 0;
}

int open_i2c_dev(int i2cbus, char *filename, size_t size, int quiet)
{
    int file;

    snprintf(filename, size, "/dev/i2c/%d", i2cbus);
    filename[size - 1] = '\0';
    file = open(filename, O_RDWR);

    if (file < 0 && (errno == ENOENT || errno == ENOTDIR)) {
        sprintf(filename, "/dev/i2c-%d", i2cbus);
        file = open(filename, O_RDWR);
    }

    if (file < 0 && !quiet) {
        if (errno == ENOENT) {
            fprintf(stderr, "Error: Could not open file "
                "`/dev/i2c-%d' or `/dev/i2c/%d': %s\n",
                i2cbus, i2cbus, strerror(ENOENT));
        } else {
            fprintf(stderr, "Error: Could not open file "
                "`%s': %s\n", filename, strerror(errno));
            if (errno == EACCES)
                fprintf(stderr, "Run as root?\n");
        }
    }

    return file;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "start reading data from the roloc ?";
    QTimer *pTimer = new QTimer();

    QObject::connect(pTimer, &QTimer::timeout, [&]()
    {
        // do stuff

        char filename[20];
        int fd = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);

        qDebug("filename = %s . fd =  %d", filename, fd);

        if (fd < 0 || check_funcs(file, size, daddress, pec) )
        {
            qFatal("couldnt open the i2cdevice");
        }

        int res = i2c_smbus_read_word_data(fd, 220);

        close(fd);

        if (res < 0)
        {
            qDebug("error %d %s", res, strerror(errno) );
        }
        else
        {
            qDebug() << "result = " << res;
        }



    });

    pTimer->start(2000);


    return a.exec();
}
