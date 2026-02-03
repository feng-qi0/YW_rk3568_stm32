#ifndef RK3568CONTROLPANEL_H
#define RK3568CONTROLPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include "sensorwidget.h"
#include "controlpanelwidget.h"
#include "logwidget.h"

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

    // Main layout
    QHBoxLayout *mainHBoxLayout;
    QWidget *leftColumnWidget;
    QVBoxLayout *leftColumnLayout;

    // Video card
    QFrame *videoCard;
    QVBoxLayout *videoCardLayout;
    QLabel *videoTitleLabel;
    QLabel *videoDisplayLabel;
    QVideoWidget *videoWidget;
    QMediaPlayer *mediaPlayer;
    QGridLayout *videoControlsLayout;
    QPushButton *streamStartBtn;
    QPushButton *streamStopBtn;
    QPushButton *snapshotBtn;
    QPushButton *recordBtn;

    // Sensor row
    QFrame *sensorRowWidget;
    QGridLayout *sensorGridLayout;
    SensorWidget *tempCard;
    SensorWidget *humiCard;
    SensorWidget *lightCard;
    SensorWidget *irCard;

    // Right column
    QWidget *rightColumnWidget;
    QVBoxLayout *rightColumnLayout;

    // Control panel widget (now a separate component)
    ControlPanelWidget *controlPanelWidget;

    // Log widget (now a separate component)
    LogWidget *logWidget;

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

private:
    void sendHardwareControlCommand();
    void sendCameraCommand(const QString &command);
};

#endif // RK3568CONTROLPANEL_H