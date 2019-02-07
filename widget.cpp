#include "widget.h"
#include "ui_widget.h"
#include <QHBoxLayout>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    oglwidget = new OGLWidget(this);
    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(oglwidget);
    layout->setMargin(0);
    this->setLayout(layout);
}

Widget::~Widget()
{
    delete ui;
}
