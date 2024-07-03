#include "servocontrol.h"

Servocontrol::Servocontrol(QObject *parent)
	: QObject(parent)
{
    // 建立串口连接
    //modbustoolDevice = new QModbusRtuSerialMaster(this);
    modbustoolDevice = new QModbusRtuSerialClient(this);
    //串口连接相关参数设置
    modbustoolDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
        "COM11");//串口名称 
    modbustoolDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
        QSerialPort::NoParity);//串口奇偶校验位
    modbustoolDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
        115200);//串口波特率
    modbustoolDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
        QSerialPort::Data8);//串口数据位
    modbustoolDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
        QSerialPort::OneStop);//串口停止位 

    //再设置从机无响应时的动作
    modbustoolDevice->setTimeout(1000);//从设备回复信息的超时时间
    modbustoolDevice->setNumberOfRetries(2);//重复发送次数
    
    //定义Modbus基本数据单元，读取数据，模式0x03读保持寄存器，寄存器地址257，所读取数据长度1
    readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 257, 1);
}

//连接舵机
void Servocontrol::connectToservo()
{
    //开始连接串口，并判断连接状态
    if (modbustoolDevice->connectDevice()) {
        //无提示
    }
    else {
        QMessageBox* cpt = new QMessageBox;
        cpt->setWindowTitle(QString::fromLocal8Bit("舵机连接失败 "));//加空格就不报错
        cpt->setText(QString::fromLocal8Bit("modbustoolDevice->errorString() "));
        cpt->setIcon(QMessageBox::Information);
        cpt->addButton(QMessageBox::Ok);
        // 设置提示窗口总在顶层显示
        cpt->setWindowFlags(cpt->windowFlags() | Qt::WindowStaysOnTopHint);
        cpt->exec();
        return;
    }
}

//断开舵机连接
void Servocontrol::disconnectToservo()
{
    modbustoolDevice->disconnectDevice();
}

//上位机向下位机的某个寄存器写入数据 参数为寄存器地址
//写入数据(regStartAddr为调整角度寄存器，地址为128，number为写入数据为转动目标位置0~4095，serverAddress为从机地址，01，02，03)
void Servocontrol::writeToclient(unsigned int regStartAddr, unsigned int number,unsigned int serverAddress)
{
    //如果没有定义modbus串口，直接返回
    if (!modbustoolDevice) {
        //statusBar()->showMessage(tr("Modbus通讯失败，请检查串口连接！"), 5000);
        return;
    }
    //清除状态栏显示
    //statusBar()->clearMessage();

    //定义Modbus的基本数据单元，这里设置了modbus为0x06写模式，寄存器地址，以及传输数据的大小
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, regStartAddr, 1);//06
    //要写入的数据
    writeUnit.setValue(0, number);

    //发送Modbus格式的数据到从机地址为serverAddress的从机，serverAddress表示第几个从机，01，02，03
    if (auto* reply = modbustoolDevice->sendWriteRequest(writeUnit, serverAddress)) {
        if (!reply->isFinished()) {
            //如果接收到响应信息
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    //接收到的响应信息有协议错误，则更新状态栏，错误码
                    //statusBar()->showMessage(tr("写入数据错误: %1 (Mobus exception: 0x%2)")
                    //    .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                    //    5000);
                }
                else if (reply->error() != QModbusDevice::NoError) {
                    //接收到的响应信息有其他错误，则更新状态栏，错误码
                    //statusBar()->showMessage(tr("写入数据错误: %1 (code: 0x%2)").
                    //    arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                }
                else {
                    //获取响应信息的数据到modbus基本数据QModbusDataUnit
                    const QModbusDataUnit unit = reply->result();

                    //统计响应信息中数据的个数
                    QString showdata = tr("总数：%1 值为：").arg(unit.valueCount());

                    //依次展示响应信息中的数据
                    for (uint i = 0; i < unit.valueCount(); i++) {
                        const QString entry = tr("%1 ").arg(unit.value(i));
                        showdata = showdata + entry;
                    }
                    //将信息输出到控制台
                    qDebug() << showdata;
                }
                reply->deleteLater();
                });
        }
        else {
            reply->deleteLater();
        }
    }
    else {
        //发送没有成功，更新状态栏信息
        //statusBar()->showMessage(tr("写入数据错误: ") + modbustoolDevice->errorString(), 5000);
    }
}

//读取位置信息数据
double Servocontrol::readToclient(int scanmodel_SLR)
{
    //int live_angle_value;//接收舵机实际位置（0~4095）
    if (auto* reply = modbustoolDevice->sendReadRequest(readUnit, scanmodel_SLR))//sendReadRequest不是sendWirteRequest
    {
        if (!reply->isFinished()) {
            //如果接收到响应信息
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError)
                {
                    //接收到的响应信息有协议错误，则更新状态栏，错误码
                    //statusBar()->showMessage(tr("写入数据错误: %1 (Mobus exception: 0x%2)")
                    //    .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
                }
                else if (reply->error() != QModbusDevice::NoError) {
                    //接收到的响应信息有其他错误，则更新状态栏，错误码
                    //statusBar()->showMessage(tr("写入数据错误: %1 (code: 0x%2)").
                    //    arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                }
                else {
                    const QModbusDataUnit unit = reply->result();
                    //统计响应信息中数据的个数
                    QString showdata = tr("总数：%1 值为：").arg(unit.valueCount());
                    //依次展示响应信息中的数据
                    for (uint i = 0; i < unit.valueCount(); i++) {
                        const QString entry = tr("%1 ").arg(unit.value(i));
                        showdata = showdata + entry;
                    }
                    //显示在界面中
                    //qDebug() << "output" << showdata;
                    live_angle_value = unit.value(0);
                    double num = (360.0 / 4095.0);
                    live_angle = num * unit.value(0);
                }
                reply->deleteLater();
                });
        }
        else {
            reply->deleteLater();
        }
    }
    else {
        //statusBar()->showMessage(tr("写入数据错误: ") + modbustoolDevice->errorString(), 5000);
    }
    return live_angle;
}

Servocontrol::~Servocontrol()
{}
