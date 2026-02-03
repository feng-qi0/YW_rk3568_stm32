#include "widget.h"
// #include "ui_widget.h" // 不再需要包含 ui_widget.h

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    // , ui(new Ui::Widget) // 不再需要 ui 成员
    , controlPanel(nullptr) // 初始化 controlPanel 指针
{
    // ui->setupUi(this); // 不再设置 Widget 的 UI

    // 创建 RK3568ControlPanel 实例
    controlPanel = new RK3568ControlPanel(this);

    // 可以选择隐藏当前的 Widget 窗口，只显示新窗口
    // this->hide();

    // 或者，将新窗口设置为主窗口的中央部件（如果 Widget 本身是主窗口）
    // 但通常我们会直接让 RK3568ControlPanel 成为主窗口
    // 这里我们直接显示新窗口
    controlPanel->show();

    // 如果你想让 Widget 窗口作为新窗口的父窗口（非模态对话框形式）
    // controlPanel->setParent(this);
    // controlPanel->show();
    // this->show(); // 然后显示父窗口
}

Widget::~Widget()
{
    // delete ui; // 不再需要删除 ui
    // controlPanel 会被 Qt 的父子关系自动删除，因为它是在构造函数中以 'this' 为父对象创建的
    // 如果在构造函数中没有指定父对象，则需要在这里手动删除
    // delete controlPanel;
}

