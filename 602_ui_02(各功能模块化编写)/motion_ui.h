#pragma once

#include <QWidget>
#include "ui_motion_ui.h"
#include<qdebug.h>

//解决中文输出乱码
#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

class Motion_ui : public QWidget
{
	Q_OBJECT

public:
	Motion_ui(QWidget *parent = nullptr);
	~Motion_ui();

	//初始化各控件位置
	void set_ui_position();
	Ui::Motion_uiClass ui;

private:

signals:
	void writeToclient(unsigned int regStartAddr, unsigned int number, unsigned int serverAddress);
	
};
