#include "SerialPort.h"

namespace Core {

SerialPort::SerialPort(QObject* parent)
    : QObject(parent)
{
    connect(&m_serial, &QSerialPort::readyRead,
            this, &SerialPort::onReadyRead);
    connect(&m_serial, &QSerialPort::errorOccurred,
            this, &SerialPort::onErrorOccurred);
}

SerialPort::~SerialPort()
{
    close();
}

bool SerialPort::open(const QString& portName, qint32 baudRate)
{
    if (m_serial.isOpen())
        close();

    m_serial.setPortName(portName);
    m_serial.setBaudRate(baudRate);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serial.open(QIODevice::ReadOnly))
    {
        emit errorOccurred(m_serial.errorString());
        return false;
    }

    emit connected();
    return true;
}

void SerialPort::close()
{
    if (m_serial.isOpen())
    {
        m_serial.close();
        emit disconnected();
    }
}

bool SerialPort::isOpen() const
{
    return m_serial.isOpen();
}

QString SerialPort::portName() const
{
    return m_serial.portName();
}

QList<QSerialPortInfo> SerialPort::availablePorts()
{
    return QSerialPortInfo::availablePorts();
}

void SerialPort::onReadyRead()
{
    const QByteArray data = m_serial.readAll();
    if (!data.isEmpty())
        emit dataReceived(data);
}

void SerialPort::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError)
        return;

    if (error == QSerialPort::ResourceError)
    {
        close();
    }

    emit errorOccurred(m_serial.errorString());
}

} // namespace Core
