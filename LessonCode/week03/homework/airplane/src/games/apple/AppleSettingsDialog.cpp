#include "AppleSettingsDialog.h"

AppleSettingsDialog::AppleSettingsDialog(int currentLevel, int successCount, int failCount, int totalPresses, int targetSuccess, int maxApples, QWidget* parent) 
    : QDialog(parent)
{
    setWindowTitle(" Game Settings");

    spinLevel = new QSpinBox(this);
    spinLevel->setRange(1, 100);
    spinLevel->setValue(currentLevel);

    spinSuccessCount = new QSpinBox(this);
    spinSuccessCount->setRange(0, 99999);
    spinSuccessCount->setValue(successCount);

    spinFailCount = new QSpinBox(this);
    spinFailCount->setRange(0, 99999);
    spinFailCount->setValue(failCount);

    spinTotalPresses = new QSpinBox(this);
    spinTotalPresses->setRange(0, 99999);
    spinTotalPresses->setValue(totalPresses);

    spinTargetSuccess = new QSpinBox(this);
    spinTargetSuccess->setRange(1, 99999);
    spinTargetSuccess->setValue(targetSuccess);

    spinMaxApples = new QSpinBox(this);
    spinMaxApples->setRange(1, 100);
    spinMaxApples->setValue(maxApples);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Level:", spinLevel);
    formLayout->addRow("Success Count:", spinSuccessCount);
    formLayout->addRow("Fail Count:", spinFailCount);
    formLayout->addRow("Total Presses:", spinTotalPresses);
    formLayout->addRow("Target Success:", spinTargetSuccess);
    formLayout->addRow("Max Apples:", spinMaxApples);

    QPushButton* btnOk = new QPushButton("OK", this);
    QPushButton* btnCancel = new QPushButton("Cancel", this);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);

    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

int AppleSettingsDialog::getLevel() const { return spinLevel->value(); }
int AppleSettingsDialog::getSuccessCount() const { return spinSuccessCount->value(); }
int AppleSettingsDialog::getFailCount() const { return spinFailCount->value(); }
int AppleSettingsDialog::getTotalPresses() const { return spinTotalPresses->value(); }
int AppleSettingsDialog::getTargetSuccess() const { return spinTargetSuccess->value(); }
int AppleSettingsDialog::getMaxApples() const { return spinMaxApples->value(); }
