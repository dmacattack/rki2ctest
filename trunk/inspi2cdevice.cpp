#include "inspi2cdevice.hpp"
#include <QDebug>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

extern "C"
{
    #include "include/smbus.h"
}

using INSP_I2C_DEVICE::eERROR_NONE;
using INSP_I2C_DEVICE::eERROR_FILE_DESCRIPTOR;
using INSP_I2C_DEVICE::eERROR_DEVICE_CAPABILITY;
using INSP_I2C_DEVICE::eMODE_READ_BYTE;
using INSP_I2C_DEVICE::eMODE_WRITE_BYTE;
using INSP_I2C_DEVICE::eMODE_READ_WORD;
using INSP_I2C_DEVICE::eMODE_WRITE_WORD;

/**
 * @brief InspI2CDevice::InspI2CDevice - ctor
 * @param busNum
 * @param i2cAddr
 */
InspI2CDevice::InspI2CDevice(int busNum, int i2cAddr)
: mBusNum(busNum)
, mDeviceAddr(i2cAddr)
, mI2CDevicePath("")
, mMode(INSP_I2C_DEVICE::eMODE_READ_BYTE)
{
    // using the rockchip standardized path
    mI2CDevicePath = QString("/dev/i2c-%1").arg(mBusNum);
}

/**
 * @brief InspI2CDevice::readByte - read a byte from the i2c device
 * @param addr - register address
 * @returns > 0 ==> value, < 0 ==> error
 */
int InspI2CDevice::readByte(int addr)
{
    int resp = eERROR_NONE;
    mMode = eMODE_READ_BYTE;

    int fd = openI2CDevice();
    if (fd < 0)
    {
        resp = eERROR_FILE_DESCRIPTOR;
    }
    else
    {
        // read data

        // close the file descriptor
        close(fd);
    }

    Q_UNUSED(addr);
    return resp;
}

/**
 * @brief InspI2CDevice::writeByte - write a byte to the i2c device
 * @param addr - register address
 * @param val - register value
 * @returns > 0 ==> value, < 0 ==> error
 */
int InspI2CDevice::writeByte(int addr, int val)
{
    Q_UNUSED(addr);
    Q_UNUSED(val);
    int resp = eERROR_NONE;
    mMode = eMODE_WRITE_BYTE;
    return resp;
}

/**
 * @brief InspI2CDevice::readWord - read a word from the i2c device
 * @param addr - register address
 * @returns > 0 ==> value, < 0 ==> error
 */
int InspI2CDevice::readWord(int addr)
{
    int resp = eERROR_NONE;
    mMode = eMODE_READ_WORD;

    int fd = openI2CDevice();
    if (fd < 0)
    {
        resp = eERROR_FILE_DESCRIPTOR;
    }
    else
    {
        // check the capability
        resp = checkCapability(fd);
        if (resp == eERROR_NONE)
        {
            // set the slave address
            resp = setSlaveAddress(fd);
            if (resp == eERROR_NONE)
            {
                // do the read
                resp = i2c_smbus_read_word_data(fd, addr);
            }
        }

        // close the file descriptor
        close(fd);
    }

    return resp;
}

/**
 * @brief InspI2CDevice::writeWord - write a word to the i2c device
 * @param addr - register address
 * @param val - register value
 * @returns > 0 ==> value, < 0 ==> error
 */
int InspI2CDevice::writeWord(int addr, int val)
{
    int resp = eERROR_NONE;
    Q_UNUSED(addr);
    Q_UNUSED(val);
    mMode = eMODE_WRITE_WORD;
    return resp;
}

/**
 * @brief InspI2CDevice::openI2CDevice - open the i2c device
 * @returns file descriptor
 */
int InspI2CDevice::openI2CDevice()
{
    int fd = open(mI2CDevicePath.toStdString().c_str(), O_RDWR);
    return fd;
}

/**
 * @brief InspI2CDevice::checkCapability - ensure the capability of the device
 * supports the requested action
 * @param fd - file descriptor
 * @returns error code
 */
int InspI2CDevice::checkCapability(int fd)
{
    int err = eERROR_NONE;
    unsigned long funcs = 0;

    /* check adapter functionality */
    if (ioctl(fd, I2C_FUNCS, &funcs) < 0)
    {
        fprintf(stderr, "Error: Could not get the adapter functionality matrix: %s\n", strerror(errno));
        err = eERROR_DEVICE_CAPABILITY;
    }
    else
    {
        // hardcoded for the word size
        switch (mMode)
        {
        case eMODE_READ_BYTE:
            if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE_DATA))
            {
                fprintf(stderr, "Error: Adapter does not have SMBus read byte capability");
                err = eERROR_DEVICE_CAPABILITY;
            }
            break;
        case eMODE_WRITE_BYTE:

            break;
        case eMODE_READ_WORD:
            if (!(funcs & I2C_FUNC_SMBUS_READ_WORD_DATA))
            {
                fprintf(stderr, "Error: Adapter does not have SMBus read word capability");
                err = eERROR_DEVICE_CAPABILITY;
            }
            break;
        case eMODE_WRITE_WORD:

            break;
        }
    }

    return err;
}

/**
 * @brief InspI2CDevice::setSlaveAddress - set the i2c slave address
 * @param fd - file descriptor
 * @returns error code
 */
int InspI2CDevice::setSlaveAddress(int fd)
{
    int err = eERROR_NONE;

    if (ioctl(fd, I2C_SLAVE, mDeviceAddr) < 0)
    {
        fprintf(stderr, "Error: Could not set address to 0x%02x: %s\n", mDeviceAddr, strerror(errno));
        err = -errno;
    }

    return err;
}
