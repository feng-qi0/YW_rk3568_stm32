#include "rk3568controlpanel.h"
#include <QApplication>
#include <QDateTime>
#include <QButtonGroup> // 用于电机方向单选按钮组
#include <QScrollArea> // 添加滚动区域支持


RK3568ControlPanel::RK3568ControlPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
    // 使用启动消息初始化日志
    logTextBox->append(QString("[%1] 系统初始化完成...")
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
    mainLayout = new QVBoxLayout(this);
    this->setStyleSheet("QWidget { background-color: #f0f2f5; }"); // 设置整体背景色
    this->setWindowTitle(tr("RK3568 智能网关控制系统"));
    this->resize(1400, 900); // 增加初始窗口大小

    // --- 顶部标题栏 ---
    headerWidget = new QWidget();
    headerLayout = new QHBoxLayout(headerWidget);
    headerWidget->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4f46e5, stop:1 #7c3aed); "
                               "color: white; border-radius: 16px; min-height: 60px; margin: 0 20px 20px 20px;");
    titleLabel = new QLabel(tr("RK3568 智能网关控制系统"));
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 800; letter-spacing: 1px;");
    titleLabel->setMinimumWidth(300); // 为标题设置最小宽度
    statusLabel = new QLabel(tr("● 设备在线"));
    statusLabel->setStyleSheet("background-color: rgba(255, 255, 255, 30); padding: 4px 12px; border-radius: 20px; font-size: 13px;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(); // 将状态标签推到右侧
    headerLayout->addWidget(statusLabel);
    mainLayout->addWidget(headerWidget);

    // --- 主布局 (使用水平布局替代网格布局以获得更好的左右分布) ---
    QHBoxLayout *mainHBoxLayout = new QHBoxLayout();
    mainHBoxLayout->setSpacing(20); // 设置间距与HTML一致
    mainLayout->addLayout(mainHBoxLayout);

    // --- 左列 ---
    leftColumnWidget = new QWidget();
    leftColumnLayout = new QVBoxLayout(leftColumnWidget);
    leftColumnLayout->setSpacing(20); // 设置间距与HTML一致
    leftColumnWidget->setMinimumWidth(900); // 增加左侧最小宽度
    leftColumnWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // 允许扩展

    // 视频卡片
    videoCard = new QFrame(); // 使用QFrame替代QGroupBox以获得更好的样式控制
    videoCardLayout = new QVBoxLayout(videoCard);
    videoCard->setStyleSheet("background: #ffffff; border-radius: 16px; padding: 20px; "
                             "box-shadow: 0 4px 6px rgba(0, 0, 0, 0.02);");
    QLabel *videoTitleLabel = new QLabel(tr("🎥 实时画面监控"));
    videoTitleLabel->setStyleSheet("font-size: 1.1rem; font-weight: 700; margin-bottom: 15px; "
                                   "display: flex; align-items: center; gap: 8px; color: #1f2937;");

    // 创建视频显示区域
    videoDisplayLabel = new QLabel();
    videoDisplayLabel->setStyleSheet("background-color: black; color: white; border-radius: 12px; "
                                     "border: 4px solid #fff; box-shadow: 0 0 20px rgba(0, 0, 0, 0.1);");
    videoDisplayLabel->setAlignment(Qt::AlignCenter);
    videoDisplayLabel->setText(tr("视频流显示区域")); // 提示文本
    videoDisplayLabel->setMinimumHeight(400); // 设置最小高度
    videoDisplayLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 允许扩展

    // 创建视频播放组件
    videoWidget = new QVideoWidget();
    videoWidget->setVisible(false); // 初始隐藏，当有视频流时再显示
    videoWidget->setStyleSheet("background-color: black; border-radius: 12px; "
                               "border: 4px solid #fff; box-shadow: 0 0 20px rgba(0, 0, 0, 0.1);");
    videoWidget->setMinimumHeight(400);
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 允许扩展

    // 媒体播放器
    mediaPlayer = new QMediaPlayer();
    mediaPlayer->setVideoOutput(videoWidget);

    videoControlsLayout = new QGridLayout(); // 使用网格布局使按钮大小均匀
    streamStartBtn = new QPushButton(tr("▶ 开始推流"));
    streamStopBtn = new QPushButton(tr("⏹ 停止推流"));
    streamStopBtn->setEnabled(false); // 初始禁用
    snapshotBtn = new QPushButton(tr("📸 抓拍照片"));
    recordBtn = new QPushButton(tr("🔴 视频录制"));

    // 为按钮设置样式，匹配HTML设计
    streamStartBtn->setStyleSheet("padding: 12px; border: none; border-radius: 8px; "
                                  "cursor: pointer; font-weight: 600; font-size: 13px; "
                                  "color: white; background: #3b82f6;"); // info color
    streamStopBtn->setStyleSheet("padding: 12px; border: none; border-radius: 8px; "
                                 "cursor: pointer; font-weight: 600; font-size: 13px; "
                                 "color: white; background: #64748b;"); // gray color
    snapshotBtn->setStyleSheet("padding: 12px; border: none; border-radius: 8px; "
                               "cursor: pointer; font-weight: 600; font-size: 13px; "
                               "color: white; background: #10b981;"); // success color
    recordBtn->setStyleSheet("padding: 12px; border: none; border-radius: 8px; "
                             "cursor: pointer; font-weight: 600; font-size: 13px; "
                             "color: white; background: #ef4444;"); // danger color

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

    // 创建传感器卡片的辅助 Lambda 函数
    auto createSensorCard = [this](const QString &name, const QString &unit) -> SensorCard {
        SensorCard card;
        card.frame = new QFrame();
        card.frame->setStyleSheet("background: #fff; padding: 15px; border-radius: 12px; "
                                  "text-align: center; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.02);");
        card.frame->setMinimumSize(180, 120); // 为传感器卡片设置最小尺寸
        card.frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // 允许扩展
        QVBoxLayout *cardLayout = new QVBoxLayout(card.frame);
        card.nameLabel = new QLabel(name);
        card.nameLabel->setStyleSheet("font-weight: normal;"); // 标准字体权重
        card.valueLabel = new QLabel("--");
        card.valueLabel->setStyleSheet("font-size: 22px; font-weight: 800; color: #5c67f2; margin: 4px 0;");
        card.unitLabel = new QLabel(unit);
        card.unitLabel->setStyleSheet("font-size: 12px; color: #94a3b8;");
        cardLayout->addWidget(card.nameLabel);
        cardLayout->addWidget(card.valueLabel);
        cardLayout->addWidget(card.unitLabel);
        cardLayout->setContentsMargins(15, 15, 15, 15); // 框架内的填充
        cardLayout->setSpacing(4); // 元素间的间距
        cardLayout->setAlignment(Qt::AlignCenter); // 居中内容
        return card;
    };

    tempCard = createSensorCard(tr("温度"), tr("°C"));
    humiCard = createSensorCard(tr("湿度"), tr("% RH"));
    lightCard = createSensorCard(tr("光照强度"), tr("Lux"));
    irCard = createSensorCard(tr("红外检测"), tr("Status"));

    sensorGridLayout->addWidget(tempCard.frame, 0, 0);
    sensorGridLayout->addWidget(humiCard.frame, 0, 1);
    sensorGridLayout->addWidget(lightCard.frame, 0, 2);
    sensorGridLayout->addWidget(irCard.frame, 0, 3);
    leftColumnLayout->addWidget(sensorRowWidget);

    // 将左列添加到主水平布局
    mainHBoxLayout->addWidget(leftColumnWidget);

    // --- 右列 ---
    rightColumnWidget = new QWidget();
    QVBoxLayout *rightColumnLayout = new QVBoxLayout(rightColumnWidget);
    rightColumnLayout->setSpacing(20); // 设置间距与HTML一致
    rightColumnWidget->setMinimumWidth(450); // 设置右侧最小宽度
    rightColumnWidget->setMaximumWidth(500); // 设置右侧最大宽度，防止过度扩展

    // 控制面板卡片
    controlPanelCard = new QFrame(); // 使用QFrame替代QGroupBox以获得更好的样式控制
    controlPanelLayout = new QVBoxLayout(controlPanelCard);
    controlPanelCard->setStyleSheet("background: #ffffff; border-radius: 16px; padding: 20px; "
                                    "box-shadow: 0 4px 6px rgba(0, 0, 0, 0.02);");

    QLabel *controlPanelTitle = new QLabel(tr("⚙️ 硬件外设控制"));
    controlPanelTitle->setStyleSheet("font-size: 1.1rem; font-weight: 700; margin-bottom: 15px; "
                                     "display: flex; align-items: center; gap: 8px; color: #1f2937;");

    // LED 控制项
    ledControlItem = new QFrame(); // 使用 QFrame 以便设置样式
    ledControlItem->setStyleSheet("background: #f8fafc; padding: 15px; border-radius: 12px; "
                                  "margin-bottom: 12px; border: 1px solid #edf2f7;");
    ledControlItem->setMinimumHeight(100); // 为控制项设置最小高度
    ledControlLayout = new QVBoxLayout(ledControlItem);
    ledHeaderLayout = new QHBoxLayout();
    ledNameLabel = new QLabel(tr("LED 照明灯"));
    ledNameLabel->setStyleSheet("font-weight: 600;"); // 加粗标签
    ledSwitch = new QCheckBox();
    ledHeaderLayout->addWidget(ledNameLabel);
    ledHeaderLayout->addStretch(); // 将开关推到右侧
    ledHeaderLayout->addWidget(ledSwitch);
    ledBrightnessSlider = new QSlider(Qt::Horizontal);
    ledBrightnessSlider->setRange(0, 100);
    ledBrightnessSlider->setValue(50);
    // 设置滑块颜色主题
    ledBrightnessSlider->setStyleSheet("QSlider::groove:horizontal {"
                                       "    border: 1px solid #999999;"
                                       "    height: 8px;"
                                       "    background: #d3d3d3;"
                                       "    margin: 2px 0;"
                                       "}"
                                       "QSlider::handle:horizontal {"
                                       "    background: #5c67f2;"
                                       "    border: 1px solid #5c67f2;"
                                       "    width: 18px;"
                                       "    margin: -2px 0;"
                                       "    border-radius: 9px;"
                                       "}");
    ledControlLayout->addLayout(ledHeaderLayout);
    ledControlLayout->addWidget(ledBrightnessSlider);
    ledControlLayout->setContentsMargins(0, 0, 0, 0); // 移除内边距以适应框架
    ledControlLayout->setSpacing(10); // 设置间距

    // 电机控制项
    motorControlItem = new QFrame();
    motorControlItem->setStyleSheet("background: #f8fafc; padding: 15px; border-radius: 12px; "
                                    "margin-bottom: 12px; border: 1px solid #edf2f7;");
    motorControlItem->setMinimumHeight(140); // 为电机控制项设置更大最小高度
    motorControlLayout = new QVBoxLayout(motorControlItem);
    motorHeaderLayout = new QHBoxLayout();
    motorNameLabel = new QLabel(tr("直流电机控制"));
    motorNameLabel->setStyleSheet("font-weight: 600;"); // 加粗标签
    motorSwitch = new QCheckBox();
    motorHeaderLayout->addWidget(motorNameLabel);
    motorHeaderLayout->addStretch();
    motorHeaderLayout->addWidget(motorSwitch);
    motorSpeedSlider = new QSlider(Qt::Horizontal);
    motorSpeedSlider->setRange(0, 100);
    motorSpeedSlider->setValue(30);
    // 设置滑块颜色主题
    motorSpeedSlider->setStyleSheet("QSlider::groove:horizontal {"
                                    "    border: 1px solid #999999;"
                                    "    height: 8px;"
                                    "    background: #d3d3d3;"
                                    "    margin: 2px 0;"
                                    "}"
                                    "QSlider::handle:horizontal {"
                                    "    background: #5c67f2;"
                                    "    border: 1px solid #5c67f2;"
                                    "    width: 18px;"
                                    "    margin: -2px 0;"
                                    "    border-radius: 9px;"
                                    "}");
    motorDirectionLayout = new QHBoxLayout();
    dirForwardBtn = new QPushButton(tr("正向旋转"));
    dirReverseBtn = new QPushButton(tr("反向旋转"));
    dirForwardBtn->setCheckable(true);
    dirReverseBtn->setCheckable(true);
    dirForwardBtn->setChecked(true); // 默认为正转

    // 设置方向按钮样式，匹配HTML设计
    QString activeStyle = "QPushButton:checked { background: #fff; color: #5c67f2; "
                          "border: 2px solid #5c67f2; font-weight: bold; }";
    QString inactiveStyle = "QPushButton { background: #e2e8f0; color: #64748b; "
                            "padding: 6px; border-radius: 6px; border: 2px solid transparent; "
                            "font-size: 12px; }";
    dirForwardBtn->setStyleSheet(inactiveStyle + activeStyle);
    dirReverseBtn->setStyleSheet(inactiveStyle + activeStyle);

    // 为方向按钮设置最小尺寸
    dirForwardBtn->setMinimumSize(80, 30);
    dirReverseBtn->setMinimumSize(80, 30);

    motorDirectionLayout->addWidget(dirForwardBtn);
    motorDirectionLayout->addWidget(dirReverseBtn);

    motorControlLayout->addLayout(motorHeaderLayout);
    motorControlLayout->addWidget(motorSpeedSlider);
    motorControlLayout->addLayout(motorDirectionLayout);
    motorControlLayout->setContentsMargins(0, 0, 0, 0); // 移除内边距以适应框架
    motorControlLayout->setSpacing(10); // 设置间距

    // 蜂鸣器控制项
    buzzerControlItem = new QFrame();
    // 添加左侧警告色边框效果
    buzzerControlItem->setStyleSheet("background: #fff9db; padding: 15px; border-radius: 12px; "
                                     "margin-bottom: 0px; border-left: 4px solid #f59e0b;"); // 黄色警告边框
    buzzerControlItem->setMinimumHeight(70); // 稍微增加一点高度
    buzzerControlLayout = new QHBoxLayout(buzzerControlItem); // 水平布局用于名称和开关
    buzzerNameLabel = new QLabel(tr("紧急蜂鸣报警"));
    buzzerNameLabel->setStyleSheet("font-weight: 600;"); // 加粗标签
    buzzerSwitch = new QCheckBox();
    buzzerControlLayout->addWidget(buzzerNameLabel);
    buzzerControlLayout->addStretch();
    buzzerControlLayout->addWidget(buzzerSwitch);
    buzzerControlLayout->setContentsMargins(0, 0, 0, 0); // 移除内边距以适应框架

    controlPanelLayout->addWidget(controlPanelTitle);
    controlPanelLayout->addWidget(ledControlItem);
    controlPanelLayout->addWidget(motorControlItem);
    controlPanelLayout->addWidget(buzzerControlItem);
    controlPanelLayout->setContentsMargins(0, 0, 0, 0); // 移除内边距以适应框架
    controlPanelLayout->setSpacing(0); // 设置间距为0，因为项目内部已有间距

    rightColumnLayout->addWidget(controlPanelCard);

    // 日志卡片
    logCard = new QFrame(); // 使用QFrame替代QGroupBox以获得更好的样式控制
    logCardLayout = new QVBoxLayout(logCard);
    logCard->setStyleSheet("background: #ffffff; border-radius: 16px; padding: 20px; "
                           "box-shadow: 0 4px 6px rgba(0, 0, 0, 0.02);");

    QLabel *logTitleLabel = new QLabel(tr("📜 系统运行日志"));
    logTitleLabel->setStyleSheet("font-size: 1.1rem; font-weight: 700; margin-bottom: 15px; "
                                 "display: flex; align-items: center; gap: 8px; color: #1f2937;");

    logTextBox = new QTextEdit();
    logTextBox->setReadOnly(true);
    logTextBox->setStyleSheet("background-color: #0f172a; color: #38bdf8; border-radius: 12px; "
                              "font-family: 'Courier New', Courier, monospace; font-size: 12px; "
                              "border: 1px solid #1e293b; padding: 15px;"); // 模拟日志容器
    logTextBox->setMinimumHeight(200); // 设置最小高度
    logTextBox->setMaximumHeight(300); // 设置最大高度
    logTextBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 允许垂直扩展

    logCardLayout->addWidget(logTitleLabel);
    logCardLayout->addWidget(logTextBox);
    logCardLayout->setContentsMargins(0, 0, 0, 0); // 移除内边距以适应框架

    rightColumnLayout->addWidget(logCard, 1); // 拉伸因子 1 以填充剩余空间
    rightColumnLayout->setContentsMargins(0, 0, 0, 0); // 移除内边距以适应框架
    rightColumnLayout->setSpacing(20); // 设置间距与HTML一致

    // 将右列添加到主水平布局
    mainHBoxLayout->addWidget(rightColumnWidget);
    mainHBoxLayout->setStretch(0, 3); // 左列占3份
    mainHBoxLayout->setStretch(1, 2); // 右列占2份
}

void RK3568ControlPanel::setupConnections()
{
    // 连接控制信号
    connect(ledSwitch, &QCheckBox::toggled, this, [this](bool checked) {
        emit ledControlChanged(checked, ledBrightnessSlider->value());
        logTextBox->append(QString("[%1] LED调整: %2, 亮度 %3%")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(checked ? tr("开启") : tr("关闭"))
                          .arg(ledBrightnessSlider->value()));
        sendHardwareControlCommand(); // 发送控制命令到设备
    });

    connect(ledBrightnessSlider, &QSlider::valueChanged, this, [this](int value) {
        if (ledSwitch->isChecked()) { // 仅在 LED 开启时记录日志
            emit ledControlChanged(ledSwitch->isChecked(), value);
            logTextBox->append(QString("[%1] LED亮度调整: %2%")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                              .arg(value));
            sendHardwareControlCommand(); // 发送控制命令到设备
        }
    });

    connect(motorSwitch, &QCheckBox::toggled, this, [this](bool checked) {
        bool dirForward = dirForwardBtn->isChecked();
        emit motorControlChanged(checked, motorSpeedSlider->value(), dirForward);
        logTextBox->append(QString("[%1] 电机控制: %2, 速度 %3%, 方向 %4")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(checked ? tr("开启") : tr("关闭"))
                          .arg(motorSpeedSlider->value())
                          .arg(dirForward ? tr("正转") : tr("反转")));
        sendHardwareControlCommand(); // 发送控制命令到设备
    });

    connect(motorSpeedSlider, &QSlider::valueChanged, this, [this](int value) {
        if (motorSwitch->isChecked()) { // 仅在电机开启时记录日志
            bool dirForward = dirForwardBtn->isChecked();
            emit motorControlChanged(motorSwitch->isChecked(), value, dirForward);
            logTextBox->append(QString("[%1] 电机速度调整: %2%")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                              .arg(value));
            sendHardwareControlCommand(); // 发送控制命令到设备
        }
    });

    // 电机方向按钮是互斥的
    connect(dirForwardBtn, &QPushButton::clicked, this, &RK3568ControlPanel::onMotorDirectionChanged);
    connect(dirReverseBtn, &QPushButton::clicked, this, &RK3568ControlPanel::onMotorDirectionChanged);

    connect(buzzerSwitch, &QCheckBox::toggled, this, [this](bool checked) {
        emit buzzerControlChanged(checked);
        logTextBox->append(QString("[%1] 蜂鸣器: %2")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(checked ? tr("开启") : tr("关闭")));
        sendHardwareControlCommand(); // 发送控制命令到设备
    });

    connect(streamStartBtn, &QPushButton::clicked, this, [this]() {
        streamStartBtn->setEnabled(false);
        streamStopBtn->setEnabled(true);

        // 显示视频组件，隐藏占位标签
        videoDisplayLabel->setVisible(false);
        videoWidget->setVisible(true);

        sendCameraCommand("start_stream"); // 发送相机控制命令
        logTextBox->append(QString("[%1] ✅ 视频流已启动")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    });

    connect(streamStopBtn, &QPushButton::clicked, this, [this]() {
        streamStopBtn->setEnabled(false);
        streamStartBtn->setEnabled(true);

        // 隐藏视频组件，显示占位标签
        videoWidget->setVisible(false);
        videoDisplayLabel->setVisible(true);

        sendCameraCommand("stop_stream"); // 发送相机控制命令
        logTextBox->append(QString("[%1] ⏹ 视频流已停止")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    });

    connect(snapshotBtn, &QPushButton::clicked, this, [this]() {
        sendCameraCommand("snapshot"); // 发送相机控制命令
        logTextBox->append(QString("[%1] ✅ 拍照指令已发送")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    });

    connect(recordBtn, &QPushButton::clicked, this, [this]() {
        static bool recording = false; // 演示用的简单状态跟踪
        recording = !recording;
        if (recording) {
            sendCameraCommand("start_record"); // 发送相机控制命令
            recordBtn->setText(tr("⏹ 停止录制"));
            logTextBox->append(QString("[%1] ✅ 录像已开始")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else {
            sendCameraCommand("stop_record"); // 发送相机控制命令
            recordBtn->setText(tr("🔴 视频录制"));
            logTextBox->append(QString("[%1] ✅ 录像已停止")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        }
    });
}

void RK3568ControlPanel::updateSensorData(double temp, double humi, double light, int ir)
{
    // 更新传感器值
    tempCard.valueLabel->setText(QString::number(temp, 'f', 1));
    humiCard.valueLabel->setText(QString::number(humi, 'f', 1));
    lightCard.valueLabel->setText(QString::number(light, 'f', 0));

    // 根据IR值更新显示和颜色
    if (ir > 2000) {
        irCard.valueLabel->setText("☢ 有人");
        irCard.valueLabel->setStyleSheet("font-size: 16px; font-weight: 800; color: #ef4444; margin: 4px 0;"); // danger color
    } else {
        irCard.valueLabel->setText("安全");
        irCard.valueLabel->setStyleSheet("font-size: 16px; font-weight: 800; color: #10b981; margin: 4px 0;"); // success color
    }
}


void RK3568ControlPanel::sendHardwareControlCommand()
{
    // 构建控制命令的JSON数据
    QJsonObject payload;
    payload["led_on"] = ledSwitch->isChecked() ? 1 : 0;
    payload["led_br"] = ledBrightnessSlider->value();
    payload["motor_on"] = motorSwitch->isChecked() ? 1 : 0;
    payload["motor_sp"] = motorSpeedSlider->value();
    payload["motor_dir"] = dirForwardBtn->isChecked() ? 0 : 1; // 0为正转，1为反转
    payload["buzzer"] = buzzerSwitch->isChecked() ? 1 : 0;

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

        // 更新硬件状态同步
        if (jsonObj.contains("led_on")) {
            ledSwitch->setChecked(jsonObj["led_on"].toInt() == 1);
        }
        if (jsonObj.contains("led_br")) {
            ledBrightnessSlider->setValue(jsonObj["led_br"].toInt());
        }
        if (jsonObj.contains("motor_on")) {
            motorSwitch->setChecked(jsonObj["motor_on"].toInt() == 1);
        }
        if (jsonObj.contains("motor_sp")) {
            motorSpeedSlider->setValue(jsonObj["motor_sp"].toInt());
        }
        if (jsonObj.contains("motor_dir")) {
            if (jsonObj["motor_dir"].toInt() == 0) {
                dirForwardBtn->setChecked(true);
                dirReverseBtn->setChecked(false);
            } else {
                dirForwardBtn->setChecked(false);
                dirReverseBtn->setChecked(true);
            }
        }
        if (jsonObj.contains("buzzer")) {
            buzzerSwitch->setChecked(jsonObj["buzzer"].toInt() == 1);
        }

        // 更新连接状态
        statusLabel->setText(tr("● 设备在线"));
        statusLabel->setStyleSheet("background-color: rgba(255, 255, 255, 30); padding: 4px 12px; border-radius: 20px; font-size: 13px;");
    } else {
        // 处理错误情况
        statusLabel->setText(tr("● 连接断开"));
        statusLabel->setStyleSheet("background-color: rgba(255, 255, 255, 30); padding: 4px 12px; border-radius: 20px; font-size: 13px; color: #ff4d4d;");
        logTextBox->append(QString("[%1] 传感器数据获取失败: %2")
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
            logTextBox->append(QString("[%1] ✅ 视频流启动成功")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else if (command.contains("stop_stream")) {
            logTextBox->append(QString("[%1] ✅ 视频流停止成功")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else if (command.contains("snapshot")) {
            logTextBox->append(QString("[%1] ✅ 拍照成功！照片已保存至 snapshots/")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else if (command.contains("start_record")) {
            logTextBox->append(QString("[%1] ✅ 录像已开始")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else if (command.contains("stop_record")) {
            logTextBox->append(QString("[%1] ✅ 录像已保存")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        }
    } else {
        logTextBox->append(QString("[%1] ❌ 相机控制失败: %2")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(reply->errorString()));
    }

    reply->deleteLater();
}

void RK3568ControlPanel::handleHardwareControlResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        logTextBox->append(QString("[%1] ✅ 硬件控制指令发送成功")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    } else {
        logTextBox->append(QString("[%1] ❌ 硬件控制失败: %2")
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

void RK3568ControlPanel::onMotorDirectionChanged()
{
    // 确保只有一个方向按钮被选中
    if (QObject::sender() == dirForwardBtn) {
        if (dirForwardBtn->isChecked()) {
            dirReverseBtn->setChecked(false);
        } else {
            dirForwardBtn->setChecked(true); // 防止两个按钮都被取消选中
        }
    } else if (QObject::sender() == dirReverseBtn) {
        if (dirReverseBtn->isChecked()) {
            dirForwardBtn->setChecked(false);
        } else {
            dirReverseBtn->setChecked(true); // 防止两个按钮都被取消选中
        }
    }
    // 如果电机开启，则记录更改并发送控制命令
    if (motorSwitch->isChecked()) {
        bool dirForward = dirForwardBtn->isChecked();
        emit motorControlChanged(motorSwitch->isChecked(), motorSpeedSlider->value(), dirForward);
        logTextBox->append(QString("[%1] 电机方向切换: %2")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(dirForward ? tr("正转") : tr("反转")));
        sendHardwareControlCommand();
    }
}