#include "rk3568controlpanel.h"
#include <QApplication>
#include <QDateTime>
#include <QButtonGroup> // For motor direction radio button group


RK3568ControlPanel::RK3568ControlPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
    // Initialize log with startup message
    logTextBox->append(QString("[%1] 系统初始化完成...")
                      .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void RK3568ControlPanel::setupUI()
{
    // --- Main Window Layout ---
    mainLayout = new QVBoxLayout(this);
    this->setWindowTitle(tr("RK3568 智能网关控制系统"));
    this->resize(1200, 800); // Set initial size

    // --- Header ---
    headerWidget = new QWidget();
    headerLayout = new QHBoxLayout(headerWidget);
    headerWidget->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4f46e5, stop:1 #7c3aed); "
                               "color: white; border-radius: 16px; min-height: 60px; margin-bottom: 20px;");
    titleLabel = new QLabel(tr("RK3568 智能网关控制系统"));
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; letter-spacing: 1px;");
    statusLabel = new QLabel(tr("● 设备在线"));
    statusLabel->setStyleSheet("background-color: rgba(255, 255, 255, 30); padding: 4px 12px; border-radius: 20px; font-size: 13px;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(); // Push status label to the right
    headerLayout->addWidget(statusLabel);
    mainLayout->addWidget(headerWidget);

    // --- Main Splitter (Left and Right Columns) ---
    mainSplitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(mainSplitter);

    // --- Left Column ---
    leftColumnWidget = new QWidget();
    leftColumnLayout = new QVBoxLayout(leftColumnWidget);
    leftColumnWidget->setMinimumWidth(600); // Give left side more space initially

    // Video Card
    videoCard = new QGroupBox(tr("🎥 实时画面监控"));
    videoCardLayout = new QVBoxLayout(videoCard);
    videoDisplayLabel = new QLabel(tr("Video Stream Display Area"));
    videoDisplayLabel->setAlignment(Qt::AlignCenter);
    videoDisplayLabel->setStyleSheet("background-color: black; color: white; border-radius: 12px;"); // Simulate video box
    videoControlsLayout = new QGridLayout(); // Use grid for uniform button sizing
    streamStartBtn = new QPushButton(tr("▶ 开始推流"));
    streamStopBtn = new QPushButton(tr("⏹ 停止推流"));
    streamStopBtn->setEnabled(false); // Initially disabled
    snapshotBtn = new QPushButton(tr("📸 抓拍照片"));
    recordBtn = new QPushButton(tr("🔴 视频录制"));

    videoControlsLayout->addWidget(streamStartBtn, 0, 0);
    videoControlsLayout->addWidget(streamStopBtn, 0, 1);
    videoControlsLayout->addWidget(snapshotBtn, 0, 2);
    videoControlsLayout->addWidget(recordBtn, 0, 3);

    videoCardLayout->addWidget(videoDisplayLabel);
    videoCardLayout->addLayout(videoControlsLayout);
    leftColumnLayout->addWidget(videoCard);

    // Sensor Row
    sensorRowWidget = new QWidget();
    sensorGridLayout = new QGridLayout(sensorRowWidget);
    sensorRowWidget->setMaximumHeight(150); // Limit height for sensor row

    // Helper lambda to create a sensor card
    auto createSensorCard = [this](const QString &name, const QString &unit) -> SensorCard {
        SensorCard card;
        card.frame = new QFrame();
        card.frame->setStyleSheet("background: #fff; padding: 15px; border-radius: 12px; "
                                  "text-align: center; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.02);");
        QVBoxLayout *cardLayout = new QVBoxLayout(card.frame);
        card.nameLabel = new QLabel(name);
        card.valueLabel = new QLabel("--");
        card.valueLabel->setStyleSheet("font-size: 22px; font-weight: 800; color: #5c67f2; margin: 4px 0;");
        card.unitLabel = new QLabel(unit);
        card.unitLabel->setStyleSheet("font-size: 12px; color: #94a3b8;");
        cardLayout->addWidget(card.nameLabel);
        cardLayout->addWidget(card.valueLabel);
        cardLayout->addWidget(card.unitLabel);
        cardLayout->setContentsMargins(15, 15, 15, 15); // Padding inside frame
        cardLayout->setSpacing(4); // Spacing between elements
        cardLayout->setAlignment(Qt::AlignCenter); // Center content
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
    sensorGridLayout->setSpacing(15); // Gap between cards
    leftColumnLayout->addWidget(sensorRowWidget);

    mainSplitter->addWidget(leftColumnWidget);

    // --- Right Column ---
    rightColumnWidget = new QWidget();
    rightColumnLayout = new QVBoxLayout(rightColumnWidget);
    rightColumnWidget->setMinimumWidth(400);

    // Control Panel Card
    controlPanelCard = new QGroupBox(tr("⚙️ 硬件外设控制"));
    controlPanelLayout = new QVBoxLayout(controlPanelCard);

    // LED Control Item
    ledControlItem = new QFrame(); // Use QFrame for styling
    ledControlItem->setStyleSheet("background: #f8fafc; padding: 15px; border-radius: 12px; "
                                  "margin-bottom: 12px; border: 1px solid #edf2f7;");
    ledControlLayout = new QVBoxLayout(ledControlItem);
    ledHeaderLayout = new QHBoxLayout();
    ledNameLabel = new QLabel(tr("LED 照明灯"));
    ledSwitch = new QCheckBox();
    ledHeaderLayout->addWidget(ledNameLabel);
    ledHeaderLayout->addStretch(); // Push switch to the right
    ledHeaderLayout->addWidget(ledSwitch);
    ledBrightnessSlider = new QSlider(Qt::Horizontal);
    ledBrightnessSlider->setRange(0, 100);
    ledBrightnessSlider->setValue(50);
    ledControlLayout->addLayout(ledHeaderLayout);
    ledControlLayout->addWidget(ledBrightnessSlider);
    controlPanelLayout->addWidget(ledControlItem);

    // Motor Control Item
    motorControlItem = new QFrame();
    motorControlItem->setStyleSheet("background: #f8fafc; padding: 15px; border-radius: 12px; "
                                    "margin-bottom: 12px; border: 1px solid #edf2f7;");
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
    dirForwardBtn->setChecked(true); // Default to forward
    // Style for active state (simulating .btn-dir.active)
    QString activeStyle = "QPushButton:checked { background: #fff; color: #5c67f2; border: 2px solid #5c67f2; font-weight: bold; }";
    QString inactiveStyle = "QPushButton { background: #e2e8f0; color: #64748b; padding: 6px; border-radius: 6px; border: 2px solid transparent; }";
    dirForwardBtn->setStyleSheet(inactiveStyle + activeStyle);
    dirReverseBtn->setStyleSheet(inactiveStyle + activeStyle);

    motorDirectionLayout->addWidget(dirForwardBtn);
    motorDirectionLayout->addWidget(dirReverseBtn);

    motorControlLayout->addLayout(motorHeaderLayout);
    motorControlLayout->addWidget(motorSpeedSlider);
    motorControlLayout->addLayout(motorDirectionLayout);
    controlPanelLayout->addWidget(motorControlItem);

    // Buzzer Control Item
    buzzerControlItem = new QFrame();
    buzzerControlItem->setStyleSheet("background: #f8fafc; padding: 15px; border-radius: 12px; "
                                     "margin-bottom: 0px; border: 1px solid #edf2f7; border-left: 4px solid #f59e0b;"); // Warning color border
    buzzerControlLayout = new QHBoxLayout(buzzerControlItem); // Horizontal for name and switch
    buzzerNameLabel = new QLabel(tr("紧急蜂鸣报警"));
    buzzerSwitch = new QCheckBox();
    buzzerControlLayout->addWidget(buzzerNameLabel);
    buzzerControlLayout->addStretch();
    buzzerControlLayout->addWidget(buzzerSwitch);
    controlPanelLayout->addWidget(buzzerControlItem);

    rightColumnLayout->addWidget(controlPanelCard);

    // Log Card
    logCard = new QGroupBox(tr("📜 系统运行日志"));
    logCardLayout = new QVBoxLayout(logCard);
    logTextBox = new QTextEdit();
    logTextBox->setReadOnly(true);
    logTextBox->setStyleSheet("background-color: #0f172a; color: #38bdf8; border-radius: 12px; "
                              "font-family: 'Courier New', Courier, monospace; font-size: 12px; "
                              "border: 1px solid #1e293b;"); // Simulate log container
    logCardLayout->addWidget(logTextBox);
    rightColumnLayout->addWidget(logCard, 1); // Stretch factor 1 to fill remaining space

    mainSplitter->addWidget(rightColumnWidget);
}

void RK3568ControlPanel::setupConnections()
{
    // Connect control signals
    connect(ledSwitch, &QCheckBox::toggled, this, [this](bool checked) {
        emit ledControlChanged(checked, ledBrightnessSlider->value());
        logTextBox->append(QString("[%1] LED调整: %2, 亮度 %3%")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(checked ? tr("开启") : tr("关闭"))
                          .arg(ledBrightnessSlider->value()));
    });

    connect(ledBrightnessSlider, &QSlider::valueChanged, this, [this](int value) {
        if (ledSwitch->isChecked()) { // Only log if LED is on
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
        if (motorSwitch->isChecked()) { // Only log if motor is on
            bool dirForward = dirForwardBtn->isChecked();
            emit motorControlChanged(motorSwitch->isChecked(), value, dirForward);
            logTextBox->append(QString("[%1] 电机速度调整: %2%")
                              .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                              .arg(value));
        }
    });

    // Motor direction buttons are mutually exclusive
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
        static bool recording = false; // Simple state tracking for demo
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
    // Ensure only one direction button is checked
    if (QObject::sender() == dirForwardBtn) {
        if (dirForwardBtn->isChecked()) {
            dirReverseBtn->setChecked(false);
        } else {
            dirForwardBtn->setChecked(true); // Prevent both from being unchecked
        }
    } else if (QObject::sender() == dirReverseBtn) {
        if (dirReverseBtn->isChecked()) {
            dirForwardBtn->setChecked(false);
        } else {
            dirReverseBtn->setChecked(true); // Prevent both from being unchecked
        }
    }
    // Log the change if motor is on
    if (motorSwitch->isChecked()) {
        bool dirForward = dirForwardBtn->isChecked();
        emit motorControlChanged(motorSwitch->isChecked(), motorSpeedSlider->value(), dirForward);
        logTextBox->append(QString("[%1] 电机方向切换: %2")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(dirForward ? tr("正转") : tr("反转")));
    }
}