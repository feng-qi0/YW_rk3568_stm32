#include "controlpanelwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

ControlPanelWidget::ControlPanelWidget(QWidget *parent)
    : QFrame(parent)
{
    setupUI();
}

void ControlPanelWidget::setupUI()
{
    // 设置样式对象名称，以便应用全局样式表
    this->setObjectName("card");

    // 主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 标题
    QLabel *titleLabel = new QLabel("⚙️ 硬件外设控制");
    titleLabel->setObjectName("cardTitle");
    titleLabel->setStyleSheet("font-size: 1.1rem; font-weight: 700; margin-bottom: 15px; "
                              "color: #1f2937;");
    mainLayout->addWidget(titleLabel);

    // LED 控制项
    ledControlItem = new QFrame();
    ledControlItem->setObjectName("controlItem");
    ledControlItem->setMinimumHeight(120);
    ledControlItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    ledControlLayout = new QVBoxLayout(ledControlItem);
    ledHeaderLayout = new QHBoxLayout();

    ledNameLabel = new QLabel("LED 照明灯");
    ledNameLabel->setStyleSheet("font-weight: 600; font-size: 15px; min-height: 20px;");

    ledSwitch = new QCheckBox();
    ledSwitch->setObjectName("modern"); // 使用现代复选框样式，带勾号
    ledSwitch->setStyleSheet(""); // 确保不使用内联样式覆盖QSS
    ledSwitch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // 确保复选框大小固定

    ledHeaderLayout->addWidget(ledNameLabel);
    ledHeaderLayout->addStretch();
    ledHeaderLayout->addWidget(ledSwitch);
    
    ledBrightnessSlider = new QSlider(Qt::Horizontal);
    ledBrightnessSlider->setRange(0, 100);
    ledBrightnessSlider->setValue(50);
    
    ledControlLayout->addLayout(ledHeaderLayout);
    ledControlLayout->addWidget(ledBrightnessSlider);
    ledControlLayout->setContentsMargins(8, 8, 8, 8);
    ledControlLayout->setSpacing(10);

    // 电机控制项
    motorControlItem = new QFrame();
    motorControlItem->setObjectName("controlItem");
    motorControlItem->setMinimumHeight(180);
    motorControlItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    motorControlLayout = new QVBoxLayout(motorControlItem);
    motorHeaderLayout = new QHBoxLayout();

    motorNameLabel = new QLabel("直流电机控制");
    motorNameLabel->setStyleSheet("font-weight: 600; font-size: 15px; min-height: 20px;");

    motorSwitch = new QCheckBox();
    motorSwitch->setObjectName("modern"); // 使用现代复选框样式，带勾号
    motorSwitch->setStyleSheet(""); // 确保不使用内联样式覆盖QSS
    motorSwitch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // 确保复选框大小固定

    motorHeaderLayout->addWidget(motorNameLabel);
    motorHeaderLayout->addStretch();
    motorHeaderLayout->addWidget(motorSwitch);
    
    motorSpeedSlider = new QSlider(Qt::Horizontal);
    motorSpeedSlider->setRange(0, 100);
    motorSpeedSlider->setValue(30);
    
    motorDirectionLayout = new QHBoxLayout();
    dirForwardBtn = new QPushButton("正向旋转");
    dirReverseBtn = new QPushButton("反向旋转");
    dirForwardBtn->setCheckable(true);
    dirReverseBtn->setCheckable(true);
    dirForwardBtn->setChecked(true); // 默认为正转

    // 设置对象名称以便全局样式表可以应用样式
    dirForwardBtn->setObjectName("dirForwardBtn");
    dirReverseBtn->setObjectName("dirReverseBtn");

    // 为方向按钮设置最小尺寸
    dirForwardBtn->setMinimumSize(110, 45);
    dirReverseBtn->setMinimumSize(110, 45);
    
    // 创建按钮组以确保互斥选择
    directionGroup = new QButtonGroup(this);
    directionGroup->addButton(dirForwardBtn, 0);
    directionGroup->addButton(dirReverseBtn, 1);
    
    motorDirectionLayout->addWidget(dirForwardBtn);
    motorDirectionLayout->addWidget(dirReverseBtn);

    motorControlLayout->addLayout(motorHeaderLayout);
    motorControlLayout->addWidget(motorSpeedSlider);
    motorControlLayout->addLayout(motorDirectionLayout);
    motorControlLayout->setContentsMargins(8, 8, 8, 8);
    motorControlLayout->setSpacing(10);

    // 蜂鸣器控制项
    buzzerControlItem = new QFrame();
    buzzerControlItem->setObjectName("buzzerControlItem");
    buzzerControlItem->setMinimumHeight(80);
    buzzerControlItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    buzzerControlLayout = new QHBoxLayout(buzzerControlItem);
    buzzerNameLabel = new QLabel("紧急蜂鸣报警");
    buzzerNameLabel->setStyleSheet("font-weight: 600; font-size: 15px; min-height: 20px;");
    buzzerSwitch = new QCheckBox();
    buzzerSwitch->setObjectName("modern"); // 使用现代复选框样式，带勾号
    buzzerSwitch->setStyleSheet(""); // 确保不使用内联样式覆盖QSS
    buzzerSwitch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // 确保复选框大小固定

    buzzerControlLayout->addWidget(buzzerNameLabel);
    buzzerControlLayout->addStretch();
    buzzerControlLayout->addWidget(buzzerSwitch);
    buzzerControlLayout->setContentsMargins(5, 5, 5, 5);

    // 添加到主布局
    mainLayout->addWidget(ledControlItem);
    mainLayout->addWidget(motorControlItem);
    mainLayout->addWidget(buzzerControlItem);

    // 连接信号槽
    connect(ledSwitch, &QCheckBox::toggled, this, &ControlPanelWidget::onLedSwitchToggled);
    connect(ledBrightnessSlider, &QSlider::valueChanged, this, &ControlPanelWidget::onLedBrightnessChanged);
    connect(motorSwitch, &QCheckBox::toggled, this, &ControlPanelWidget::onMotorSwitchToggled);
    connect(motorSpeedSlider, &QSlider::valueChanged, this, &ControlPanelWidget::onMotorSpeedChanged);
    connect(directionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &ControlPanelWidget::onMotorDirectionChanged);
    connect(buzzerSwitch, &QCheckBox::toggled, this, &ControlPanelWidget::onBuzzerSwitchToggled);
}

// Getters
bool ControlPanelWidget::getLedState() const
{
    return ledSwitch->isChecked();
}

int ControlPanelWidget::getLedBrightness() const
{
    return ledBrightnessSlider->value();
}

bool ControlPanelWidget::getMotorState() const
{
    return motorSwitch->isChecked();
}

int ControlPanelWidget::getMotorSpeed() const
{
    return motorSpeedSlider->value();
}

bool ControlPanelWidget::getMotorDirection() const
{
    return directionGroup->checkedId() == 0;  // true for forward (ID 0), false for reverse (ID 1)
}

bool ControlPanelWidget::getBuzzerState() const
{
    return buzzerSwitch->isChecked();
}

// Setters
void ControlPanelWidget::setLedState(bool enabled)
{
    ledSwitch->setChecked(enabled);
}

void ControlPanelWidget::setLedBrightness(int brightness)
{
    ledBrightnessSlider->setValue(brightness);
}

void ControlPanelWidget::setMotorState(bool enabled)
{
    motorSwitch->setChecked(enabled);
}

void ControlPanelWidget::setMotorSpeed(int speed)
{
    motorSpeedSlider->setValue(speed);
}

void ControlPanelWidget::setMotorDirection(bool forward)
{
    if (forward) {
        dirForwardBtn->setChecked(true);
    } else {
        dirReverseBtn->setChecked(true);
    }
}

void ControlPanelWidget::setBuzzerState(bool enabled)
{
    buzzerSwitch->setChecked(enabled);
}

void ControlPanelWidget::onLedSwitchToggled(bool checked)
{
    emit ledControlChanged(checked, ledBrightnessSlider->value());
}

void ControlPanelWidget::onLedBrightnessChanged(int value)
{
    if (ledSwitch->isChecked()) {
        emit ledControlChanged(ledSwitch->isChecked(), value);
    }
}

void ControlPanelWidget::onMotorSwitchToggled(bool checked)
{
    bool dirForward = directionGroup->checkedId() == 0;
    emit motorControlChanged(checked, motorSpeedSlider->value(), dirForward);
}

void ControlPanelWidget::onMotorSpeedChanged(int value)
{
    if (motorSwitch->isChecked()) {
        bool dirForward = directionGroup->checkedId() == 0;
        emit motorControlChanged(motorSwitch->isChecked(), value, dirForward);
    }
}

void ControlPanelWidget::onMotorDirectionChanged()
{
    bool dirForward = directionGroup->checkedId() == 0;
    emit motorControlChanged(motorSwitch->isChecked(), motorSpeedSlider->value(), dirForward);
}

void ControlPanelWidget::onBuzzerSwitchToggled(bool checked)
{
    emit buzzerControlChanged(checked);
}