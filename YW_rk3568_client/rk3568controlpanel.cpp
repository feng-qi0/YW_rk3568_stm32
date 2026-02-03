#include "rk3568controlpanel.h"
#include <QApplication>
#include <QDateTime>
#include <QFile> // 用于加载QSS样式文件

RK3568ControlPanel::RK3568ControlPanel(QWidget *parent)
    : QWidget(parent)
{
    // 确保样式表可以正确应用到子控件
    setStyleSheet("/* Apply base style */");

    setupUI();
    setupConnections();

    // 使用启动消息初始化日志
    logWidget->appendLog(QString("[%1] 系统初始化完成...")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));

    // 初始化传感器更新定时器
    sensorUpdateTimer = new QTimer(this);
    connect(sensorUpdateTimer, &QTimer::timeout, this, &RK3568ControlPanel::updateSensorDisplay);
    sensorUpdateTimer->start(1000); // 每秒更新一次传感器数据

    // 初始化网络管理器
    networkManager = new QNetworkAccessManager(this);
}

void RK3568ControlPanel::setupUI()
{
    // --- 主窗口布局 ---
    mainLayout = new QVBoxLayout(this);// QVBoxLayout(this) 创建了一个垂直布局，并将其设置为窗口的主布局。
    this->setWindowTitle(tr("RK3568 智能网关控制系统"));// tr的使用是为了支持多语言
    this->resize(1400, 900); // 增加初始窗口大小

    // --- 顶部标题栏 ---
    headerWidget = new QWidget();
    headerWidget->setObjectName("headerWidget");
    headerWidget->setAutoFillBackground(true);  // 确保样式表正确应用
    headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 0, 20, 20);
    headerLayout->setSpacing(0);

    titleLabel = new QLabel(tr("RK3568 智能网关控制系统"));
    titleLabel->setObjectName("titleLabel");
    statusLabel = new QLabel(tr("● 设备在线"));
    statusLabel->setObjectName("statusLabel");

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(); // 将状态标签推到右侧
    headerLayout->addWidget(statusLabel);
    mainLayout->addWidget(headerWidget);

    // --- 主布局 (使用水平布局替代网格布局以获得更好的左右分布) ---
    mainHBoxLayout = new QHBoxLayout();
    mainHBoxLayout->setSpacing(20); // 设置间距与HTML一致
    mainLayout->addLayout(mainHBoxLayout);

    // --- 左列 ---
    leftColumnWidget = new QWidget();
    leftColumnLayout = new QVBoxLayout(leftColumnWidget);
    leftColumnLayout->setSpacing(20); // 设置间距与HTML一致
    leftColumnWidget->setMinimumWidth(900); // 增加左侧最小宽度
    leftColumnWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // 允许扩展

    // 视频卡片
    videoCard = new QFrame();
    videoCard->setObjectName("card");
    videoCardLayout = new QVBoxLayout(videoCard);

    videoTitleLabel = new QLabel(tr("🎥 实时画面监控"));
    videoTitleLabel->setObjectName("cardTitle");

    // 创建视频显示区域
    videoDisplayLabel = new QLabel();
    videoDisplayLabel->setObjectName("videoDisplayLabel");
    videoDisplayLabel->setAlignment(Qt::AlignCenter);
    videoDisplayLabel->setText(tr("视频流显示区域")); // 提示文本
    videoDisplayLabel->setMinimumHeight(400); // 设置最小高度
    videoDisplayLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 允许扩展

    // 创建视频播放组件
    videoWidget = new QVideoWidget();
    videoWidget->setVisible(false); // 初始隐藏，当有视频流时再显示
    videoWidget->setObjectName("videoWidget");
    videoWidget->setMinimumHeight(400);
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 允许扩展

    // 媒体播放器
    mediaPlayer = new QMediaPlayer();
    mediaPlayer->setVideoOutput(videoWidget);

    videoControlsLayout = new QGridLayout(); // 使用网格布局使按钮大小均匀
    streamStartBtn = new QPushButton(tr("▶ 开始推流"));
    streamStartBtn->setObjectName("streamStartBtn");
    streamStopBtn = new QPushButton(tr("⏹ 停止推流"));
    streamStopBtn->setObjectName("streamStopBtn");
    streamStopBtn->setEnabled(false); // 初始禁用
    snapshotBtn = new QPushButton(tr("📸 抓拍照片"));
    snapshotBtn->setObjectName("snapshotBtn");
    recordBtn = new QPushButton(tr("🔴 视频录制"));
    recordBtn->setObjectName("recordBtn");

    // 为按钮设置最小尺寸，确保文字不被截断
    streamStartBtn->setMinimumSize(100, 40);
    streamStopBtn->setMinimumSize(100, 40);
    snapshotBtn->setMinimumSize(100, 40);
    recordBtn->setMinimumSize(100, 40);

    videoControlsLayout->addWidget(streamStartBtn, 0, 0);
    videoControlsLayout->addWidget(streamStopBtn, 0, 1);
    videoControlsLayout->addWidget(snapshotBtn, 0, 2);
    videoControlsLayout->addWidget(recordBtn, 0, 3);
    videoControlsLayout->setContentsMargins(0, 15, 0, 0); // 上边距与HTML一致

    videoCardLayout->addWidget(videoTitleLabel);
    videoCardLayout->addWidget(videoDisplayLabel);
    videoCardLayout->addWidget(videoWidget);
    videoCardLayout->addLayout(videoControlsLayout);
    leftColumnLayout->addWidget(videoCard);

    // 传感器行
    sensorRowWidget = new QFrame();
    sensorRowWidget->setStyleSheet("background: transparent;"); // 透明背景
    sensorGridLayout = new QGridLayout(sensorRowWidget);
    sensorGridLayout->setSpacing(15); // 设置间距与HTML一致
    sensorGridLayout->setContentsMargins(0, 0, 0, 0); // 设置边距为0

    // 创建传感器组件
    tempCard = new SensorWidget(tr("温度"), tr("°C"));
    humiCard = new SensorWidget(tr("湿度"), tr("% RH"));
    lightCard = new SensorWidget(tr("光照强度"), tr("Lux"));
    irCard = new SensorWidget(tr("红外检测"), tr("Status"));

    sensorGridLayout->addWidget(tempCard, 0, 0);
    sensorGridLayout->addWidget(humiCard, 0, 1);
    sensorGridLayout->addWidget(lightCard, 0, 2);
    sensorGridLayout->addWidget(irCard, 0, 3);
    leftColumnLayout->addWidget(sensorRowWidget);

    // 将左列添加到主水平布局
    mainHBoxLayout->addWidget(leftColumnWidget);

    // --- 右列 ---
    rightColumnWidget = new QWidget();
    rightColumnLayout = new QVBoxLayout(rightColumnWidget);
    rightColumnLayout->setSpacing(20); // 设置间距与HTML一致
    rightColumnWidget->setMinimumWidth(450); // 设置右侧最小宽度
    rightColumnWidget->setMaximumWidth(500); // 设置右侧最大宽度，防止过度扩展

    // 控制面板组件
    controlPanelWidget = new ControlPanelWidget();
    rightColumnLayout->addWidget(controlPanelWidget);

    // 日志组件
    logWidget = new LogWidget();
    rightColumnLayout->addWidget(logWidget, 1); // 拉伸因子 1 以填充剩余空间

    // 将右列添加到主水平布局
    mainHBoxLayout->addWidget(rightColumnWidget);
    mainHBoxLayout->setStretch(0, 3); // 左列占3份
    mainHBoxLayout->setStretch(1, 2); // 右列占2份
}

void RK3568ControlPanel::setupConnections()
{
    // 连接控制面板信号
    connect(controlPanelWidget, &ControlPanelWidget::ledControlChanged,
            this, &RK3568ControlPanel::ledControlChanged);
    connect(controlPanelWidget, &ControlPanelWidget::motorControlChanged,
            this, &RK3568ControlPanel::motorControlChanged);
    connect(controlPanelWidget, &ControlPanelWidget::buzzerControlChanged,
            this, &RK3568ControlPanel::buzzerControlChanged);

    // 连接控制信号
    connect(controlPanelWidget, &ControlPanelWidget::ledControlChanged, this, [this](bool enabled, int brightness) {
        logWidget->appendLog(QString("[%1] LED调整: %2, 亮度 %3%")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(enabled ? tr("开启") : tr("关闭"))
                             .arg(brightness));
        sendHardwareControlCommand(); // 发送控制命令到设备
    });

    connect(controlPanelWidget, &ControlPanelWidget::motorControlChanged, this, [this](bool enabled, int speed, bool directionForward) {
        logWidget->appendLog(QString("[%1] 电机控制: %2, 速度 %3%, 方向 %4")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(enabled ? tr("开启") : tr("关闭"))
                             .arg(speed)
                             .arg(directionForward ? tr("正转") : tr("反转")));
        sendHardwareControlCommand(); // 发送控制命令到设备
    });

    connect(controlPanelWidget, &ControlPanelWidget::buzzerControlChanged, this, [this](bool enabled) {
        logWidget->appendLog(QString("[%1] 蜂鸣器: %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(enabled ? tr("开启") : tr("关闭")));
        sendHardwareControlCommand(); // 发送控制命令到设备
    });

    connect(streamStartBtn, &QPushButton::clicked, this, [this]() {
        streamStartBtn->setEnabled(false);
        streamStopBtn->setEnabled(true);

        // 显示视频组件，隐藏占位标签
        videoDisplayLabel->setVisible(false);
        videoWidget->setVisible(true);

        sendCameraCommand("start_stream"); // 发送相机控制命令
        logWidget->appendLog(QString("[%1] ✅ 视频流已启动")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    });

    connect(streamStopBtn, &QPushButton::clicked, this, [this]() {
        streamStopBtn->setEnabled(false);
        streamStartBtn->setEnabled(true);

        // 隐藏视频组件，显示占位标签
        videoWidget->setVisible(false);
        videoDisplayLabel->setVisible(true);

        sendCameraCommand("stop_stream"); // 发送相机控制命令
        logWidget->appendLog(QString("[%1] ⏹ 视频流已停止")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    });

    connect(snapshotBtn, &QPushButton::clicked, this, [this]() {
        sendCameraCommand("snapshot"); // 发送相机控制命令
        logWidget->appendLog(QString("[%1] ✅ 拍照指令已发送")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    });

    connect(recordBtn, &QPushButton::clicked, this, [this]() {
        static bool recording = false; // 演示用的简单状态跟踪
        recording = !recording;
        if (recording) {
            sendCameraCommand("start_record"); // 发送相机控制命令
            recordBtn->setText(tr("⏹ 停止录制"));
            logWidget->appendLog(QString("[%1] ✅ 录像已开始")
                                 .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else {
            sendCameraCommand("stop_record"); // 发送相机控制命令
            recordBtn->setText(tr("🔴 视频录制"));
            logWidget->appendLog(QString("[%1] ✅ 录像已停止")
                                 .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        }
    });
}

void RK3568ControlPanel::updateSensorData(double temp, double humi, double light, int ir)
{
    // 更新传感器值
    tempCard->setValue(QString::number(temp, 'f', 1));
    humiCard->setValue(QString::number(humi, 'f', 1));
    lightCard->setValue(QString::number(light, 'f', 0));

    // 根据IR值更新显示和颜色
    if (ir > 2000) {
        irCard->setValue("☢ 有人");
        irCard->setValueColor("#ef4444"); // danger color
    } else {
        irCard->setValue("安全");
        irCard->setValueColor("#10b981"); // success color
    }
}


void RK3568ControlPanel::sendHardwareControlCommand()
{
    // 构建控制命令的JSON数据，从controlPanelWidget获取当前值
    QJsonObject payload;
    payload["led_on"] = controlPanelWidget->getLedState() ? 1 : 0;
    payload["led_br"] = controlPanelWidget->getLedBrightness();
    payload["motor_on"] = controlPanelWidget->getMotorState() ? 1 : 0;
    payload["motor_sp"] = controlPanelWidget->getMotorSpeed();
    payload["motor_dir"] = controlPanelWidget->getMotorDirection() ? 0 : 1; // 0为正转，1为反转
    payload["buzzer"] = controlPanelWidget->getBuzzerState() ? 1 : 0;

    QJsonObject msg;
    msg["type"] = "control";
    msg["payload"] = payload;

    QJsonDocument doc(msg);
    QByteArray jsonData = doc.toJson();

    // 发送POST请求到控制端点
    QNetworkRequest request;
    request.setUrl(QUrl("http://localhost/api/control")); // 实际应用中应替换为实际设备IP
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleHardwareControlResponse(reply);
    });
}

void RK3568ControlPanel::sendCameraCommand(const QString &command)
{
    QUrl url;
    if (command == "start_stream") {
        url = QUrl("http://localhost/api/camera/start_stream");
    } else if (command == "stop_stream") {
        url = QUrl("http://localhost/api/camera/stop_stream");
    } else if (command == "snapshot") {
        url = QUrl("http://localhost/api/camera/snapshot");
    } else if (command == "start_record") {
        url = QUrl("http://localhost/api/camera/start_record");
    } else if (command == "stop_record") {
        url = QUrl("http://localhost/api/camera/stop_record");
    }

    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->post(request, QByteArray());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleCameraControlResponse(reply);
    });
}

void RK3568ControlPanel::handleSensorDataResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();

        // 解析传感器数据
        double temp = jsonObj["temp"].toDouble();
        double humi = jsonObj["humi"].toDouble();
        double light = jsonObj["light"].toDouble();
        int ir = jsonObj["ir"].toInt();

        // 更新UI
        updateSensorData(temp, humi, light, ir);

        // 更新硬件状态同步 - 现在需要更新controlPanelWidget的状态
        if (jsonObj.contains("led_on")) {
            controlPanelWidget->setLedState(jsonObj["led_on"].toInt() == 1);
        }
        if (jsonObj.contains("led_br")) {
            controlPanelWidget->setLedBrightness(jsonObj["led_br"].toInt());
        }
        if (jsonObj.contains("motor_on")) {
            controlPanelWidget->setMotorState(jsonObj["motor_on"].toInt() == 1);
        }
        if (jsonObj.contains("motor_sp")) {
            controlPanelWidget->setMotorSpeed(jsonObj["motor_sp"].toInt());
        }
        if (jsonObj.contains("motor_dir")) {
            controlPanelWidget->setMotorDirection(jsonObj["motor_dir"].toInt() == 0);
        }
        if (jsonObj.contains("buzzer")) {
            controlPanelWidget->setBuzzerState(jsonObj["buzzer"].toInt() == 1);
        }

        // 更新连接状态
        statusLabel->setText(tr("● 设备在线"));
        statusLabel->setStyleSheet("background-color: rgba(255, 255, 255, 30); padding: 4px 12px; border-radius: 20px; font-size: 13px;");
    } else {
        // 处理错误情况
        statusLabel->setText(tr("● 连接断开"));
        statusLabel->setStyleSheet("background-color: rgba(255, 255, 255, 30); padding: 4px 12px; border-radius: 20px; font-size: 13px; color: #ff4d4d;");
        logWidget->appendLog(QString("[%1] 传感器数据获取失败: %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(reply->errorString()));
    }

    reply->deleteLater();
}

void RK3568ControlPanel::handleCameraControlResponse(QNetworkReply *reply)
{
    QString command = reply->request().url().toString();
    if (reply->error() == QNetworkReply::NoError) {
        if (command.contains("start_stream")) {
            logWidget->appendLog(QString("[%1] ✅ 视频流启动成功")
                                 .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else if (command.contains("stop_stream")) {
            logWidget->appendLog(QString("[%1] ✅ 视频流停止成功")
                                 .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else if (command.contains("snapshot")) {
            logWidget->appendLog(QString("[%1] ✅ 拍照成功！照片已保存至 snapshots/")
                                 .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else if (command.contains("start_record")) {
            logWidget->appendLog(QString("[%1] ✅ 录像已开始")
                                 .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else if (command.contains("stop_record")) {
            logWidget->appendLog(QString("[%1] ✅ 录像已保存")
                                 .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        }
    } else {
        logWidget->appendLog(QString("[%1] ❌ 相机控制失败: %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(reply->errorString()));
    }

    reply->deleteLater();
}

void RK3568ControlPanel::handleHardwareControlResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        logWidget->appendLog(QString("[%1] ✅ 硬件控制指令发送成功")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    } else {
        logWidget->appendLog(QString("[%1] ❌ 硬件控制失败: %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(reply->errorString()));
    }

    reply->deleteLater();
}

void RK3568ControlPanel::updateSensorDisplay()
{
    // 发送GET请求获取传感器数据
    QNetworkRequest request;
    request.setUrl(QUrl("http://localhost/api/status")); // 实际应用中应替换为实际设备IP
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleSensorDataResponse(reply);
    });
}