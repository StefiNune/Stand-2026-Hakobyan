#include "mainwindow.h"
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Qt Задачи №1-4 (COM-порт + JSON)");
    resize(800, 700);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // ЗАДАЧА №1: QComboBox с COM-портами
    QHBoxLayout *portLayout = new QHBoxLayout();
    portLayout->addWidget(new QLabel("Порт:"));
    
    portCombo = new QComboBox();
    portCombo->setMinimumWidth(150);
    portLayout->addWidget(portCombo);

    refreshBtn = new QPushButton("Обновить");
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshPorts);
    portLayout->addWidget(refreshBtn);

    connectBtn = new QPushButton("Подключиться");
    connect(connectBtn, &QPushButton::clicked, this, &MainWindow::toggleConnection);
    portLayout->addWidget(connectBtn);

    portLayout->addStretch();
    mainLayout->addLayout(portLayout);

    // ЗАДАЧА №2: Отправка сообщений в COM-порт
    QHBoxLayout *sendLayout = new QHBoxLayout();
    sendLayout->addWidget(new QLabel("Отправить:"));
    
    messageEdit = new QLineEdit();
    messageEdit->setPlaceholderText("Введите сообщение...");
    sendLayout->addWidget(messageEdit);

    sendBtn = new QPushButton("Отправить");
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::sendMessage);
    sendLayout->addWidget(sendBtn);

    sendLayout->addStretch();
    mainLayout->addLayout(sendLayout);

    // ЗАДАЧА №3: QTextEdit + QTimer для чтения
    textEdit = new QTextEdit();
    textEdit->setFont(QFont("Courier New", 10));
    textEdit->setReadOnly(true);
    textEdit->setPlaceholderText("Данные из COM-порта будут появляться здесь...");
    mainLayout->addWidget(textEdit);


    // ЗАДАЧА №4: 5 QLineEdit + JSON
    QGroupBox *jsonGroup = new QGroupBox("Задача №4: Создание JSON");
    QVBoxLayout *jsonLayout = new QVBoxLayout(jsonGroup);

    // Поля ввода
    QHBoxLayout *fieldsLayout = new QHBoxLayout();
    
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Имя");
    fieldsLayout->addWidget(nameEdit);
    
    ageEdit = new QLineEdit();
    ageEdit->setPlaceholderText("Возраст");
    fieldsLayout->addWidget(ageEdit);
    
    cityEdit = new QLineEdit();
    cityEdit->setPlaceholderText("Город");
    fieldsLayout->addWidget(cityEdit);
    
    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("Email");
    fieldsLayout->addWidget(emailEdit);
    
    phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("Телефон");
    fieldsLayout->addWidget(phoneEdit);
    
    jsonLayout->addLayout(fieldsLayout);

    // Кнопка и вывод
    QHBoxLayout *jsonBtnLayout = new QHBoxLayout();
    jsonBtn = new QPushButton("Создать JSON");
    connect(jsonBtn, &QPushButton::clicked, this, &MainWindow::createJson);
    jsonBtnLayout->addWidget(jsonBtn);
    jsonBtnLayout->addStretch();
    jsonLayout->addLayout(jsonBtnLayout);

    jsonOutput = new QTextEdit();
    jsonOutput->setFont(QFont("Courier New", 10));
    jsonOutput->setReadOnly(true);
    jsonOutput->setMaximumHeight(100);
    jsonOutput->setPlaceholderText("JSON будет появляться здесь...");
    jsonLayout->addWidget(jsonOutput);

    mainLayout->addWidget(jsonGroup);

    // Строка статуса
    statusLabel = new QLabel("Статус: отключено");
    mainLayout->addWidget(statusLabel);

    // Таймер и порт
    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, &QTimer::timeout, this, &MainWindow::readFromPort);

    serial = new QSerialPort(this);
    refreshPorts();
}

MainWindow::~MainWindow() {}

// ЗАДАЧА №1: Обновление списка портов
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

// ЗАДАЧА №2: Отправка сообщения
void MainWindow::sendMessage() {
    if (!serial->isOpen()) {
        QMessageBox::warning(this, "Ошибка", "Сначала подключитесь к порту!");
        return;
    }
    
    QString message = messageEdit->text();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите сообщение для отправки!");
        return;
    }
    
    QByteArray data = message.toUtf8();
    qint64 bytesWritten = serial->write(data);
    
    if (bytesWritten == -1) {
        QMessageBox::critical(this, "Ошибка", "Не удалось отправить сообщение!");
    } else {
        statusLabel->setText("Статус: отправлено " + QString::number(bytesWritten) + " байт");
        appendData(">>> " + message + "\n");
        messageEdit->clear();
    }
}

// ЗАДАЧА №3: Подключение/отключение + чтение
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

    if (serial->open(QIODevice::ReadWrite)) {
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

// ЗАДАЧА №4: Создание JSON
void MainWindow::createJson() {
    QJsonObject jsonObject;
    jsonObject["name"] = nameEdit->text();
    jsonObject["age"] = ageEdit->text().toInt();
    jsonObject["city"] = cityEdit->text();
    jsonObject["email"] = emailEdit->text();
    jsonObject["phone"] = phoneEdit->text();
    jsonObject["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(jsonObject);
    QString jsonString = doc.toJson(QJsonDocument::Indented);

    jsonOutput->setText(jsonString);
    appendData("\n--- JSON создан ---\n" + jsonString + "\n");
}