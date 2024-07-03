#include "mouseeventlabel.h"

MouseEventLabel::MouseEventLabel(QWidget *parent)
	: QLabel(parent)
{

}

//��갴ѹ�¼�
void MouseEventLabel::mousePressEvent(QMouseEvent* ev)
{
	emit mousePressd(ev->position().x());
}

//�������¼�
void MouseEventLabel::wheelEvent(QWheelEvent* ev)
{
	emit mousewheel(ev->angleDelta().y());//������������ƶ��ľ���
}

//���˫���¼�
void MouseEventLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
	Q_UNUSED(event);
	emit doubleClicked();
}

MouseEventLabel::~MouseEventLabel()
{}
