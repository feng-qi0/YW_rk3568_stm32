#include "widget.h"
#include <QApplication>
// #include "ui_widget.h" // 不再需要包含 ui_widget.h

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    // , ui(new Ui::Widget) // 不再需要 ui 成员
    , controlPanel(nullptr) // 初始化 controlPanel 指针
{
    // ui->setupUi(this); // 不再设置 Widget 的 UI

    // 创建 RK3568ControlPanel 实例
    controlPanel = new RK3568ControlPanel(this);

    // 设置主窗口属性
    this->setWindowTitle(tr("RK3568 智能网关控制系统"));
    this->resize(1400, 900);

    // 将控制面板设置为主窗口的布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(controlPanel);
    layout->setContentsMargins(0, 0, 0, 0); // 移除边距

    // 显示主窗口
    this->show();

    // 隐藏原始的Widget窗口，只显示控制面板
    // this->hide();
}

Widget::~Widget()
{
    // delete ui; // 不再需要删除 ui
    // controlPanel 会被 Qt 的父子关系自动删除，因为它是在构造函数中以 'this' 为父对象创建的
    // 如果在构造函数中没有指定父对象，则需要在这里手动删除
    // delete controlPanel;
}

