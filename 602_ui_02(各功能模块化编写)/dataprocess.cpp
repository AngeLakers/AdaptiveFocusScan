#include "dataprocess.h"

DataProcess::DataProcess(QObject* parent)
    : QObject(parent)
{
    //��ʼ����ɨ����
    myline_rule.elm_num = 64;
    myline_rule.pitch = 0.6;
    myline_rule.aperture_size = 29;
    myline_rule.focus_angle = 0;
    myline_rule.focus_distance = 0;
    myline_rule.sound_velocity = 5900;
    myline_rule.start_elm = 1;
    myline_rule.end_elm = 64;
    //��ʼ���ź�����
    signal_num = myline_rule.end_elm - myline_rule.start_elm + 1 - myline_rule.aperture_size + 1;
    //��ʼ��Cɨͼ���������
    ys_door_changed(0, 200, 400, 0, 800);//����ֵ����parameter_ui��door�����ĳ�ʼ��ֵ
}

//�����̻߳�ȡ���ݺ���
void DataProcess::getdata(QByteArray data)
{
    Data = data;
    if (EncoderCscanstatus)//��ʼ������ɨ���ٿ�ʼ��ȡ������λ��
    {
    }
        getencoderpos();
}

//��parameter_ui������ɨ��������
void DataProcess::getLine_rule(Line_rule line_rule)
{
    myline_rule = line_rule;
    signal_num = myline_rule.end_elm - myline_rule.start_elm + 1 - myline_rule.aperture_size + 1;
}


//����Bɨ���ݣ�����Aɨ���ݷ��͸�
void DataProcess::showBscan()
{
    //�ж��������Ƿ�Ϊ�ջ��߲�����Ӧ����������С,���̻߳�ȡ����ʱ�����Ѿ���ǰ��16���ֽڵ�ʶ���ɾ����
    if ((Data.isEmpty()) || (Data.size() != (signal_num * 512)))
        return;

    QVector<int> data;

    for (int i = 0; i < Data.size(); i++)
    {
        int dex = i / 512;
        if (((0 + 512 * dex) <= i) && (i <= (399 + 512 * dex)))
        {
            data.append((int)(unsigned char)Data[i]);//ֻ���ֵ����
        }
    }
    if (data.size() == signal_num * 400)//��ֹQVector����index out of range
    {
        //�����ݷ��͸����̣߳�������ʾAɨ����
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
        //��Ӳ�ֵ��
        for (int i = 0; i < signal_num - 1; i++)
        {
            for (int x = 1; x < 10; x++)
            {
                for (int y = 0; y < 200; y++)
                {
                    int value = data[y + 400 * i + ys] * (10 - x) / 10 + data[y + 400 * i + 400 + ys] * x / 10;//int���ܹ���������
                    Bimage.setPixelColor(x + i * 10, y, valuetocolor(value));
                }
            }
        }
        //ͼ������
        BscaledImage = Bimage.copy(0, 0, Bimage.width(), Bimage.height());
        sendBscanImage(BscaledImage);
    }
}

//Cɨ
void DataProcess::showCscan()
{
    //�ж��������Ƿ�Ϊ�ջ��߲�����Ӧ����������С
    if ((Data.isEmpty()) || (Data.size() != (signal_num * 512)))
        return;

    QVector<int> data;
    for (int i = 0; i < Data.size(); i++)
    {
        int dex = i / 512;
        if ((0 + 512 * dex <= i) && (i <= 399 + 512 * dex))
        {
            data.append((int)(unsigned char)Data[i]);//ֻ���ֵ����
        }
    }
    emit sendDataToSave(data, Cscandex, signal_num);
    //Cɨ
    for (int y = 0; y < CImageHeight; y++)
    {
        //�ױ���
        QVector<int>temp = data.mid(startdex + (y + CscanStartsignaldex) * 400, signallength);
        int  value = *std::max_element(temp.constBegin(), temp.constEnd());

        CImage.setPixelColor(Cscandex, y, valuetocolor(value));
    }

    if (Cscandex >= C_Scan_label_width)//����ʾ��ͼƬ���ȴ���labelʱ��ͼƬ��������ʾ
    {
        //��QImage�е�copy��������ֱ�� =
        CscaledImage = CImage.copy(Cscandex - C_Scan_label_width + 1, 0, C_Scan_label_width, CImage.height());
    }
    else
    {
        CscaledImage = CImage.copy(0, 0, Cscandex + 1, CImage.height());
    }
    emit sendCscanImage(CscaledImage);
}

//��ȡ������λ�ú���
void DataProcess::getencoderpos()
{
    //�ж��������Ƿ�Ϊ�ջ��߲�����Ӧ����������С
    if ((Data.isEmpty()) || (Data.size() != (signal_num * 512)))
        return;
    //������λ�� 412-415�е�ֵΪencoderposA;416-419�е�ֵΪencoderposB;
    quint32 encoderposA = 0;
    quint32 encoderposB = 0;
    // �� Data �ж�ȡС�˴洢���޷�����������
    QDataStream streamA(Data.mid(412, sizeof(quint32)));
    streamA.setByteOrder(QDataStream::LittleEndian); // ����ΪС�˴洢
    streamA >> encoderposA;
    QDataStream streamB(Data.mid(416, sizeof(quint32)));
    streamB.setByteOrder(QDataStream::LittleEndian); // ����ΪС�˴洢
    streamB >> encoderposB;

    if (encoder_first_scanstatus)
    {
        emit sendEncoderpos(0);//��λ��0���ͳ�ȥ����ֹ������λ������
        encoder_first_scanstatus = false;
        return;
    }


    if (EncoderCscandex < 100)
    {
        //�ж�A,B��ľ���ֵ��ֵ�Ƿ���󣬹���Ļ���ֱ��return
        int abs_AB = encoderposA - encoderposB;
        if (abs(abs_AB) > 10000)
        {
            //qDebug() << "abs_AB=" << abs(abs_AB);
            return;
        }
        EncoderCscandex++;
    }

    int currentpos;
    if (encoderdex == 0)//����
    {
        currentpos = encoderposA - encoderposB;
        emit sendEncoderpos(currentpos);//����λ�ø����߳�������ʾ
    }
    else
    {
        currentpos = encoderposB - encoderposA;
        emit sendEncoderpos(currentpos);
    }

    //������Cɨ�źŴ���
    if (currentpos >= encoderlastpos + encoder_accuracy)
    {
        //��֤λ��ÿ����encoder_accuracy���ܴ���һ��Cɨ�źţ�encoder_accuracy�����滻����������
        int value = (currentpos - encoderlastpos) / encoder_accuracy;
        for (int i = 0; i < value; i++)
        {
            Cscandex++;
            emit startEncoderCscan();
        }
        //���ϴ����Cɨ���ݵĵ��ٴ�10���������һ�㣬�������������Ա�֤ÿ10���������������һ��Cɨ����
        encoderlastpos = currentpos - (currentpos - encoderlastpos) % encoder_accuracy;
    }
    else if ((encoderlastpos - currentpos >= encoder_accuracy) && (Cscandex > 0))
    {

        int value = (encoderlastpos - currentpos) / encoder_accuracy;
        for (int i = 0; (i < value) && (Cscandex > 0); i++)
        {
            Cscandex--;
            emit startEncoderCscan();//���д����ܹ���ͼ��ʵ�ֹ���Ч��
        }
        encoderlastpos = currentpos + (encoderlastpos - currentpos) % encoder_accuracy;
    }

}

//��ʱ��բ�Ÿı�ʱ����Ӧ�Ĳ���
void DataProcess::ys_door_changed(int value, int top_door, int bot_door, int left_door, int right_door)
{
    ys = value;
    startdex = top_door / (B_Scan_label_height / 200) + ys;
    qDebug() << "startdex= " << startdex;
    signallength = bot_door / (B_Scan_label_height / 200) - startdex;//����
    CscanStartsignaldex = (left_door / (B_Scan_label_width / signal_num));
    CscanStopsignaldex = (right_door / (B_Scan_label_width / signal_num));
    CImageHeight = CscanStopsignaldex - CscanStartsignaldex;
}

// ��ɫ���ɺ���
QColor DataProcess::interpolateColors(const QColor& startColor, const QColor& endColor, qreal ratio)
{
    // ratio��0��1֮�䣬��ʾ����ʼ��ɫ���ɵ�������ɫ�ĳ̶�
    int r = startColor.red() + ratio * (endColor.red() - startColor.red());
    int g = startColor.green() + ratio * (endColor.green() - startColor.green());
    int b = startColor.blue() + ratio * (endColor.blue() - startColor.blue());
    return QColor(r, g, b);
}

//��ֵת��Ӧ��ɫ
QColor DataProcess::valuetocolor(int value)
{
    QColor color;
    if (value <= 63)
    {
        // ��ɫ����ɫ����
        color = interpolateColors(Qt::white, customBlue, value / 63.0);
    }
    else if (value <= 127)
    {
        // ��ɫ����ɫ����
        color = interpolateColors(customBlue, Qt::green, (value - 63) / 64.0);
    }
    else if (value <= 191)
    {
        // ��ɫ����ɫ����
        color = interpolateColors(Qt::green, Qt::yellow, (value - 127) / 64.0);
    }
    else
    {
        // ��ɫ����ɫ����
        color = interpolateColors(Qt::yellow, Qt::red, (value - 191) / 64.0);
    }
    return color;
}

DataProcess::~DataProcess()
{}

