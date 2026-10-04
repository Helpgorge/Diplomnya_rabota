#include "settings_view.h"

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>

SettingsView::SettingsView(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Настройки");
    resize(600, 400);

    // Группа сетевых настроек
    QGroupBox* networkGroup = new QGroupBox("Сеть", this);

    // Кнопки
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    QPushButton* saveButton = new QPushButton("Сохранить", this);

    // Горизонтальное расположение кнопок
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addStretch();

    // IP-адрес
    QLineEdit* ipEdit = new QLineEdit(networkGroup);
    ipEdit->setMaximumWidth(120);
    ipEdit->setText("127.0.0.1");

    // Порт
    QSpinBox* portEdit = new QSpinBox(networkGroup);
    portEdit->setRange(1, 65535);
    portEdit->setValue(5000);

    // Параметры внутри рамки
    QFormLayout* networkLayout = new QFormLayout;
    networkLayout->addRow("IP-адрес:", ipEdit);
    networkLayout->addRow("Порт:", portEdit);

    networkGroup->setLayout(networkLayout);

    // Основная компоновка окна
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(networkGroup);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    connect(
        cancelButton,
        &QPushButton::clicked,
        this,
        &QDialog::reject
    );
    
}

SettingsView::~SettingsView()
{
}

void SettingsView::onApplyClicked()
{

}