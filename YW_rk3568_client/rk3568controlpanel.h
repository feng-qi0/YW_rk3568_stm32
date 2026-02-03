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
#include <QRadioButton>
#include <QTimer>
#include <QVideoWidget> // For video display
#include <QMediaPlayer> // For video playback
#include <QNetworkAccessManager> // For HTTP requests
#include <QNetworkRequest> // For HTTP requests
#include <QNetworkReply> // For HTTP responses
#include <QJsonObject> // For JSON parsing
#include <QJsonDocument> // For JSON parsing
#include <QJsonArray> // For JSON parsing

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

    // Main grid layout (replacing splitter for better alignment with HTML design)
    QGridLayout *mainGridLayout;
    QWidget *leftColumnWidget;
    QWidget *rightColumnWidget;
    QVBoxLayout *leftColumnLayout; // 添加缺失的声明

    // Video card
    QFrame *videoCard;
    QVBoxLayout *videoCardLayout;
    QLabel *videoDisplayLabel;
    QVideoWidget *videoWidget; // 新增视频组件
    QMediaPlayer *mediaPlayer; // 新增媒体播放器
    QGridLayout *videoControlsLayout;
    QPushButton *streamStartBtn;
    QPushButton *streamStopBtn;
    QPushButton *snapshotBtn;
    QPushButton *recordBtn;

    // Sensor row widgets
    QFrame *sensorRowWidget;
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

    // Control panel card
    QFrame *controlPanelCard;
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
    QFrame *logCard;
    QVBoxLayout *logCardLayout;
    QTextEdit *logTextBox;

    // Timer for sensor updates
    QTimer *sensorUpdateTimer;

    // Network manager for HTTP requests
    QNetworkAccessManager *networkManager;

public slots:
    void updateSensorData(double temp, double humi, double light, int ir);

private slots:
    void updateSensorDisplay();
    void handleSensorDataResponse(QNetworkReply *reply);
    void handleCameraControlResponse(QNetworkReply *reply);
    void handleHardwareControlResponse(QNetworkReply *reply);
    // Slot for motor direction buttons to ensure mutual exclusivity
    void onMotorDirectionChanged();

private:
    void sendHardwareControlCommand();
    void sendCameraCommand(const QString &command);
};

#endif // RK3568CONTROLPANEL_H