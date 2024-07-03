#pragma once

#include <QObject>
#include<qvector.h>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include<qdebug.h>
#include<qthread.h>
#include<QCoreApplication>
#include<QFileDialog>

class DataSave : public QObject
{
	Q_OBJECT

public:
	DataSave(QObject* parent = Q_NULLPTR);
	~DataSave();

	//临时文件位置
	QString tempfilePath;

	void savetempdata(QVector<int> newData, int TimeCscandex, int signal_num);//存储临时数据
	void savedata(QString);//将临时数据转存为正式数据
	void startsave(int signal_num, int top, int bot, int left, int right, int ys);//存入信号数量（a扫信号条数）,闸门在上下左右的位置，及创建临时文件
	void stopsave(int TimeCscandex, int signal_num);//存入C扫数据长度

signals:
	void progressUpdated(int);//数据保存进度
};
