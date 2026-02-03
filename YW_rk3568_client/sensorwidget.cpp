#include "sensorwidget.h"
#include <QVBoxLayout>

SensorWidget::SensorWidget(const QString &name, const QString &unit, QWidget *parent)
    : QFrame(parent)
{
    // 设置样式对象名称，以便应用全局样式表
    this->setObjectName("sensorCard");

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->setAlignment(Qt::AlignCenter);

    // 创建标签
    nameLabel = new QLabel(name);
    nameLabel->setObjectName("sensorNameLabel");
    nameLabel->setStyleSheet("font-weight: normal; font-size: 15px;");
    
    valueLabel = new QLabel("--");
    valueLabel->setObjectName("sensorValueLabel");
    valueLabel->setStyleSheet("font-size: 24px; font-weight: 800; color: #5c67f2; margin: 6px 0;");
    
    unitLabel = new QLabel(unit);
    unitLabel->setObjectName("sensorUnitLabel");
    unitLabel->setStyleSheet("font-size: 14px; color: #94a3b8;");

    // 添加到布局
    layout->addWidget(nameLabel);
    layout->addWidget(valueLabel);
    layout->addWidget(unitLabel);
}

void SensorWidget::setValue(const QString &value)
{
    valueLabel->setText(value);
}

void SensorWidget::setValueColor(const QString &color)
{
    valueLabel->setStyleSheet(QString("font-size: 24px; font-weight: 800; color: %1; margin: 6px 0;").arg(color));
}