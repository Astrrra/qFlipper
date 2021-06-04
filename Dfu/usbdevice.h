#ifndef USBDEVICE_H
#define USBDEVICE_H

#include <QObject>

#include "usbbackend.h"
#include "usbdeviceparams.h"

/*
 * Assumptions made:
 * 1) Single device descriptor (as per DFU protocol specification)
 * 2) Single configuration descriptor (see above)
 * 3) Single interface descriptor (see above)
 * 4) Multiple alternate configurations
 * 5) For now, only control transfers are supported
 *
 * Therefore, this class only provides access to alternate configurations
 * of the first interface of the first configuration.
*/

/*
 * TODO: Refactor this mess into something more logical.
 *
 * Suggested structure:
 * class USBInterfaceAltSetting {
 *      controlTransfer()....
 * }
 * class USBInterface {
 *      getAlternateSettings() ...
 *      QList<USBInterfaceAltSetting> ...
 * }
 * class USBConfiguration {
 *      getInterfaces() ...
 *      QList<USBInterface> ...
 * }
 *
 * class USBDevice {
 *      open(), close(), ...
 *      getConfigurations() ...
 *      QList<USBConfiguration> ...
 *
 *      Backend backend ...
 * }
 *
 * All this functionality is not needed right now, but might come in handy in the future.
 * Plus, it looks much better.
 *
 */

class QMutex;

class USBDEviceDetector : public QObject
{
    Q_OBJECT

public:
    USBDEviceDetector(QObject *parent = nullptr);
    void registerHotplugEvent(const QList<USBDeviceParams> &paramList);

signals:
    void devicePluggedIn(const USBDeviceParams&);
    void deviceUnplugged(const USBDeviceParams&);
};

class USBDevice : public QObject
{
    Q_OBJECT

    friend class USBDEviceDetector;

public:
    enum EndpointDirection {
        ENDPOINT_IN = 0x80,
        ENDPOINT_OUT = 0x00,
    };

    enum RequestType {
        REQUEST_TYPE_STANDARD = 0x00,
        REQUEST_TYPE_CLASS = (0x01 << 5),
        REQUEST_TYPE_VENDOR = (0x02 << 5),
        REQUEST_TYPE_RESERVED = (0x03 << 5)
    };

    enum RequestRecipient {
        RECIPIENT_DEVICE = 0x00,
        RECIPIENT_INTERFACE = 0x01,
        RECIPIENT_ENDPOINT = 0x02,
        RECIPIENT_OTHER = 0x03
    };

    USBDevice(const USBDeviceParams &parameters, QObject *parent = nullptr);
    virtual ~USBDevice();

    bool open();
    void close();

    bool claimInterface(int interfaceNum);
    bool releaseInterface(int interfaceNum);
    bool setInterfaceAltSetting(int interfaceNum, uint8_t alt);

    bool controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &data);
    QByteArray controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length);

    // TODO: Refactor this part first
    QByteArray extraInterfaceDescriptor();
    QByteArray stringInterfaceDescriptor(int interfaceNum);

    static USBDEviceDetector *detector();

private:
    static USBBackend &backend();
    static QMutex &backendMutex();

    USBBackend::DeviceHandle *m_handle = nullptr;
    bool m_isOpen = false;
};

#endif // USBDEVICE_H