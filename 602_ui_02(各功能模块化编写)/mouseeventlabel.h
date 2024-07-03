#pragma once

#include <QObject>
#include<QLabel>
#include<qevent.h>
#include<qwidget.h>
#include<QWheelEvent>

//鼠标事件类
class MouseEventLabel  : public QLabel
{
	Q_OBJECT

public:
	MouseEventLabel(QWidget*parent);
	~MouseEventLabel();

	//鼠标按压事件
	virtual void mousePressEvent(QMouseEvent* ev);

	//鼠标滚轮事件
	virtual void wheelEvent(QWheelEvent* ev);

	//鼠标双击事件
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

signals:
	void mousePressd(int x);
	void mousewheel(int x);
	void doubleClicked();
};
