#pragma once

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Core {

class SerialPort : public QObject
{
    Q_OBJECT

public:
    explicit SerialPort(QObject* parent = nullptr);
    ~SerialPort();

    bool open(const QString& portName, qint32 baudRate = 115200);
    void close();
    bool isOpen() const;
    QString portName() const;

    static QList<QSerialPortInfo> availablePorts();

signals:
    void dataReceived(const QByteArray& data);
    void errorOccurred(const QString& error);
    void connected();
    void disconnected();

private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serial;
};

} // namespace Core
