#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QSerialPort>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Задача №1: обновление портов
    void refreshPorts();
    
    // Задача №2: отправка сообщения
    void sendMessage();
    
    // Задача №3: подключение/отключение + чтение
    void toggleConnection();
    void connectPort();
    void disconnectPort();
    void readFromPort();
    void appendData(const QString &text);
    
    // Задача №4: создание JSON
    void createJson();

private:
    // Общие элементы
    QComboBox *portCombo;
    QPushButton *refreshBtn;
    QPushButton *connectBtn;
    QTextEdit *textEdit;
    QLabel *statusLabel;
    QTimer *timer;
    QSerialPort *serial;
    
    // Задача №2: отправка
    QLineEdit *messageEdit;
    QPushButton *sendBtn;
    
    // Задача №4: поля для ввода + кнопка
    QLineEdit *nameEdit;
    QLineEdit *ageEdit;
    QLineEdit *cityEdit;
    QLineEdit *emailEdit;
    QLineEdit *phoneEdit;
    QPushButton *jsonBtn;
    QTextEdit *jsonOutput;
};

#endif // MAINWINDOW_H