#ifndef APPLESETTINGSDIALOG_H
#define APPLESETTINGSDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>

class AppleSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AppleSettingsDialog(int currentLevel, int successCount, int failCount, int totalPresses, int targetSuccess, int maxApples, QWidget* parent = nullptr);

    int getLevel() const;
    int getSuccessCount() const;
    int getFailCount() const;
    int getTotalPresses() const;
    int getTargetSuccess() const;
    int getMaxApples() const;

private:
    QSpinBox* spinLevel;
    QSpinBox* spinSuccessCount;
    QSpinBox* spinFailCount;
    QSpinBox* spinTotalPresses;
    QSpinBox* spinTargetSuccess;
    QSpinBox* spinMaxApples;
};

#endif // APPLESETTINGSDIALOG_H
