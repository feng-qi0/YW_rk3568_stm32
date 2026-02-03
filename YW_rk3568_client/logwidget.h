#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>

class LogWidget : public QFrame
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = nullptr);

    void appendLog(const QString &message);

private:
    QTextEdit *logTextBox;
};

#endif // LOGWIDGET_H