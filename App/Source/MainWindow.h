#pragma once

#include <QMainWindow>
#include <QTableView>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

#include "CanTableModel.h"
#include "Core/SerialPort.h"
#include "Core/CanFrameParser.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onConnectClicked();
    void onRefreshPortsClicked();
    void onSerialConnected();
    void onSerialDisconnected();
    void onSerialError(const QString& error);

private:
    void setupUi();
    void refreshPorts();

    // UI
    QTableView*   m_tableView = nullptr;
    QComboBox*    m_portCombo = nullptr;
    QPushButton*  m_connectBtn = nullptr;
    QPushButton*  m_refreshBtn = nullptr;
    QLabel*       m_statusLabel = nullptr;

    // Logic
    CanTableModel*        m_model = nullptr;
    Core::SerialPort*     m_serialPort = nullptr;
    Core::CanFrameParser* m_parser = nullptr;
};
