#pragma once

#include <QObject>
#include<qtcpsocket.h>
#include<QJsonObject>
#include<QJsonDocument>
#include<QNetworkProxy>//包含这个头文件，可以调用setProxy(QNetworkProxy::NoProxy)，挂VPN的时候也能连接上22S
#include<qmessagebox.h>
#include<QRegularExpression>//为了使用正则表达式
#include"Line_rule_struct.h"

//解决中文输出乱码
#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

class Pa22scontrol : public QObject
{
	Q_OBJECT

public:
	Pa22scontrol(QObject* parent = Q_NULLPTR);//将父对象设为null，防止报错无默认构造对象
	~Pa22scontrol();

	//Tcp通信接口
	QTcpSocket* order_tcp;
	QTcpSocket* data_tcp;

	//22s设备ip地址
	QString Pa22s_ip = "192.168.22.121";

	//22s指令端口号
	unsigned short port1 = 51007;
	//数据端
	unsigned short port2 = 51005;

	//连接22s
	void connect_Pa22s();
	//断开22s连接
	void disconnect_Pa22s();
	//关机
	void turn_off_Pa22s();
	//扫查信号开
	void open_signal();
	//扫查信号关
	void close_signal();
	//传输指令函数
	void transferinstruction(QJsonObject jsonObject);
	//获取22s芯片温度
	QString gettemprature();
	//设置数字增益
	void setdgain(double dgain);
	//设置发射电压
	void setvolt(int volt);
	//设置脉冲宽度
	void setpulsewidth(int pulse_width);
	//设置增益1,2
	void setvga(int vga1, int vga2);
	//设置重复频率
	void setprf(int prf);
	//设置压缩比
	void setrangeratio(double range_ratio);
	//设置数字滤波
	void setdfilter(int dfilter);
	//设置视频检波
	void setvideodetect(int video_detect);
	//设置线扫法则  默认无斜楔
	void setpalscan(Line_rule line_rule);//设置线扫法则
	Line_rule myline_rule;//线扫法则数据
	//编码器清零
	void resetencoder();

	//接收数据
	void receive_data();
	//接收数据
	QByteArray receivedData;
	QByteArray Data;

signals:
	void sendData(QByteArray Data);

};
