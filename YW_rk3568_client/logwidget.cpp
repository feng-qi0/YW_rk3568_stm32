#include "logwidget.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QDateTime>

LogWidget::LogWidget(QWidget *parent)
    : QFrame(parent)
{
    // 设置样式
    this->setObjectName("card");
    this->setStyleSheet("QFrame#card {"
                        "    background: #ffffff;"
                        "    border-radius: 16px;"
                        "    padding: 20px;"
                        "    border: 1px solid #e2e8f7;"
                        "}");

    // 主布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 标题
    QLabel *titleLabel = new QLabel("📜 系统运行日志");
    titleLabel->setObjectName("cardTitle");
    titleLabel->setStyleSheet("font-size: 1.1rem; font-weight: 700; margin-bottom: 15px; "
                              "display: flex; align-items: center; gap: 8px; color: #1f2937;");
    layout->addWidget(titleLabel);

    // 日志文本框
    logTextBox = new QTextEdit();
    logTextBox->setObjectName("logTextBox");
    logTextBox->setReadOnly(true);
    logTextBox->setStyleSheet("#logTextBox {"
                              "    background-color: #0f172a;"
                              "    color: #38bdf8;"
                              "    border-radius: 12px;"
                              "    font-family: 'Courier New', Courier, monospace;"
                              "    font-size: 12px;"
                              "    border: 1px solid #1e293b;"
                              "    padding: 15px;"
                              "}");
    logTextBox->setMinimumHeight(200);
    logTextBox->setMaximumHeight(300);
    logTextBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout->addWidget(logTextBox);
}

void LogWidget::appendLog(const QString &message)
{
    logTextBox->append(message);
}