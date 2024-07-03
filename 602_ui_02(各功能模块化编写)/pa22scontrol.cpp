#include "pa22scontrol.h"


Pa22scontrol::Pa22scontrol(QObject* parent)
    : QObject(parent)
{
    //实例化套接字对象
    order_tcp = new QTcpSocket;
    data_tcp = new QTcpSocket;

    //数据接收
    connect(data_tcp, &QTcpSocket::readyRead, this, &Pa22scontrol::receive_data);

    //初始化线扫法则
    myline_rule.elm_num = 64;
    myline_rule.pitch = 0.6;
    myline_rule.aperture_size = 29;
    myline_rule.focus_angle = 0;
    myline_rule.focus_distance = 0;
    myline_rule.sound_velocity = 5900;
    myline_rule.start_elm = 1;
    myline_rule.end_elm = 64;

}

//连接22s
void Pa22scontrol::connect_Pa22s()
{
    //每次连接前，先将数据清零
    receivedData.clear();
    //禁用代理(防止挂了梯子无法连接，以增强程序的鲁棒性)
    order_tcp->setProxy(QNetworkProxy::NoProxy);
    data_tcp->setProxy(QNetworkProxy::NoProxy);

    //连接
    order_tcp->connectToHost(Pa22s_ip, port1);
    data_tcp->connectToHost(Pa22s_ip, port2);

    //连接成功时，执行下一步操作
    //指令端连接成功时
    connect(order_tcp, &QTcpSocket::connected, [=]() {
        qDebug() << "指令端成功连接";
        });
    //数据端连接成功  
    connect(data_tcp, &QTcpSocket::connected, [=]() {
        //执行相应操作
        QMessageBox msgBox;
        msgBox.setText("连接成功！");
        msgBox.setWindowTitle("提示");
        msgBox.setWindowFlags(msgBox.windowFlags() | Qt::WindowStaysOnTopHint);
        msgBox.exec();

        //检波
        QJsonObject jsonObject8;
        jsonObject8["rectify"] = "full_wave";
        transferinstruction(jsonObject8);

        //设置数据长度，设置为512长
        QJsonObject jsonObject2;
        jsonObject2["a_data_len"] = 512;
        transferinstruction(jsonObject2);

        //设置A波平滑 
        QJsonObject jsonObject11;
        jsonObject11["a_smooth"] = 0;
        transferinstruction(jsonObject11);

        //编码器清零
        QJsonObject jsonObject12;
        jsonObject12["encoder_reset"] = QJsonObject{
            {"idx", 0}
        };
        transferinstruction(jsonObject12);

        //设置线扫法则
        setpalscan(myline_rule);

        });
}

//断开22s连接
void Pa22scontrol::disconnect_Pa22s()
{
    //停止数据采集
    QJsonObject jsonObject;
    jsonObject["scan"] = 0;
    transferinstruction(jsonObject);
    //断开Socket
    order_tcp->close();
    data_tcp->close();

    //只要客户端与服务器的连接断开了，就会发出disconnected信号
    connect(order_tcp, &QTcpSocket::disconnected, this, [=]() {
        order_tcp->close();
        });

    //数据端断开
    connect(data_tcp, &QTcpSocket::disconnected, this, [=]() {
        data_tcp->close();
        if (!order_tcp->isOpen()) {
            QMessageBox msgBox;
            msgBox.setText("断开连接！");
            msgBox.setWindowTitle("提示");
            msgBox.setWindowFlags(msgBox.windowFlags() | Qt::WindowStaysOnTopHint);
            msgBox.exec();

            qDebug() << "客户端已断开";

            //停止B扫数据采集
            QJsonObject jsonObject;
            jsonObject["scan"] = 0;
            transferinstruction(jsonObject);
        }


        });
}

//关机
void Pa22scontrol::turn_off_Pa22s()
{
    //消息提示框能够在顶层显示
    QMessageBox* messageBox = new QMessageBox(NULL);
    messageBox->setWindowTitle("提示");
    messageBox->setText("确认关机？");
    messageBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox->setDefaultButton(QMessageBox::No); // 默认选中 No 按钮

    // 设置消息框总在顶层显示
    messageBox->setWindowFlags(messageBox->windowFlags() | Qt::WindowStaysOnTopHint);

    // 显示消息框并等待用户响应
    int ref = messageBox->exec();

    // 根据用户的选择执行不同操作
    if (ref == QMessageBox::Yes)
    {
        // 用户点击了 Yes 按钮，执行关机指令
        QJsonObject jsonObjectclose;
        jsonObjectclose["power"] = "off";
        transferinstruction(jsonObjectclose);
    }
    else if (ref == QMessageBox::No)
    {
        // 用户点击了 No 按钮，不执行任何操作
        return;
    }
}

//扫查信号开
void Pa22scontrol::open_signal()
{
    //每次连接前，先将数据清零
    receivedData.clear();

    QJsonObject jsonObject;
    jsonObject["scan"] = 1;
    transferinstruction(jsonObject);
}

//扫查信号关
void Pa22scontrol::close_signal()
{
    QJsonObject jsonObject;
    jsonObject["scan"] = 0;
    transferinstruction(jsonObject);
}

//传输指令函数
void Pa22scontrol::transferinstruction(QJsonObject jsonObject)
{
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonData = jsonDocument.toJson(QJsonDocument::Compact);
    jsonData.append(char(0x1E));
    qDebug() << jsonData;
    order_tcp->write(jsonData);
    order_tcp->waitForBytesWritten();
    order_tcp->waitForReadyRead();

    QByteArray receivedData = order_tcp->readAll();
    qDebug() << receivedData;
    //判断是否设置成功
    QString str = QString::fromUtf8(receivedData);
    if (str.contains("error"))
    {
        QMessageBox::warning(nullptr, "警告", "数据设置错误");//不指定父对象，一般窗口类控件的父对象都是窗口类，本类的是基于QObject类的
        return;
    }
}

//获取22s芯片温度
QString Pa22scontrol::gettemprature()
{
    //发送JSON指令，将JSON指令写成代码形式
    QJsonObject jsonObject;
    jsonObject["board_temp"] = QJsonValue::Null;
    // 将 JSON 对象转换为 JSON 文档
    QJsonDocument jsonDocument(jsonObject);
    // 将 JSON 文档转换为 QByteArray形式
    QByteArray jsonData = jsonDocument.toJson(QJsonDocument::Compact);

    //每个指令要以 RS(0x1E) 字符结束，在这个转换成QByteArray后，加上字符
    jsonData.append(char(0x1E));

    // 发送数据到服务器
    order_tcp->write(jsonData);
    //等待服务器完全接收数据
    order_tcp->waitForBytesWritten();
    //等待服务器发送的数据
    order_tcp->waitForReadyRead();

    //读取数据
     //接收数据，是以QByteArray类型进行传递
    QByteArray receivedData = order_tcp->readAll();

    QString str = QString::fromUtf8(receivedData);
    if (str.contains("error"))
    {
        QMessageBox::warning(nullptr, "警告", "数据设置错误");
        return "";
    }
    else
    {
        // 定义正则表达式模式，用于匹配 "result" 后面的数字部分
        QRegularExpression result("\"result\":([0-9]+\\.?[0-9]*)");

        // 在字符串中搜索匹配的内容
        QRegularExpressionMatch match = result.match(str);

        // 如果找到匹配项，则提取数值部分
        if (match.hasMatch()) {
            QString resultString = match.captured(1); // 获取第一个捕获组中的内容            
            return resultString;
        }
        else {
            return "";
        }
    }
}

//设置数字增益
void Pa22scontrol::setdgain(double dgain)
{
    QJsonObject jsonObject;
    //数字增益
    jsonObject["dgain"] = dgain;
    transferinstruction(jsonObject);
}

//设置发射电压
void Pa22scontrol::setvolt(int volt)
{
    QJsonObject jsonObject;
    jsonObject["volt"] = volt;
    transferinstruction(jsonObject);
}

//设置脉冲宽度
void Pa22scontrol::setpulsewidth(int pulse_width)
{
    QJsonObject jsonObject;
    jsonObject["pulse_width"] = pulse_width;
    transferinstruction(jsonObject);
}

//设置增益1,2
void Pa22scontrol::setvga(int vga1, int vga2)
{
    QJsonObject jsonObject;
    jsonObject["gain"] = QJsonObject({
       {"vga1", vga1},
        {"vga2", vga2}
        });
    transferinstruction(jsonObject);
}

//设置重复频率
void Pa22scontrol::setprf(int prf)
{
    QJsonObject jsonObject;
    jsonObject["prf"] = prf;
    transferinstruction(jsonObject);
}

//设置压缩比
void Pa22scontrol::setrangeratio(double range_ratio)
{
    QJsonObject jsonObject;
    jsonObject["range_ratio"] = range_ratio;
    transferinstruction(jsonObject);
}

//设置数字滤波
void Pa22scontrol::setdfilter(int dfilter)
{
    QJsonObject jsonObject;
    jsonObject["dfilter"] = dfilter;
    transferinstruction(jsonObject);
}

//设置视频检波
void Pa22scontrol::setvideodetect(int video_detect)
{
    QJsonObject jsonObject;
    jsonObject["video_detect"] = 2;
    transferinstruction(jsonObject);
}

//设置线扫法则  默认无斜楔
void Pa22scontrol::setpalscan(Line_rule line_rule)
{
    myline_rule = line_rule;
    qDebug() << "Pa22scontrol all : " << myline_rule.elm_num;

    QJsonObject jsonObject;
    jsonObject["pa_lscan"] = QJsonObject{
        {"linear_pa_probe", QJsonObject{
            {"elm_num", myline_rule.elm_num},//阵元数量
            {"pitch", myline_rule.pitch}
        }},
        /*{"slope_wedge", QJsonObject{
            {"angle", 37.0},
            {"height", 20.0},
            {"sound_velocity", 2337.0}
        }},*/
        {"aperture", QJsonObject{
            {"start_elm", myline_rule.start_elm - 1},//第一个阵元 索引为0
            {"stop_elm", myline_rule.end_elm - 1},
            {"size", myline_rule.aperture_size}//13个孔径
        }},
        {"focus", QJsonObject{
            {"angle", myline_rule.focus_angle},
            {"distance", myline_rule.focus_distance}
        }},
        {"workpiece", QJsonObject{
            {"sound_velocity", myline_rule.sound_velocity}
        }}
    };
    transferinstruction(jsonObject);
    //清理之前留存的数据
    //receivedData.clear();
}

//编码器清零
void Pa22scontrol::resetencoder()
{
    QJsonObject jsonObject;
    jsonObject["encoder_reset"] = QJsonObject{
        {"idx", 0}
    };
    transferinstruction(jsonObject);
}

//接收数据
void Pa22scontrol::receive_data()
{
    int signal_num = myline_rule.elm_num - myline_rule.aperture_size + 1;
    //qDebug() << "22scontrol signal_num is " << signal_num;

    receivedData.append(data_tcp->readAll());
    if (!receivedData.isEmpty())
    {
        while (receivedData.size() >= (signal_num * 512 + 16))//加上等号，将不会出现"n <= d.size - pos"的报错
        {
            Data = receivedData.mid(16, signal_num * 512);//此处，已经删除了16个字节的识别符
            emit sendData(Data); //通过信号和槽将数据传递出去 
            receivedData.remove(0, (signal_num * 512 + 16));
        }
    }
    //qDebug() << "receive data";
}

Pa22scontrol::~Pa22scontrol()
{}
