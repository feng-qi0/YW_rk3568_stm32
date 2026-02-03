#ifndef CONTROLPANELWIDGET_H
#define CONTROLPANELWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include <QButtonGroup>

class ControlPanelWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ControlPanelWidget(QWidget *parent = nullptr);

signals:
    void ledControlChanged(bool enabled, int brightness);
    void motorControlChanged(bool enabled, int speed, bool directionForward);
    void buzzerControlChanged(bool enabled);

public:
    // Methods to get current control states
    bool getLedState() const;
    int getLedBrightness() const;
    bool getMotorState() const;
    int getMotorSpeed() const;
    bool getMotorDirection() const;  // true for forward, false for reverse
    bool getBuzzerState() const;

    // Methods to set control states (for synchronization from external sources)
    void setLedState(bool enabled);
    void setLedBrightness(int brightness);
    void setMotorState(bool enabled);
    void setMotorSpeed(int speed);
    void setMotorDirection(bool forward);
    void setBuzzerState(bool enabled);

private slots:
    void onLedSwitchToggled(bool checked);
    void onLedBrightnessChanged(int value);
    void onMotorSwitchToggled(bool checked);
    void onMotorSpeedChanged(int value);
    void onMotorDirectionChanged();
    void onBuzzerSwitchToggled(bool checked);

private:
    void setupUI();

    // Layouts
    QVBoxLayout *mainLayout;

    // LED control
    QFrame *ledControlItem;
    QVBoxLayout *ledControlLayout;
    QHBoxLayout *ledHeaderLayout;
    QLabel *ledNameLabel;
    QCheckBox *ledSwitch;
    QSlider *ledBrightnessSlider;

    // Motor control
    QFrame *motorControlItem;
    QVBoxLayout *motorControlLayout;
    QHBoxLayout *motorHeaderLayout;
    QLabel *motorNameLabel;
    QCheckBox *motorSwitch;
    QSlider *motorSpeedSlider;
    QHBoxLayout *motorDirectionLayout;
    QPushButton *dirForwardBtn;
    QPushButton *dirReverseBtn;
    QButtonGroup *directionGroup;

    // Buzzer control
    QFrame *buzzerControlItem;
    QHBoxLayout *buzzerControlLayout;
    QLabel *buzzerNameLabel;
    QCheckBox *buzzerSwitch;
};

#endif // CONTROLPANELWIDGET_H