#pragma once

#include <QDialog>

class QLineEdit;
class QSpinBox;


class SettingsView : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsView(QWidget* parent = nullptr);
    ~SettingsView();

private slots:
    void onApplyClicked();

private:
    QLineEdit* ipEdit;
    QSpinBox* portEdit;
};