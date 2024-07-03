#pragma once

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QModbusDataUnit>
#include <QModbusRtuSerialMaster>
#include<qvariant.h>
#include<qmodbusrtuserialclient.h>
#include<qmessagebox.h>

class Servocontrol  : public QObject
{
	Q_OBJECT

public:
	Servocontrol(QObject *parent = Q_NULLPTR);
	~Servocontrol();

	//舵机相关
	QModbusClient* modbustoolDevice;//连接串口
	QModbusDataUnit readUnit;//读取舵机位置信息数据单元，用于发送数据到从机

	//连接舵机
	void connectToservo();
	//断开舵机连接
	void disconnectToservo();

	//上位机向下位机的某个寄存器写入数据 参数为寄存器地址，写入数据(regStartAddr为调整角度寄存器，地址为128，
	// number为写入数据为转动目标位置0~4095，serverAddress为从机地址，01，02，03)
	void writeToclient(unsigned int regStartAddr, unsigned int number,unsigned int serverAddress);

	//读取位置信息数据，scanmodel_SLR表示要读取哪个从机的位置
	double readToclient(int scanmodel_SLR);
	int live_angle_value = 0;
	double live_angle = 0;
};
