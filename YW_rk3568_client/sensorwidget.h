#ifndef SENSORWIDGET_H
#define SENSORWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

class SensorWidget : public QFrame
{
    Q_OBJECT

public:
    explicit SensorWidget(const QString &name, const QString &unit, QWidget *parent = nullptr);

    void setValue(const QString &value);
    void setValueColor(const QString &color);

private:
    QLabel *nameLabel;
    QLabel *valueLabel;
    QLabel *unitLabel;
};

#endif // SENSORWIDGET_H