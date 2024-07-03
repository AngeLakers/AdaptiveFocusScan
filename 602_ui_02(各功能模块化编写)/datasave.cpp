#include "datasave.h"

DataSave::DataSave(QObject* parent)
    : QObject(parent)
{
    //��ʱ�ļ�λ��
    tempfilePath = QDir::currentPath() + "/tempdatafile/temp.bin";
}

//������  //ʹ�ó������üȱ����˶Բ����ĸ���Ӱ�����ܣ��ֱ��������ù����в�����������޸�,�����ܱ������߳�ͬʱ����ͬһ������
void DataSave::savetempdata(QVector<int> data, int TimeCscandex, int signal_num)
{
    QFile file(tempfilePath);
    if (file.open(QIODevice::Append))//ʹ��QIODevice::Append������׷���������ļ��У������Ǹ�д
    {
        // ��λ��Ҫ��д����ʼλ��
        qint64 startPos = ((TimeCscandex)*signal_num * 400 + 7) * sizeof(int);//7Ϊ�ڴ�����֮ǰ������7��int ������

        if (!file.seek(startPos))
        {
            qDebug() << "Failed to seek to position:" << startPos;
            file.close();
            return;
        }

        QDataStream out(&file);
        // д�����ݵ��������ļ�
        for (int value : data)
        {
            out << value;
        }
        file.close();
    }
    //qDebug() << "sucess ";//ֻҪ�����˾ͻ����
}

//����ʱ����ת��Ϊ��ʽ����
void DataSave::savedata(QString filePath)
{
    /*D:\\MyCode\\hjqtest��ʵս���룩\\�����Զ����豸\\602_ui_design_02����������ֱ�ӴӶ������ļ��ж�ȡ��*/
    //QString filePath = QFileDialog::getSaveFileName(nullptr, "Save Binary File", QDir::currentPath()+"/Cɨ����", "Binary Files (*.bin)");
    if (filePath.isEmpty())
    {
        qDebug() << "User canceled the operation.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to open file for writing:" << file.errorString();
        return;
    }

    QFile tempFile(tempfilePath); // ��ʱ�������ļ�
    if (!tempFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open temp file for reading:" << tempFile.errorString();
        file.close();
        return;
    }

    QDataStream out(&file);
    QDataStream in(&tempFile);

    qint64 totalBytes = tempFile.size();
    qint64 bytesWritten = 0;
    int dex = 1;

    while (!in.atEnd())
    {
        int value;
        in >> value; // ��ȡһ������
        out << value; // д�뵽��ʱ�ļ���
        bytesWritten += sizeof(int);
        int percentage = (int)((bytesWritten * 100) / totalBytes);
        if (percentage == dex)
        {
            emit progressUpdated(percentage); // ���ͱ�������ź�
            dex++;
        }

    }

    tempFile.close();
    file.close();

    // ɾ��ԭ�ļ�
    tempFile.remove();
    //qDebug() << "Saving progress: already" ;

}


//�����ź�������aɨ�ź���������������ʱ�ļ�
void DataSave::startsave(int signal_num, int top, int bot, int left, int right, int ys)
{
    qDebug() << "lefft ;" << left << "right :" << right;
    QFile file(tempfilePath);
    if (file.open(QIODevice::Append))
    {
        int Clength = 0;//Cɨ���ݳ��ȣ���ռλ
        QDataStream out(&file);
        // д�����ݵ��������ļ�
        out << Clength;
        out << signal_num; //��������int������
        out << top;
        out << bot;
        out << left;
        out << right;
        out << ys;
        file.close();
    }
}
//��ֹͣɨ��󣬴���Cɨ���ݳ���
void DataSave::stopsave(int Cscandex, int signal_num)
{
    //qDebug() << "TimeCscandex:" << TimeCscandex; 
    int Clength = Cscandex + 1;
    QFile file(tempfilePath);
    //�Ƚ����ݳ��Ƚ���д��
    if (file.open(QIODevice::Append))
    {
        // ��λ��Ҫ��д����ʼλ��
        if (!file.seek(0))
        {
            qDebug() << "Failed to seek to position:" << 0;
            file.close();
            return;
        }
        QDataStream out(&file);
        out << Clength;//д��Cɨ���ݳ���
        file.close();
    }
    //ȷ����ʱ�ļ����ճ��ȣ����������ʱ�ļ�
    if (file.open(QIODevice::ReadOnly))
    {
        // ��ȡ�ļ��ܵ�������
        qint64 fileSize = file.size();
        qint64 bytesToKeep = (Clength * signal_num * 400 + 7) * sizeof(int); // ����Ҫ�������ֽ���//�˴����� TimeCscandex *signal_num*400+2)
        if (bytesToKeep >= 0) // ȷ��Ҫɾ�������ݷ�Χ�Ƿ�Ϸ�
        {
            QFile tempFile("tempfile.dat"); // ����һ����ʱ�ļ����ڴ洢����������
            if (tempFile.open(QIODevice::WriteOnly))
            {
                QDataStream in(&file);
                QDataStream out(&tempFile);
                // ��ȡ������Ҫ����������
                while (!in.atEnd() && file.pos() < bytesToKeep)
                {
                    int value;
                    in >> value; // ��ȡһ������
                    out << value; // д�뵽��ʱ�ļ���
                }
                tempFile.close();
                file.close();
                // ɾ��ԭ�ļ�
                file.remove();
                // ��������ʱ�ļ�ΪĿ���ļ�����
                tempFile.rename(tempfilePath);
            }
        }
    }
}

DataSave::~DataSave()
{}
