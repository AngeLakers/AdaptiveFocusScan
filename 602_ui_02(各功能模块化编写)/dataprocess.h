#pragma once

#include <QObject>
#include<qvector.h>
#include"Line_rule_struct.h"
#include<qdebug.h>
#include<qimage.h>

class DataProcess : public QObject
{
	Q_OBJECT

public:
	DataProcess(QObject* parent = Q_NULLPTR);
	~DataProcess();

	//从parameter_ui中接收线扫法则数据
	void getLine_rule(Line_rule line_rule);
	Line_rule myline_rule;//线扫法则数据
	int signal_num;//信号量

	//从主线程获取数据函数
	void getdata(QByteArray data);
	QByteArray Data;//存储数据

	int ys = 0;
	int B_Scan_label_height = 600;
	int B_Scan_label_width = 800;
	int C_Scan_label_width = 1100;

	//B扫
	void showBscan();//处理实时B扫数据及发送A扫数据至主线程
	QImage BscaledImage;//B扫缩放图

	//C扫
	void showCscan();
	void ys_door_changed(int ys, int top_door, int bot_door, int left_door, int right_door);	//延时，闸门改变时，相应的操作
	QImage CImage;
	QImage CscaledImage;//C扫缩放图
	int Cscandex;//用于C扫图像的索引
	int CscanStartsignaldex;//C扫开始信号索引，对第几条信号开始成像
	int CscanStopsignaldex;;//C扫结束信号索引，到第几条信号结束成像
	int CImageHeight;//有多少条信号用于成像
	int startdex;//信号的第几个点进行成像
	int signallength;//信号的长度，选取这个长度范围内的最大值进行成像

	//图像颜色显示
	QColor interpolateColors(const QColor& startColor, const QColor& endColor, qreal ratio);
	QColor valuetocolor(int value);
	QColor customBlue = QColor(60, 115, 217);//天蓝色

	//编码器
	//编码器正向反向指数，0为正，1为负
	int encoderdex = 0;//默认为正向
	// 编码器历史位置
	int encoderlastpos;
	void getencoderpos();//获取编码器位置函数
	bool EncoderCscanstatus = false;//编码器扫查状态  true:接收数据时实时读取编码器位置，false:接收数据时，不读取编码器位置
	int EncoderCscandex;//前100读取位置相差过大直接return
	//编码器扫查精度
	int encoder_accuracy = 0.5 * 144;//默认扫查精度为0.05mm,144为系数
	bool encoder_first_scanstatus;//判断是不是第一次编码器扫查，true:为第一次扫查，false:为第二次扫查

	bool datastatus;//datastatus为ture,保存数据至二进制文件，为false，二进制文件删除一次数据


signals:
	//将处理好的A、B、C扫数据发送至主线程中
	void sendAscanData(QVector<int>);
	void sendBscanImage(QImage);
	void sendCscanImage(QImage);

	void sendEncoderpos(int pos);//发送编码器位置
	void startEncoderCscan();//子线程发送信号，让C扫函数showCscan()运行在子线程中,为什么无法向同一线程中的其他函数传参？
	void sendDataToSave(QVector<int>, int Cscandex, int signal_num);//QVector传递不用引用的方式，因为是跨线程传递数据，用引用可能会导致多个线程访问统一数据
	void deletedata(int signal_num);
};
