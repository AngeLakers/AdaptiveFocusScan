#include "motion_ui.h"

Motion_ui::Motion_ui(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//初始化各控件位置
	set_ui_position();
}

//初始化各控件位置
void Motion_ui::set_ui_position()
{
	setWindowTitle("Motion control");
	setFixedSize(740, 350);

	//相关group设置
	ui.groupBox->setGeometry(10, 10, 420, 325);
	ui.groupBox_2->setGeometry(440, 10, 390, 325);

	ui.connect_duoji_pushButton->setGeometry(20, 20, 94, 28);
	ui.disconnect_duoji_pushButton->setGeometry(130, 20, 94, 28);
	ui.groupBox_3->setGeometry(10, 60, 270, 80);
	ui.groupBox_5->setGeometry(10, 150, 270, 80);
	ui.groupBox_6->setGeometry(10, 240, 270, 80);

	//舵机移动
    //设定相关控件的范围，角度限制皆为500范围，舵机一圈4095
    
    //短边,范围0~800
    ui.S_angle_doubleSpinBox->setRange(0, (360.0 / 4095.0) * 800);
    ui.S_angle_doubleSpinBox->setSingleStep(0.1);
    ui.S_angle_doubleSpinBox->setDecimals(1);//数值显示小数点后一位
    ui.S_duoji_horizontalSlider->setRange(0, 800);
    //Slider控件
    connect(ui.S_duoji_horizontalSlider, &QSlider::valueChanged, [=](int value) {
        //更新目标位置
        double num = (360.0 / 4095.0) * value;
        ui.S_angle_doubleSpinBox->setValue(num);//减去了中间差值
        //在地址为128的寄存器中的第0号位置写入数据
        emit writeToclient(128, value, 1);
        });
    //Spinbox控件与确认输入
    connect(ui.S_angle_doubleSpinBox, &QDoubleSpinBox::editingFinished, [=]() {
        ui.S_duoji_horizontalSlider->setValue(ui.S_angle_doubleSpinBox->value() * (4095.0 / 360.0));
        });


    //长边，角度限制为0~800
    ui.L_angle_doubleSpinBox->setRange(0, (360.0 / 4095.0) * 800);
    ui.L_angle_doubleSpinBox->setSingleStep(0.1);
    ui.L_angle_doubleSpinBox->setDecimals(1);//数值显示小数点后一位
    ui.L_duoji_horizontalSlider->setRange(0, 800);
    //Slider控件
    connect(ui.L_duoji_horizontalSlider, &QSlider::valueChanged, [=](int value) {
        //更新目标位置
        double num = (360.0 / 4095.0) * value;
        ui.L_angle_doubleSpinBox->setValue(num);//减去了中间差值
        //在地址为128的寄存器中的第0号位置写入数据
        emit writeToclient(128, value, 2);
        });
    //Spinbox控件与确认输入按钮
    connect(ui.L_angle_doubleSpinBox, &QDoubleSpinBox::editingFinished, [=]() {
        ui.L_duoji_horizontalSlider->setValue(ui.L_angle_doubleSpinBox->value() * (4095.0 / 360.0));
        });


    //R角 角度限制范围0~1000
    ui.R_angle_doubleSpinBox->setRange(0, (360.0 / 4095.0) * 1000);
    ui.R_angle_doubleSpinBox->setSingleStep(0.1);
    ui.R_angle_doubleSpinBox->setDecimals(1);//数值显示小数点后一位
    ui.R_duoji_horizontalSlider->setRange(0, 1000);
    //Slider控件
    connect(ui.R_duoji_horizontalSlider, &QSlider::valueChanged, [=](int value) {

        //更新目标位置
        double num = (360.0 / 4095.0) * value;
        ui.R_angle_doubleSpinBox->setValue(num);//减去了中间差值
        //在地址为128的寄存器中的第0号位置写入数据
        emit writeToclient(128, value, 3);

        });
    //Spinbox控件与确认输入按钮
    connect(ui.R_angle_doubleSpinBox, &QDoubleSpinBox::editingFinished, [=]() {
        ui.R_duoji_horizontalSlider->setValue(ui.R_angle_doubleSpinBox->value() * (4095.0 / 360.0));
        });
}

Motion_ui::~Motion_ui()
{}
