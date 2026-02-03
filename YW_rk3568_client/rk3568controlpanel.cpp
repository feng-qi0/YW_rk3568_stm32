#include "rk3568controlpanel.h"
#include <QApplication>
#include <QDateTime>
#include <QButtonGroup> // 用于电机方向单选按钮组


RK3568ControlPanel::RK3568ControlPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
    // 使用启动消息初始化日志
    logTextBox->append(QString("[%1] 系统初始化完成...")
                      .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void RK3568ControlPanel::setupUI()
{
    // --- 主窗口布局 ---
    mainLayout = new QVBoxLayout(this);
    this->setWindowTitle(tr("RK3568 智能网关控制系统"));
    this->resize(1400, 900); // 增加初始窗口大小

    // --- 顶部标题栏 ---
    headerWidget = new QWidget();
    headerLayout = new QHBoxLayout(headerWidget);
    headerWidget->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4f46e5, stop:1 #7c3aed); "
                               "color: white; border-radius: 16px; min-height: 60px; margin-bottom: 20px;");
    titleLabel = new QLabel(tr("RK3568 智能网关控制系统"));
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; letter-spacing: 1px;");
    titleLabel->setMinimumWidth(300); // 为标题设置最小宽度
    statusLabel = new QLabel(tr("● 设备在线"));
    statusLabel->setStyleSheet("background-color: rgba(255, 255, 255, 30); padding: 4px 12px; border-radius: 20px; font-size: 13px;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(); // 将状态标签推到右侧
    headerLayout->addWidget(statusLabel);
    mainLayout->addWidget(headerWidget);

    // --- 主分割器 (左列和右列) ---
    mainSplitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(mainSplitter);

    // --- 左列 ---
    leftColumnWidget = new QWidget();
    leftColumnLayout = new QVBoxLayout(leftColumnWidget);
    leftColumnWidget->setMinimumWidth(700); // 增加左侧最小宽度

    // 视频卡片
    videoCard = new QGroupBox(tr("🎥 实时画面监控"));
    videoCardLayout = new QVBoxLayout(videoCard);
    videoCard->setMinimumHeight(350); // 进一步增加视频卡片的最小高度
    videoCard->setStyleSheet("QGroupBox { font-weight: bold; }"); // 确保标题样式清晰，但不强制改变高度过多
    videoDisplayLabel = new QLabel(tr("Video Stream Display Area"));
    videoDisplayLabel->setAlignment(Qt::AlignCenter);
    videoDisplayLabel->setStyleSheet("background-color: black; color: white; border-radius: 12px;"); // 模拟视频框
    videoControlsLayout = new QGridLayout(); // 使用网格布局使按钮大小均匀
    streamStartBtn = new QPushButton(tr("▶ 开始推流"));
    streamStopBtn = new QPushButton(tr("⏹ 停止推流"));
    streamStopBtn->setEnabled(false); // 初始禁用
    snapshotBtn = new QPushButton(tr("📸 抓拍照片"));
    recordBtn = new QPushButton(tr("🔴 视频录制"));

    // 为按钮设置最小尺寸，确保文字不被截断
    streamStartBtn->setMinimumSize(100, 40);
    streamStopBtn->setMinimumSize(100, 40);
    snapshotBtn->setMinimumSize(100, 40);
    recordBtn->setMinimumSize(100, 40);

    videoControlsLayout->addWidget(streamStartBtn, 0, 0);
    videoControlsLayout->addWidget(streamStopBtn, 0, 1);
    videoControlsLayout->addWidget(snapshotBtn, 0, 2);
    videoControlsLayout->addWidget(recordBtn, 0, 3);

    videoCardLayout->addWidget(videoDisplayLabel);
    videoCardLayout->addLayout(videoControlsLayout);
    leftColumnLayout->addWidget(videoCard);

    // 传感器行
    sensorRowWidget = new QWidget();
    sensorGridLayout = new QGridLayout(sensorRowWidget);
    sensorRowWidget->setMaximumHeight(180); // 增加传感器行的最大高度

    // 创建传感器卡片的辅助 Lambda 函数
    auto createSensorCard = [this](const QString &name, const QString &unit) -> SensorCard {
        SensorCard card;
        card.frame = new QFrame();
        card.frame->setStyleSheet("background: #fff; padding: 15px; border-radius: 12px; "
                                  "text-align: center; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.02);");
        card.frame->setMinimumSize(150, 120); // 为传感器卡片设置最小尺寸
        QVBoxLayout *cardLayout = new QVBoxLayout(card.frame);
        card.nameLabel = new QLabel(name);
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
    sensorGridLayout->setSpacing(15); // 卡片间的间距
    leftColumnLayout->addWidget(sensorRowWidget);

    mainSplitter->addWidget(leftColumnWidget);

    // --- 右列 ---
    rightColumnWidget = new QWidget();
    rightColumnLayout = new QVBoxLayout(rightColumnWidget);
    rightColumnWidget->setMinimumWidth(500); // 增加右侧最小宽度

    // 控制面板卡片
    controlPanelCard = new QGroupBox(tr("⚙️ 硬件外设控制"));
    controlPanelLayout = new QVBoxLayout(controlPanelCard);
    controlPanelCard->setMinimumHeight(450); // 进一步增加控制面板卡片的最小高度
    controlPanelCard->setStyleSheet("QGroupBox { font-weight: bold; }"); // 确保标题样式清晰，但不强制改变高度过多

    // LED 控制项
    ledControlItem = new QFrame(); // 使用 QFrame 以便设置样式
    ledControlItem->setStyleSheet("background: #f8fafc; padding: 15px; border-radius: 12px; "
                                  "margin-bottom: 12px; border: 1px solid #edf2f7;");
    ledControlItem->setMinimumHeight(100); // 为控制项设置最小高度
    ledControlLayout = new QVBoxLayout(ledControlItem);
    ledHeaderLayout = new QHBoxLayout();
    ledNameLabel = new QLabel(tr("LED 照明灯"));
    ledSwitch = new QCheckBox();
    ledHeaderLayout->addWidget(ledNameLabel);
    ledHeaderLayout->addStretch(); // 将开关推到右侧
    ledHeaderLayout->addWidget(ledSwitch);
    ledBrightnessSlider = new QSlider(Qt::Horizontal);
    ledBrightnessSlider->setRange(0, 100);
    ledBrightnessSlider->setValue(50);
    ledControlLayout->addLayout(ledHeaderLayout);
    ledControlLayout->addWidget(ledBrightnessSlider);
    controlPanelLayout->addWidget(ledControlItem);

    // 电机控制项
    motorControlItem = new QFrame();
    motorControlItem->setStyleSheet("background: #f8fafc; padding: 15px; border-radius: 12px; "
                                    "margin-bottom: 12px; border: 1px solid #edf2f7;");
    motorControlItem->setMinimumHeight(140); // 为电机控制项设置更大最小高度
    motorControlLayout = new QVBoxLayout(motorControlItem);
    motorHeaderLayout = new QHBoxLayout();
    motorNameLabel = new QLabel(tr("直流电机控制"));
    motorSwitch = new QCheckBox();
    motorHeaderLayout->addWidget(motorNameLabel);
    motorHeaderLayout->addStretch();
    motorHeaderLayout->addWidget(motorSwitch);
    motorSpeedSlider = new QSlider(Qt::Horizontal);
    motorSpeedSlider->setRange(0, 100);
    motorSpeedSlider->setValue(30);
    motorDirectionLayout = new QHBoxLayout();
    dirForwardBtn = new QPushButton(tr("正向旋转"));
    dirReverseBtn = new QPushButton(tr("反向旋转"));
    dirForwardBtn->setCheckable(true);
    dirReverseBtn->setCheckable(true);
    dirForwardBtn->setChecked(true); // 默认为正转
    // 激活状态的样式 (模拟 .btn-dir.active)
    QString activeStyle = "QPushButton:checked { background: #fff; color: #5c67f2; border: 2px solid #5c67f2; font-weight: bold; }";
    QString inactiveStyle = "QPushButton { background: #e2e8f0; color: #64748b; padding: 6px; border-radius: 6px; border: 2px solid transparent; }";
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
    controlPanelLayout->addWidget(motorControlItem);

    // 蜂鸣器控制项
    buzzerControlItem = new QFrame();
    // 移除 border-left，改用背景色渐变或一个单独的 QFrame 作为边框
    buzzerControlItem->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #f59e0b, stop:0.01 #f8fafc, stop:1 #f8fafc); " // 左侧警告色边
                                    "padding: 15px; border-radius: 12px; "
                                    "margin-bottom: 0px; border: 1px solid #edf2f7;");
    buzzerControlItem->setMinimumHeight(70); // 稍微增加一点高度
    buzzerControlLayout = new QHBoxLayout(buzzerControlItem); // 水平布局用于名称和开关
    buzzerNameLabel = new QLabel(tr("紧急蜂鸣报警"));
    buzzerNameLabel->setStyleSheet("font-weight: bold;"); // 让名字更突出
    buzzerSwitch = new QCheckBox();
    buzzerControlLayout->addWidget(buzzerNameLabel);
    buzzerControlLayout->addStretch();
    buzzerControlLayout->addWidget(buzzerSwitch);
    controlPanelLayout->addWidget(buzzerControlItem);

    rightColumnLayout->addWidget(controlPanelCard);

    // 日志卡片
    logCard = new QGroupBox(tr("📜 系统运行日志"));
    logCardLayout = new QVBoxLayout(logCard);
    logTextBox = new QTextEdit();
    logTextBox->setReadOnly(true);
    logTextBox->setStyleSheet("background-color: #0f172a; color: #38bdf8; border-radius: 12px; "
                              "font-family: 'Courier New', Courier, monospace; font-size: 12px; "
                              "border: 1px solid #1e293b;"); // 模拟日志容器
    logCardLayout->addWidget(logTextBox);
    rightColumnLayout->addWidget(logCard, 1); // 拉伸因子 1 以填充剩余空间

    mainSplitter->addWidget(rightColumnWidget);
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
    });

    connect(ledBrightnessSlider, &QSlider::valueChanged, this, [this](int value) {
        if (ledSwitch->isChecked()) { // 仅在 LED 开启时记录日志
            emit ledControlChanged(ledSwitch->isChecked(), value);
            logTextBox->append(QString("[%1] LED亮度调整: %2%")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                              .arg(value));
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
    });

    connect(motorSpeedSlider, &QSlider::valueChanged, this, [this](int value) {
        if (motorSwitch->isChecked()) { // 仅在电机开启时记录日志
            bool dirForward = dirForwardBtn->isChecked();
            emit motorControlChanged(motorSwitch->isChecked(), value, dirForward);
            logTextBox->append(QString("[%1] 电机速度调整: %2%")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                              .arg(value));
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
    });

    connect(streamStartBtn, &QPushButton::clicked, this, [this]() {
        streamStartBtn->setEnabled(false);
        streamStopBtn->setEnabled(true);
        emit cameraStreamStartRequested();
        logTextBox->append(QString("[%1] ✅ 视频流已启动")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    });

    connect(streamStopBtn, &QPushButton::clicked, this, [this]() {
        streamStopBtn->setEnabled(false);
        streamStartBtn->setEnabled(true);
        emit cameraStreamStopRequested();
        logTextBox->append(QString("[%1] ⏹ 视频流已停止")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    });

    connect(snapshotBtn, &QPushButton::clicked, this, [this]() {
        emit cameraSnapshotRequested();
        logTextBox->append(QString("[%1] ✅ 拍照指令已发送")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    });

    connect(recordBtn, &QPushButton::clicked, this, [this]() {
        static bool recording = false; // 演示用的简单状态跟踪
        recording = !recording;
        if (recording) {
            recordBtn->setText(tr("⏹ 停止录制"));
            logTextBox->append(QString("[%1] ✅ 录像已开始")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else {
            recordBtn->setText(tr("🔴 视频录制"));
            logTextBox->append(QString("[%1] ✅ 录像已停止")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        }
        emit cameraRecordToggleRequested();
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
    // 如果电机开启，则记录更改
    if (motorSwitch->isChecked()) {
        bool dirForward = dirForwardBtn->isChecked();
        emit motorControlChanged(motorSwitch->isChecked(), motorSpeedSlider->value(), dirForward);
        logTextBox->append(QString("[%1] 电机方向切换: %2")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(dirForward ? tr("正转") : tr("反转")));
    }
}
