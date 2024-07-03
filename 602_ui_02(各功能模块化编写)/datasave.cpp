#include "datasave.h"

DataSave::DataSave(QObject* parent)
    : QObject(parent)
{
    //临时文件位置
    tempfilePath = QDir::currentPath() + "/tempdatafile/temp.bin";
}

//存数据  //使用常量引用既避免了对参数的复制影响性能，又避免了引用过程中不会参数产生修改,但不能避免多个线程同时访问同一个数据
void DataSave::savetempdata(QVector<int> data, int TimeCscandex, int signal_num)
{
    QFile file(tempfilePath);
    if (file.open(QIODevice::Append))//使用QIODevice::Append，将会追加数据至文件中，而不是覆写
    {
        // 定位到要覆写的起始位置
        qint64 startPos = ((TimeCscandex)*signal_num * 400 + 7) * sizeof(int);//7为在存数据之前，存了7的int 型数据

        if (!file.seek(startPos))
        {
            qDebug() << "Failed to seek to position:" << startPos;
            file.close();
            return;
        }

        QDataStream out(&file);
        // 写入数据到二进制文件
        for (int value : data)
        {
            out << value;
        }
        file.close();
    }
    //qDebug() << "sucess ";//只要调用了就会输出
}

//将临时数据转存为正式数据
void DataSave::savedata(QString filePath)
{
    /*D:\\MyCode\\hjqtest（实战代码）\\包铁自动化设备\\602_ui_design_02（所有数据直接从二进制文件中读取）*/
    //QString filePath = QFileDialog::getSaveFileName(nullptr, "Save Binary File", QDir::currentPath()+"/C扫数据", "Binary Files (*.bin)");
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

    QFile tempFile(tempfilePath); // 临时二进制文件
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
        in >> value; // 读取一个整数
        out << value; // 写入到临时文件中
        bytesWritten += sizeof(int);
        int percentage = (int)((bytesWritten * 100) / totalBytes);
        if (percentage == dex)
        {
            emit progressUpdated(percentage); // 发送保存进度信号
            dex++;
        }

    }

    tempFile.close();
    file.close();

    // 删除原文件
    tempFile.remove();
    //qDebug() << "Saving progress: already" ;

}


//存入信号数量（a扫信号条数）及创建临时文件
void DataSave::startsave(int signal_num, int top, int bot, int left, int right, int ys)
{
    qDebug() << "lefft ;" << left << "right :" << right;
    QFile file(tempfilePath);
    if (file.open(QIODevice::Append))
    {
        int Clength = 0;//C扫数据长度，先占位
        QDataStream out(&file);
        // 写入数据到二进制文件
        out << Clength;
        out << signal_num; //存了两个int型数据
        out << top;
        out << bot;
        out << left;
        out << right;
        out << ys;
        file.close();
    }
}
//按停止扫查后，存入C扫数据长度
void DataSave::stopsave(int Cscandex, int signal_num)
{
    //qDebug() << "TimeCscandex:" << TimeCscandex; 
    int Clength = Cscandex + 1;
    QFile file(tempfilePath);
    //先将数据长度进行写入
    if (file.open(QIODevice::Append))
    {
        // 定位到要覆写的起始位置
        if (!file.seek(0))
        {
            qDebug() << "Failed to seek to position:" << 0;
            file.close();
            return;
        }
        QDataStream out(&file);
        out << Clength;//写入C扫数据长度
        file.close();
    }
    //确定临时文件最终长度，获得最终临时文件
    if (file.open(QIODevice::ReadOnly))
    {
        // 获取文件总的数据量
        qint64 fileSize = file.size();
        qint64 bytesToKeep = (Clength * signal_num * 400 + 7) * sizeof(int); // 计算要保留的字节数//此处存疑 TimeCscandex *signal_num*400+2)
        if (bytesToKeep >= 0) // 确定要删除的数据范围是否合法
        {
            QFile tempFile("tempfile.dat"); // 创建一个临时文件用于存储保留的数据
            if (tempFile.open(QIODevice::WriteOnly))
            {
                QDataStream in(&file);
                QDataStream out(&tempFile);
                // 读取并保留要保留的数据
                while (!in.atEnd() && file.pos() < bytesToKeep)
                {
                    int value;
                    in >> value; // 读取一个整数
                    out << value; // 写入到临时文件中
                }
                tempFile.close();
                file.close();
                // 删除原文件
                file.remove();
                // 重命名临时文件为目标文件名称
                tempFile.rename(tempfilePath);
            }
        }
    }
}

DataSave::~DataSave()
{}
