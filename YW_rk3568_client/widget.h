#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "rk3568controlpanel.h" // 包含新窗口的头文件

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    RK3568ControlPanel *controlPanel; // 声明新窗口的指针
};
#endif // WIDGET_H
