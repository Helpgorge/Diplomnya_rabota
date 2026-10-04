#pragma once

#include <QDialog>

class SettingsView : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsView(QWidget* parent = nullptr);
    ~SettingsView();

private slots:
    void onApplyClicked();

};