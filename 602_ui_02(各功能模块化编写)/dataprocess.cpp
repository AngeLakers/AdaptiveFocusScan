#include "dataprocess.h"

DataProcess::DataProcess(QObject* parent)
    : QObject(parent)
{
    //初始化线扫法则
    myline_rule.elm_num = 64;
    myline_rule.pitch = 0.6;
    myline_rule.aperture_size = 29;
    myline_rule.focus_angle = 0;
    myline_rule.focus_distance = 0;
    myline_rule.sound_velocity = 5900;
    myline_rule.start_elm = 1;
    myline_rule.end_elm = 64;
    //初始化信号数量
    signal_num = myline_rule.end_elm - myline_rule.start_elm + 1 - myline_rule.aperture_size + 1;
    //初始化C扫图像相关索引
    ys_door_changed(0, 200, 400, 0, 800);//参数值根据parameter_ui中door参数的初始化值
}

//从主线程获取数据函数
void DataProcess::getdata(QByteArray data)
{
    Data = data;
    if (EncoderCscanstatus)//开始编码器扫查再开始读取编码器位置
    {
    }
        getencoderpos();
}

//从parameter_ui接收线扫法则数据
void DataProcess::getLine_rule(Line_rule line_rule)
{
    myline_rule = line_rule;
    signal_num = myline_rule.end_elm - myline_rule.start_elm + 1 - myline_rule.aperture_size + 1;
}


//生成B扫数据，并将A扫数据发送给
void DataProcess::showBscan()
{
    //判定数据量是否为空或者不等于应等数据量大小,主线程获取数据时，就已经把前面16个字节的识别符删除了
    if ((Data.isEmpty()) || (Data.size() != (signal_num * 512)))
        return;

    QVector<int> data;

    for (int i = 0; i < Data.size(); i++)
    {
        int dex = i / 512;
        if (((0 + 512 * dex) <= i) && (i <= (399 + 512 * dex)))
        {
            data.append((int)(unsigned char)Data[i]);//只存幅值数据
        }
    }
    if (data.size() == signal_num * 400)//防止QVector报错index out of range
    {
        //将数据发送给主线程，用于显示A扫数据
        emit sendAscanData(data);

        QImage Bimage((signal_num + (signal_num - 1) * 9), 200, QImage::Format_RGB32);
        for (int x = 0; x < signal_num; x++)
        {
            for (int y = 0; y < 200; y++)
            {
                int value = data[y + x * 400 + ys];
                Bimage.setPixelColor(x * 10, y, valuetocolor(value));
            }
        }
        //添加插值点
        for (int i = 0; i < signal_num - 1; i++)
        {
            for (int x = 1; x < 10; x++)
            {
                for (int y = 0; y < 200; y++)
                {
                    int value = data[y + 400 * i + ys] * (10 - x) / 10 + data[y + 400 * i + 400 + ys] * x / 10;//int型能够四舍五入
                    Bimage.setPixelColor(x + i * 10, y, valuetocolor(value));
                }
            }
        }
        //图像缩放
        BscaledImage = Bimage.copy(0, 0, Bimage.width(), Bimage.height());
        sendBscanImage(BscaledImage);
    }
}

//C扫
void DataProcess::showCscan()
{
    //判定数据量是否为空或者不等于应等数据量大小
    if ((Data.isEmpty()) || (Data.size() != (signal_num * 512)))
        return;

    QVector<int> data;
    for (int i = 0; i < Data.size(); i++)
    {
        int dex = i / 512;
        if ((0 + 512 * dex <= i) && (i <= 399 + 512 * dex))
        {
            data.append((int)(unsigned char)Data[i]);//只存幅值数据
        }
    }
    emit sendDataToSave(data, Cscandex, signal_num);
    //C扫
    for (int y = 0; y < CImageHeight; y++)
    {
        //易报错
        QVector<int>temp = data.mid(startdex + (y + CscanStartsignaldex) * 400, signallength);
        int  value = *std::max_element(temp.constBegin(), temp.constEnd());

        CImage.setPixelColor(Cscandex, y, valuetocolor(value));
    }

    if (Cscandex >= C_Scan_label_width)//当显示的图片长度大于label时，图片将滚动显示
    {
        //用QImage中的copy，而不是直接 =
        CscaledImage = CImage.copy(Cscandex - C_Scan_label_width + 1, 0, C_Scan_label_width, CImage.height());
    }
    else
    {
        CscaledImage = CImage.copy(0, 0, Cscandex + 1, CImage.height());
    }
    emit sendCscanImage(CscaledImage);
}

//获取编码器位置函数
void DataProcess::getencoderpos()
{
    //判定数据量是否为空或者不等于应等数据量大小
    if ((Data.isEmpty()) || (Data.size() != (signal_num * 512)))
        return;
    //编码器位置 412-415中的值为encoderposA;416-419中的值为encoderposB;
    quint32 encoderposA = 0;
    quint32 encoderposB = 0;
    // 从 Data 中读取小端存储的无符号整数数据
    QDataStream streamA(Data.mid(412, sizeof(quint32)));
    streamA.setByteOrder(QDataStream::LittleEndian); // 设置为小端存储
    streamA >> encoderposA;
    QDataStream streamB(Data.mid(416, sizeof(quint32)));
    streamB.setByteOrder(QDataStream::LittleEndian); // 设置为小端存储
    streamB >> encoderposB;

    if (encoder_first_scanstatus)
    {
        emit sendEncoderpos(0);//将位置0发送出去，防止编码器位置乱码
        encoder_first_scanstatus = false;
        return;
    }


    if (EncoderCscandex < 100)
    {
        //判断A,B相的绝对值差值是否过大，过大的话将直接return
        int abs_AB = encoderposA - encoderposB;
        if (abs(abs_AB) > 10000)
        {
            //qDebug() << "abs_AB=" << abs(abs_AB);
            return;
        }
        EncoderCscandex++;
    }

    int currentpos;
    if (encoderdex == 0)//正向
    {
        currentpos = encoderposA - encoderposB;
        emit sendEncoderpos(currentpos);//发送位置给主线程用于显示
    }
    else
    {
        currentpos = encoderposB - encoderposA;
        emit sendEncoderpos(currentpos);
    }

    //编码器C扫信号触发
    if (currentpos >= encoderlastpos + encoder_accuracy)
    {
        //保证位置每增加encoder_accuracy就能触发一次C扫信号，encoder_accuracy可以替换成其他参数
        int value = (currentpos - encoderlastpos) / encoder_accuracy;
        for (int i = 0; i < value; i++)
        {
            Cscandex++;
            emit startEncoderCscan();
        }
        //比上次添加C扫数据的点再大10才能添加下一点，加上余数，可以保证每10个编码器量就添加一次C扫数据
        encoderlastpos = currentpos - (currentpos - encoderlastpos) % encoder_accuracy;
    }
    else if ((encoderlastpos - currentpos >= encoder_accuracy) && (Cscandex > 0))
    {

        int value = (encoderlastpos - currentpos) / encoder_accuracy;
        for (int i = 0; (i < value) && (Cscandex > 0); i++)
        {
            Cscandex--;
            emit startEncoderCscan();//这行代码能够让图像实现滚动效果
        }
        encoderlastpos = currentpos + (encoderlastpos - currentpos) % encoder_accuracy;
    }

}

//延时，闸门改变时，相应的操作
void DataProcess::ys_door_changed(int value, int top_door, int bot_door, int left_door, int right_door)
{
    ys = value;
    startdex = top_door / (B_Scan_label_height / 200) + ys;
    qDebug() << "startdex= " << startdex;
    signallength = bot_door / (B_Scan_label_height / 200) - startdex;//长度
    CscanStartsignaldex = (left_door / (B_Scan_label_width / signal_num));
    CscanStopsignaldex = (right_door / (B_Scan_label_width / signal_num));
    CImageHeight = CscanStopsignaldex - CscanStartsignaldex;
}

// 颜色过渡函数
QColor DataProcess::interpolateColors(const QColor& startColor, const QColor& endColor, qreal ratio)
{
    // ratio在0到1之间，表示从起始颜色过渡到结束颜色的程度
    int r = startColor.red() + ratio * (endColor.red() - startColor.red());
    int g = startColor.green() + ratio * (endColor.green() - startColor.green());
    int b = startColor.blue() + ratio * (endColor.blue() - startColor.blue());
    return QColor(r, g, b);
}

//幅值转对应颜色
QColor DataProcess::valuetocolor(int value)
{
    QColor color;
    if (value <= 63)
    {
        // 白色到蓝色过渡
        color = interpolateColors(Qt::white, customBlue, value / 63.0);
    }
    else if (value <= 127)
    {
        // 蓝色到绿色过渡
        color = interpolateColors(customBlue, Qt::green, (value - 63) / 64.0);
    }
    else if (value <= 191)
    {
        // 绿色到黄色过渡
        color = interpolateColors(Qt::green, Qt::yellow, (value - 127) / 64.0);
    }
    else
    {
        // 黄色到红色过渡
        color = interpolateColors(Qt::yellow, Qt::red, (value - 191) / 64.0);
    }
    return color;
}

DataProcess::~DataProcess()
{}

