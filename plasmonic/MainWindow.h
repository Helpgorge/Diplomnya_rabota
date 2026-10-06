#pragma once

#include <QMainWindow>
#include <QProgressDialog>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void showConnectionProgress();
    void setConnectionStatus(const QString& text);
    void closeConnectionProgress();

signals:
    void connectRequested();

private slots:
    void onConnectClicked();
    void onSettingsClicked();

private:
    QProgressDialog* connectionDialog = nullptr;
};