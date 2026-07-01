#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QSerialPort>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshPorts();
    void toggleConnection();
    void connectPort();
    void disconnectPort();
    void readFromPort();
    void appendData(const QString &text);

private:
    QComboBox *portCombo;
    QPushButton *refreshBtn;
    QPushButton *connectBtn;
    QTextEdit *textEdit;
    QLabel *statusLabel;
    QTimer *timer;
    QSerialPort *serial;
};

#endif // MAINWINDOW_H