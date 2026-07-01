#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("COM-порт монитор (Задача №3)");
    resize(600, 500);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Панель управления
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    controlLayout->addWidget(new QLabel("Порт:"));
    portCombo = new QComboBox();
    portCombo->setMinimumWidth(150);
    controlLayout->addWidget(portCombo);

    refreshBtn = new QPushButton("Обновить");
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshPorts);
    controlLayout->addWidget(refreshBtn);

    connectBtn = new QPushButton("Подключиться");
    connect(connectBtn, &QPushButton::clicked, this, &MainWindow::toggleConnection);
    controlLayout->addWidget(connectBtn);

    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);

    // Текстовое поле
    textEdit = new QTextEdit();
    textEdit->setFont(QFont("Courier New", 10));
    textEdit->setReadOnly(true);
    mainLayout->addWidget(textEdit);

    // Статус
    statusLabel = new QLabel("Статус: отключено");
    mainLayout->addWidget(statusLabel);

    // Таймер
    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, &QTimer::timeout, this, &MainWindow::readFromPort);

    // Serial port
    serial = new QSerialPort(this);
    refreshPorts();
}

MainWindow::~MainWindow() {}

void MainWindow::refreshPorts() {
    portCombo->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    if (ports.isEmpty()) {
        portCombo->addItem("Нет доступных портов");
    } else {
        for (const auto &port : ports) {
            portCombo->addItem(port.portName() + " - " + port.description());
        }
    }
}

void MainWindow::toggleConnection() {
    if (serial->isOpen()) {
        disconnectPort();
    } else {
        connectPort();
    }
}

void MainWindow::connectPort() {
    if (portCombo->currentText() == "Нет доступных портов") {
        statusLabel->setText("Статус: нет доступных портов");
        return;
    }

    QString portName = portCombo->currentText().split(" ").first();
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadOnly)) {
        timer->start();
        connectBtn->setText("Отключиться");
        statusLabel->setText("Статус: подключено к " + portName);
        appendData("--- Подключено к " + portName + " ---\n");
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть порт " + portName);
        statusLabel->setText("Статус: ошибка подключения");
    }
}

void MainWindow::disconnectPort() {
    timer->stop();
    if (serial->isOpen()) {
        serial->close();
    }
    connectBtn->setText("Подключиться");
    statusLabel->setText("Статус: отключено");
    appendData("--- Отключено от порта ---\n");
}

void MainWindow::readFromPort() {
    if (!serial->isOpen()) return;

    QByteArray data = serial->readAll();
    if (!data.isEmpty()) {
        QString text = QString::fromUtf8(data);
        appendData(text);
    }
}

void MainWindow::appendData(const QString &text) {
    textEdit->insertPlainText(text);
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);
}