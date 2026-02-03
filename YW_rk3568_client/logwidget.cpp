#include "logwidget.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QDateTime>

LogWidget::LogWidget(QWidget *parent)
    : QFrame(parent)
{
    // 设置样式对象名称，以便应用全局样式表
    this->setObjectName("card");

    // 主布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 标题
    QLabel *titleLabel = new QLabel("📜 系统运行日志");
    titleLabel->setObjectName("cardTitle");
    layout->addWidget(titleLabel);

    // 日志文本框
    logTextBox = new QTextEdit();
    logTextBox->setObjectName("logTextBox");
    logTextBox->setReadOnly(true);
    layout->addWidget(logTextBox);
    logTextBox->setMinimumHeight(200);
    logTextBox->setMaximumHeight(300);
    logTextBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout->addWidget(logTextBox);
}

void LogWidget::appendLog(const QString &message)
{
    logTextBox->append(message);
}