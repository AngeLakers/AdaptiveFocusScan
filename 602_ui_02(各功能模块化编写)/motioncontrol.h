#pragma once

#include <QObject>
#include"LTDMC.h"
#include<qmessagebox.h>
#include"Windows.h"
#include<qdebug.h>

//解决中文输出乱码
#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

class Motioncontrol : public QObject
{
	Q_OBJECT

public:
	Motioncontrol(QObject* parent = Q_NULLPTR);
	~Motioncontrol();

	//停止状态，当到限位，或者按了停止键，轴停止运动，true:停止运动，false:可以运动
	bool axis_stopstatus;

	//常规定长运动，连续运动函数，在主线程中调用，在主线程中执行
	//自动扫查函数利用多线程，在子线程中执行

	//热复位
	void axis_reset();

	//所有轴使能
	void axis_enable();

	//所有轴失能
	void axis_disable();

	//x轴定长运动
	void X_axis_pmove(double speed, double len);//轴运动速度，运动距离（相对坐标模式，正值，正方向运动，负值，负方向运动）

	//x轴连续运动
	void X_axis_vmove(double speed, int dir);//轴运动速度，运动方向

	//x轴停止
	void X_axis_stop();

	//其余轴定长运动
	void other_axis_pmove(int axis, double speed, double len);

	//其余轴连续运动
	void other_axis_vmove(int axis, double speed, int dir);

	//其余轴停止
	void other_axis_stop(int axis);

	//紧急停止
	void stop_all_axis();

	//读取编码器位置
	double get_axis_position(WORD axis);

	//所有轴编码器位置归零
	void set_axis_pos_zero();

	//自动扫查
	void auto_scan(QVector<QVector<double>>pos_Vector, int scan_model);

signals:
	//启动舵机调姿
	//写入数据(regStartAddr为调整角度寄存器，地址为128，number为写入数据为转动目标位置0~4095，serverAddress为从机地址，01，02，03)
	void start_duoji(unsigned int regStartAddr, unsigned int number, unsigned int serverAddress);
};
