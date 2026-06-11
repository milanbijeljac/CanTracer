#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_serialPort = new Core::SerialPort(this);
    m_parser     = new Core::CanFrameParser(this);
    m_model      = new CanTableModel(this);

    // Wire: serial → parser → model
    connect(m_serialPort, &Core::SerialPort::dataReceived,
            m_parser, &Core::CanFrameParser::processBytes);
    connect(m_parser, &Core::CanFrameParser::frameReceived,
            m_model, &CanTableModel::updateMessage);

    // Serial status
    connect(m_serialPort, &Core::SerialPort::connected,
            this, &MainWindow::onSerialConnected);
    connect(m_serialPort, &Core::SerialPort::disconnected,
            this, &MainWindow::onSerialDisconnected);
    connect(m_serialPort, &Core::SerialPort::errorOccurred,
            this, &MainWindow::onSerialError);

    setupUi();
    refreshPorts();
}

void MainWindow::setupUi()
{
    setWindowTitle("CanTracer");
    resize(800, 500);

    // Toolbar
    auto* toolbar = new QToolBar("Connection", this);
    toolbar->setMovable(false);
    addToolBar(toolbar);

    m_portCombo = new QComboBox(this);
    m_portCombo->setMinimumWidth(150);
    toolbar->addWidget(m_portCombo);

    m_refreshBtn = new QPushButton("Refresh", this);
    toolbar->addWidget(m_refreshBtn);

    m_connectBtn = new QPushButton("Connect", this);
    toolbar->addWidget(m_connectBtn);

    toolbar->addSeparator();

    m_statusLabel = new QLabel("Disconnected", this);
    toolbar->addWidget(m_statusLabel);

    // Table
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setCentralWidget(m_tableView);

    // Signals
    connect(m_connectBtn, &QPushButton::clicked,
            this, &MainWindow::onConnectClicked);
    connect(m_refreshBtn, &QPushButton::clicked,
            this, &MainWindow::onRefreshPortsClicked);
}

void MainWindow::refreshPorts()
{
    m_portCombo->clear();
    const auto ports = Core::SerialPort::availablePorts();
    for (const auto& info : ports)
        m_portCombo->addItem(info.portName(), info.portName());
}

void MainWindow::onConnectClicked()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
    else
    {
        const QString port = m_portCombo->currentData().toString();
        if (!port.isEmpty())
            m_serialPort->open(port);
    }
}

void MainWindow::onRefreshPortsClicked()
{
    refreshPorts();
}

void MainWindow::onSerialConnected()
{
    m_statusLabel->setText("Connected: " + m_serialPort->portName());
    m_connectBtn->setText("Disconnect");
    m_model->clear();
    m_parser->reset();
}

void MainWindow::onSerialDisconnected()
{
    m_statusLabel->setText("Disconnected");
    m_connectBtn->setText("Connect");
}

void MainWindow::onSerialError(const QString& error)
{
    m_statusLabel->setText("Error: " + error);
}
