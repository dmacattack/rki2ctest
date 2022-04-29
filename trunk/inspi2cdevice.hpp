#ifndef INSPI2CDEVICE_HPP
#define INSPI2CDEVICE_HPP

#include <QString>

namespace INSP_I2C_DEVICE
{
    enum eI2C_ERROR
    {
        eERROR_NONE = 0,
        eERROR_FILE_DESCRIPTOR = -ENODEV,
        eERROR_DEVICE_CAPABILITY = -1000,
    };

    enum eI2C_MODE
    {
        eMODE_READ_BYTE,
        eMODE_WRITE_BYTE,
        eMODE_READ_WORD,
        eMODE_WRITE_WORD
    };
}

/**
 * @brief The InspI2CDevice class - object to provide i2c access using
 * the libi2c library that i2cget/set uses.
 */
class InspI2CDevice
{
public:
    /**
     * @brief InspI2CDevice - construct the i2c device
     * @param busNum - i2c bus number
     * @param i2cAddr - i2c device address
     */
    InspI2CDevice(int busNum, int i2cAddr);

    /**
     * @brief readByte - read a byte from the i2c device
     * @param addr - register address
     * @returns > 0 ==> value, < 0 ==> error
     */
    int readByte(int addr);

    /**
     * @brief writeByte - write a byte to the i2c device
     * @param addr - register address
     * @param val - register value
     * @returns > 0 ==> value, < 0 ==> error
     */
    int writeByte(int addr, int val);

    /**
     * @brief readWord - read a word from the i2c device
     * @param addr - register address
     * @returns > 0 ==> value, < 0 ==> error
     */
    int readWord(int addr);

    /**
     * @brief writeWord - write a word to the i2c device
     * @param addr - register address
     * @param val - register value
     * @returns > 0 ==> value, < 0 ==> error
     */
    int writeWord(int addr, int val);

    /**
     * @brief InspI2CDevice::getErrString - convert the error code to a string
     * @param errCode - InspI2CDevice returned error code
     * @returns string of the error code
     */
    static QString getErrString(int errCode);

private:

    /**
     * @brief openI2CDevice - open the i2c device
     * @returns file descriptor or error
     */
    int openI2CDevice();

    /**
     * @brief checkCapability - ensure the capability of the device
     * supports the requested action
     * @param fd - file descriptor
     * @returns error code
     */
    int checkCapability(int fd);

    /**
     * @brief setSlaveAddress - set the i2c slave address
     * @param fd - file descriptor
     * @returns error code
     */
    int setSlaveAddress(int fd);

private:
    int mBusNum;
    int mDeviceAddr;
    QString mI2CDevicePath;
    INSP_I2C_DEVICE::eI2C_MODE mMode;

};

/**
 * @brief InspI2CDevice::getErrString - convert the error code to a string
 * @param errCode - InspI2CDevice returned error code
 * @returns string of the error code
 */
inline QString InspI2CDevice::getErrString(int errCode)
{
    return (errCode >= INSP_I2C_DEVICE::eERROR_NONE              ? "NO ERROR"        :
           (errCode == INSP_I2C_DEVICE::eERROR_FILE_DESCRIPTOR   ? "FILE DESCRIPTOR" :
           (errCode == INSP_I2C_DEVICE::eERROR_DEVICE_CAPABILITY ? "NO CAPABILITY"   : strerror(-errCode) )));
}

#endif // INSPI2CDEVICE_HPP
