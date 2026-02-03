#ifndef RK3568CONTROLPANEL_H
#define RK3568CONTROLPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QFrame>
#include <QRadioButton> // For motor direction

class RK3568ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit RK3568ControlPanel(QWidget *parent = nullptr);

signals:
    // Signals for hardware control commands
    void ledControlChanged(bool enabled, int brightness);
    void motorControlChanged(bool enabled, int speed, bool directionForward);
    void buzzerControlChanged(bool enabled);
    void cameraStreamStartRequested();
    void cameraStreamStopRequested();
    void cameraSnapshotRequested();
    void cameraRecordToggleRequested();

private slots:
    // Slot for motor direction buttons to ensure mutual exclusivity
    void onMotorDirectionChanged();

private:
    void setupUI();
    void setupConnections();

    // Main layout
    QVBoxLayout *mainLayout;

    // Header widgets
    QWidget *headerWidget;
    QHBoxLayout *headerLayout;
    QLabel *titleLabel;
    QLabel *statusLabel;

    // Main splitter
    QSplitter *mainSplitter;

    // Left column widgets
    QWidget *leftColumnWidget;
    QVBoxLayout *leftColumnLayout;
    QGroupBox *videoCard;
    QVBoxLayout *videoCardLayout;
    QLabel *videoDisplayLabel;
    QGridLayout *videoControlsLayout;
    QPushButton *streamStartBtn;
    QPushButton *streamStopBtn;
    QPushButton *snapshotBtn;
    QPushButton *recordBtn;

    // Sensor row widgets
    QWidget *sensorRowWidget;
    QGridLayout *sensorGridLayout;
    struct SensorCard {
        QFrame *frame;
        QLabel *nameLabel;
        QLabel *valueLabel;
        QLabel *unitLabel;
    };
    SensorCard tempCard;
    SensorCard humiCard;
    SensorCard lightCard;
    SensorCard irCard;

    // Right column widgets
    QWidget *rightColumnWidget;
    QVBoxLayout *rightColumnLayout;

    // Control panel card
    QGroupBox *controlPanelCard;
    QVBoxLayout *controlPanelLayout;

    // LED control item
    QFrame *ledControlItem;
    QVBoxLayout *ledControlLayout;
    QHBoxLayout *ledHeaderLayout;
    QLabel *ledNameLabel;
    QCheckBox *ledSwitch;
    QSlider *ledBrightnessSlider;

    // Motor control item
    QFrame *motorControlItem;
    QVBoxLayout *motorControlLayout;
    QHBoxLayout *motorHeaderLayout;
    QLabel *motorNameLabel;
    QCheckBox *motorSwitch;
    QSlider *motorSpeedSlider;
    QHBoxLayout *motorDirectionLayout;
    QPushButton *dirForwardBtn;
    QPushButton *dirReverseBtn;

    // Buzzer control item
    QFrame *buzzerControlItem;
    QHBoxLayout *buzzerControlLayout;
    QLabel *buzzerNameLabel;
    QCheckBox *buzzerSwitch;

    // Log card
    QGroupBox *logCard;
    QVBoxLayout *logCardLayout;
    QTextEdit *logTextBox;
};

#endif // RK3568CONTROLPANEL_H