#include "parameter_ui.h"

Parameter_ui::Parameter_ui(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    //初始化各控件
    set_ui_position();
}

//初始化各控件位置
void Parameter_ui::set_ui_position()
{
    setWindowTitle("Scan set");
    setFixedSize(600, 350);
    ui.tabWidget->setGeometry(0, 0, 600, 350);

    //发射接收仪页
    ui.Pa_22s_groupBox->setGeometry(10, 10, 280, 190);
    ui.linescan_rule_groupBox->setGeometry(300, 10, 280, 290);
    ui.scan_model_groupBox->setGeometry(10, 220, 280, 80);
    //设置单位
    /*ui.pitch_doubleSpinBox->setSuffix(" mm");
    ui.focus_angle_doubleSpinBox->setSuffix(" °");
    ui.focus_distance_doubleSpinBox->setSuffix(" mm");
    ui.sound_velocity_doubleSpinBox->setSuffix(" m/s");*/
    //当start_elm_spinBox的值变化时，end_elm_spinBox的值要相应的+1
    /*connect(ui.start_elm_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        ui.end_elm_spinBox->setValue(value + 1);
        });*/

        //扫查参数页
    ui.scanpatameter_widget->showFullScreen();

    //设置数字增益
    ui.dgain_doubleSpinBox->setRange(0, 500);
    dgain = 30.0;
    ui.dgain_doubleSpinBox->setValue(dgain);
    connect(ui.dgain_doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value) {
        dgain = value;
        emit setdgain(dgain);
        });

    //设置发射电压
    ui.volt_spinBox->setRange(0, 31);
    volt = 1;
    ui.volt_spinBox->setValue(volt);
    connect(ui.volt_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        volt = value;
        emit setvolt(volt);
        });

    //设置脉冲宽度
    ui.pulse_width_spinBox->setRange(10, 300);
    ui.pulse_width_spinBox->setSingleStep(10);
    pulse_width = 150;
    ui.pulse_width_spinBox->setValue(pulse_width);
    connect(ui.pulse_width_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        pulse_width = value;
        emit setpulsewidth(pulse_width);
        });

    //设置增益1
    ui.vga1_spinBox->setRange(0, 500);
    vga1 = 100;
    ui.vga1_spinBox->setValue(vga1);
    connect(ui.vga1_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        vga1 = value;
        emit setvga(vga1, vga2);
        });

    //设置增益2
    ui.vga2_spinBox->setRange(0, 500);
    vga2 = 50;
    ui.vga2_spinBox->setValue(vga2);
    connect(ui.vga2_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        vga2 = value;
        emit setvga(vga1, vga2);
        });

    //设置重复频率
    ui.prf_spinBox->setRange(500, 3000);
    ui.prf_spinBox->setSingleStep(100);
    prf = 2000;
    ui.prf_spinBox->setValue(prf);
    connect(ui.prf_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        prf = value;
        emit setprf(prf);
        });

    //设置压缩比
    ui.range_ratio_doubleSpinBox->setRange(0.01, 1);
    ui.range_ratio_doubleSpinBox->setSingleStep(0.01);
    range_ratio = 0.118;
    ui.range_ratio_doubleSpinBox->setValue(range_ratio);
    connect(ui.range_ratio_doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value)
        {
            range_ratio = value;
            emit setrangeratio(range_ratio);
        });

    //设置数字滤波，0 ~ 15
    ui.dfilter_spinBox->setRange(0, 15);
    dfilter = 0;
    ui.dfilter_spinBox->setValue(dfilter);
    connect(ui.dfilter_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        dfilter = value;
        emit setdfilter(dfilter);
        });

    //设置视频检波，0~4
    ui.video_detect_spinBox->setRange(0, 4);
    video_detect = 2;
    ui.video_detect_spinBox->setValue(video_detect);
    connect(ui.dfilter_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        video_detect = value;
        emit setvideodetect(video_detect);
        });

    //线扫法则
    //阵元数量
    ui.elm_num_spinBox->setRange(1, 128);
    myline_rule.elm_num = 64;
    ui.elm_num_spinBox->setValue(myline_rule.elm_num);
    connect(ui.elm_num_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        myline_rule.elm_num = value;
        emit setpalscan(myline_rule);
        });

    //阵元间距(mm)
    ui.pitch_doubleSpinBox->setRange(0.1, 3.5);
    myline_rule.pitch = 0.6;
    ui.pitch_doubleSpinBox->setValue(myline_rule.pitch);
    connect(ui.pitch_doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value)
        {
            myline_rule.pitch = value;
            emit setpalscan(myline_rule);
        });

    //孔径大小
    ui.aperture_size_spinBox->setRange(1, myline_rule.elm_num);
    myline_rule.aperture_size = 29;

    ui.aperture_size_spinBox->setValue(myline_rule.aperture_size);
    connect(ui.aperture_size_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        myline_rule.aperture_size = value;
        emit setpalscan(myline_rule);
        });

    //聚焦角度
    ui.focus_angle_doubleSpinBox->setRange(0, 180);
    myline_rule.focus_angle = 0;
    ui.focus_angle_doubleSpinBox->setValue(myline_rule.focus_angle);
    connect(ui.focus_angle_doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value)
        {
            myline_rule.focus_angle = value;
            emit setpalscan(myline_rule);
        });

    //聚焦距离(mm)
    ui.focus_distance_doubleSpinBox->setRange(0, 100);
    myline_rule.focus_distance = 0;
    ui.focus_angle_doubleSpinBox->setValue(myline_rule.focus_distance);
    connect(ui.focus_distance_doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value)
        {
            myline_rule.focus_distance = value;
            emit setpalscan(myline_rule);
        });

    //工件声速(m/s)
    ui.sound_velocity_doubleSpinBox->setRange(1, 10000);
    myline_rule.sound_velocity = 5900;
    ui.sound_velocity_doubleSpinBox->setValue(myline_rule.sound_velocity);
    connect(ui.sound_velocity_doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value)
        {
            myline_rule.sound_velocity = value;
            emit setpalscan(myline_rule);
        });

    //起始，结束阵元
    ui.start_elm_spinBox->setRange(1, 127);
    ui.end_elm_spinBox->setRange(2, 128);
    myline_rule.start_elm = 1;
    myline_rule.end_elm = 64;
    ui.start_elm_spinBox->setValue(1);
    ui.end_elm_spinBox->setValue(64);
    connect(ui.start_elm_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        myline_rule.start_elm = value;
        emit setpalscan(myline_rule);
        });
    connect(ui.end_elm_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        myline_rule.end_elm = value;
        emit setpalscan(myline_rule);
        });

    //编码器归零
    connect(ui.encoder_btn, &QPushButton::clicked, [=]() {
        emit resetencoder();
        });

    //闸门按钮
    //延时按钮
    ui.ys_horizontalScrollBar->setSingleStep(10);
    ui.ys_horizontalScrollBar->setRange(0, 200);
    connect(ui.ys_horizontalScrollBar, &QScrollBar::valueChanged, [=](int value) {
        ys = value;
        QString str = QString("延时: %1").arg(value);
        ui.ys_label->setText(str);
        send_ys_door(ys, top_door, bot_door, left_door, right_door);
        });
    //上
    ui.top_horizontalScrollBar->setRange(0, bot_door);
    ui.top_horizontalScrollBar->setValue(top_door);
    connect(ui.top_horizontalScrollBar, &QScrollBar::valueChanged, [=](int value) {
        top_door = value;
        ui.top_label->setText(QString("上：%1").arg(value));
        ui.bot_horizontalScrollBar->setRange(top_door + minwidth, B_Scan_label_height);
        send_ys_door(ys, top_door, bot_door, left_door, right_door);
        });
    //下
    ui.bot_horizontalScrollBar->setRange(top_door, B_Scan_label_height);
    ui.bot_horizontalScrollBar->setValue(bot_door);
    connect(ui.bot_horizontalScrollBar, &QScrollBar::valueChanged, [=](int value) {
        bot_door = value;
        ui.bot_label->setText(QString("下：%1").arg(value));
        ui.top_horizontalScrollBar->setRange(0, bot_door - minwidth);
        send_ys_door(ys, top_door, bot_door, left_door, right_door);
        });
    //左
    ui.left_horizontalScrollBar->setRange(0, right_door);
    ui.left_horizontalScrollBar->setValue(left_door);
    connect(ui.left_horizontalScrollBar, &QScrollBar::valueChanged, [=](int value) {
        left_door = value;
        ui.left_label->setText(QString("左：%1").arg(value));
        ui.right_horizontalScrollBar->setRange(left_door + minwidth, B_Scan_label_width);
        send_ys_door(ys, top_door, bot_door, left_door, right_door);
        });
    //右
    ui.right_horizontalScrollBar->setRange(left_door, B_Scan_label_width);
    ui.right_horizontalScrollBar->setValue(right_door);
    connect(ui.right_horizontalScrollBar, &QScrollBar::valueChanged, [=](int value) {
        right_door = value;
        ui.right_label->setText(QString("右：%1").arg(value));
        ui.left_horizontalScrollBar->setRange(0, right_door - minwidth);
        send_ys_door(ys, top_door, bot_door, left_door, right_door);
        });

    //编码器扫查精度，X轴行走1mm，22s编码器位置为144
    ui.encoder_accuracy_comboBox->setCurrentIndex(1);//默认精度为0.5mm
    connect(ui.encoder_accuracy_comboBox, &QComboBox::currentIndexChanged, [=](int dex) {
        switch (dex)
        {
        case 0:
            emit send_encoder_accuracy(0.2 * 144);
            break;
        case 1:
            emit send_encoder_accuracy(0.5 * 144);
            break;
        case 2:
            emit send_encoder_accuracy(1 * 144);
            break;
        case 3:
            emit send_encoder_accuracy(2 * 144);
            break;
        default:
            break;
        }

        });

    //辅助功能设置
    //短边水阀开关
    ui.S_water_pushButton->setCheckable(true);
    connect(ui.S_water_pushButton, &QPushButton::toggled, [=](bool status) {
        if (status) {
            dmc_write_outbit(0, 0, 0);
            ui.S_water_pushButton->setText("短边水阀关");
        }
        else {
            dmc_write_outbit(0, 0, 1);
            ui.S_water_pushButton->setText("短边水阀开");
        }
        });

    //长边水阀开关
    ui.L_water_pushButton->setCheckable(true);
    connect(ui.L_water_pushButton, &QPushButton::toggled, [=](bool status) {
        if (status) {
            dmc_write_outbit(0, 1, 0);
            ui.L_water_pushButton->setText("长边水阀关");
        }
        else {
            dmc_write_outbit(0, 1, 1);
            ui.L_water_pushButton->setText("长边水阀开");
        }
        });

    //R角水阀开关
    ui.R_water_pushButton->setEnabled(false);//禁用R角水阀按钮
    ui.R_water_pushButton->setCheckable(true);
    connect(ui.R_water_pushButton, &QPushButton::toggled, [=](bool status) {
        if (status) {
            dmc_write_outbit(0, 2, 0);
            ui.R_water_pushButton->setText("R角水阀关");
        }
        else {
            dmc_write_outbit(0, 2, 1);
            ui.R_water_pushButton->setText("R角水阀开");
        }
        });

    //抽水泵开关
    ui.water_turbo_pushButton->setCheckable(true);
    connect(ui.water_turbo_pushButton, &QPushButton::toggled, [=](bool status) {
        if (status) {
            dmc_write_outbit(0, 3, 0);
            ui.water_turbo_pushButton->setText("抽水泵关");
        }
        else {
            dmc_write_outbit(0, 3, 1);
            
            
           ui.water_turbo_pushButton->setText("抽水泵开");
        }
        });

    //扫查路径相关
    ui.tableWidget->resizeRowsToContents();//调整行内容大小
    ui.tableWidget->setColumnCount(5);//设置列数
    ui.tableWidget->setRowCount(0);//设置行数，表头不算进行数中
    ui.tableWidget->horizontalHeader()->setDefaultSectionSize(200);//标题头大小
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//横向先自适应宽度
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);//然后设置要根据内容使用宽度的列

    //设置标题头的文字
    QStringList header;
    header << tr("控制点编号") << tr("X轴坐标") << tr("Y轴坐标") << tr("Z轴坐标") << tr("舵机角度");
    ui.tableWidget->setHorizontalHeaderLabels(header);

    //设置标题头的字体样式
    QFont font = ui.tableWidget->horizontalHeader()->font();
    font.setBold(true);
    ui.tableWidget->horizontalHeader()->setFont(font);

    ui.tableWidget->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    ui.tableWidget->verticalHeader()->setDefaultSectionSize(10); //设置行距
    ui.tableWidget->setFrameShape(QFrame::NoFrame); //设置无边框
    ui.tableWidget->setShowGrid(true); //设置不显示格子线
    ui.tableWidget->verticalHeader()->setVisible(false); //设置行号列,true为显示
    ui.tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection); //可多选（Ctrl、Shift、 Ctrl+A都可以）
    ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui.tableWidget->horizontalHeader()->resizeSection(0, 100);//设置表头第一列的宽度为100
    ui.tableWidget->horizontalHeader()->setFixedHeight(30); //设置表头的高度
    ui.tableWidget->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
    ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;}"); //设置表头背景色
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//使4列表格等宽

    //设置水平、垂直滚动条样式,添加头文件 #include <QScrollBar>
    ui.tableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
        "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
        "QScrollBar::handle:hover{background:gray;}"
        "QScrollBar::sub-line{background:transparent;}"
        "QScrollBar::add-line{background:transparent;}");
    ui.tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
        "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
        "QScrollBar::handle:hover{background:gray;}"
        "QScrollBar::sub-line{background:transparent;}"
        "QScrollBar::add-line{background:transparent;}");

    ui.tableWidget->clearContents();//清除表格数据区的所有内容，但是不清除表头
}

//加载工艺
void Parameter_ui::readTechnique()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Save Binary File", QDir::currentPath() + "/扫查工艺", "TEC Files (*.tec)");
    if (filePath.isEmpty())
    {
        qDebug() << "User canceled the operation.";
        return;
    }

    QFile file(filePath);

    QFileInfo fileInfo(file);
    qint64 fileSize = fileInfo.size(); // 获取文件大小，单位为字节

    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);

        in >> dgain;//数字增益
        in >> volt;//发射电压
        in >> pulse_width;//脉冲宽度
        in >> vga1;//增益1
        in >> vga2;//增益2
        in >> prf;//重复频率
        in >> range_ratio;//压缩比
        in >> dfilter;//数字滤波
        in >> video_detect;//视频检波
        in >> myline_rule.elm_num;//阵元数
        in >> myline_rule.pitch;//阵元间距
        in >> myline_rule.aperture_size;//孔径大小
        in >> myline_rule.focus_angle;//聚焦角度
        in >> myline_rule.focus_distance;//聚焦距离
        in >> myline_rule.sound_velocity;//工件声速
        in >> myline_rule.start_elm;//起始阵元
        in >> myline_rule.end_elm;//结束阵元
        in >> top_door;
        in >> bot_door;
        in >> left_door;
        in >> right_door;
        in >> ys;

        file.close();
    }
    else
        return;

    ui.dgain_doubleSpinBox->setValue(dgain);
    ui.volt_spinBox->setValue(volt);
    ui.pulse_width_spinBox->setValue(pulse_width);
    ui.vga1_spinBox->setValue(vga1);
    ui.vga2_spinBox->setValue(vga2);
    ui.prf_spinBox->setValue(prf);
    ui.range_ratio_doubleSpinBox->setValue(range_ratio);
    ui.dfilter_spinBox->setValue(dfilter);
    ui.video_detect_spinBox->setValue(video_detect);
    ui.elm_num_spinBox->setValue(myline_rule.elm_num);
    ui.pitch_doubleSpinBox->setValue(myline_rule.pitch);
    ui.aperture_size_spinBox->setValue(myline_rule.aperture_size);
    ui.focus_angle_doubleSpinBox->setValue(myline_rule.focus_angle);
    ui.focus_distance_doubleSpinBox->setValue(myline_rule.focus_distance);
    ui.sound_velocity_doubleSpinBox->setValue(myline_rule.sound_velocity);
    ui.start_elm_spinBox->setValue(myline_rule.start_elm);
    ui.end_elm_spinBox->setValue(myline_rule.end_elm);
    ui.top_horizontalScrollBar->setValue(top_door);
    ui.bot_horizontalScrollBar->setValue(bot_door);
    ui.left_horizontalScrollBar->setValue(left_door);
    ui.right_horizontalScrollBar->setValue(right_door);
    ui.ys_horizontalScrollBar->setValue(ys);

    QMessageBox msgBox;
    msgBox.setText("工艺加载完成！");
    msgBox.setWindowTitle("提示");
    msgBox.setWindowFlags(msgBox.windowFlags() | Qt::WindowStaysOnTopHint);
    msgBox.exec();

}

//保存工艺
void Parameter_ui::saveTechnique()
{
    QString filePath = QFileDialog::getSaveFileName(nullptr, "Save Binary File", QDir::currentPath() + "/扫查工艺", "TEC Files (*.tec)");
    if (filePath.isEmpty())
    {
        qDebug() << "User canceled the operation.";
        return;
    }
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);

        out << dgain;//数字增益
        out << volt;//发射电压
        out << pulse_width;//脉冲宽度
        out << vga1;//增益1
        out << vga2;//增益2
        out << prf;//重复频率
        out << range_ratio;//压缩比
        out << dfilter;//数字滤波
        out << video_detect;//视频检波
        out << myline_rule.elm_num;//阵元数
        out << myline_rule.pitch;//阵元间距
        out << myline_rule.aperture_size;//孔径大小
        out << myline_rule.focus_angle;//聚焦角度
        out << myline_rule.focus_distance;//聚焦距离
        out << myline_rule.sound_velocity;//工件声速
        out << myline_rule.start_elm;//起始阵元
        out << myline_rule.end_elm;//结束阵元
        out << top_door;
        out << bot_door;
        out << left_door;
        out << right_door;
        out << ys;
        file.close();

        QMessageBox msgBox;
        msgBox.setText("工艺保存成功！");
        msgBox.setWindowTitle("提示");
        msgBox.setWindowFlags(msgBox.windowFlags() | Qt::WindowStaysOnTopHint);
        msgBox.exec();
    }
}

//新建扫查路径
void Parameter_ui::creat_path()
{
    ui.tableWidget->clearContents(); // 清除所有单元格中的内容
    ui.tableWidget->setRowCount(0); // 将表格的行数设置为零，从而删除所有行
}

//添加控制点
void Parameter_ui::add_pos(double x, double y, double z, double angle)
{
    int curRow = ui.tableWidget->rowCount();
    ui.tableWidget->insertRow(curRow);//在表格尾部添加一行

    //在表格中添加内容
    ui.tableWidget->setItem(curRow, 0, new QTableWidgetItem(QString::number(curRow + 1)));
    ui.tableWidget->setItem(curRow, 1, new QTableWidgetItem(QString::number(x)));
    ui.tableWidget->setItem(curRow, 2, new QTableWidgetItem(QString::number(y)));
    ui.tableWidget->setItem(curRow, 3, new QTableWidgetItem(QString::number(z)));
    ui.tableWidget->setItem(curRow, 4, new QTableWidgetItem(QString::number(angle)));

}

//删除控制点
void Parameter_ui::delete_pos()
{
    int curRow = ui.tableWidget->currentRow();
    ui.tableWidget->removeRow(curRow); //删除当前行及其items

    //删除控制点后，需要更新控制点编号
    int allRow = ui.tableWidget->rowCount();//总行数
    for (int i = 0; i < allRow; i++)
        ui.tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
}

//加载控制点
void Parameter_ui::read_path()
{
    QVector<QVector<double>>pos_Vector;

    QString scanpath_filepath = QFileDialog::getOpenFileName(nullptr, "Save Binary File", QDir::currentPath() + "/扫查路径", "Binary Files (*.bin)");
    if (scanpath_filepath.isEmpty())
    {
        qDebug() << "User canceled the operation.";
        return;
    }

    ////加载路径之后，自动扫查按钮为激活状态
    //ui.Scan_pushButton->setEnabled(true);

    QFile file(scanpath_filepath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open temp file for reading:" << file.errorString();
        file.close();
        return;
    }

    QDataStream in(&file);

    int allRow;
    int pathmodel_SLR;
    in >> allRow;
    in >> pathmodel_SLR;

    for (int row = 0; row < allRow; row++)
    {
        QVector<double>temp_Vector;
        for (int column = 1; column < 5; column++)
        {
            double tempvalue;
            in >> tempvalue;
            temp_Vector.append(tempvalue);
        }
        pos_Vector.append(temp_Vector);
    }

    ui.tableWidget->setColumnCount(5);//设置列数
    ui.tableWidget->setRowCount(allRow);//设置行数，表头不算进行数中
    for (int i = 0; i < pos_Vector.size(); i++)
    {
        ui.tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        for (int j = 0; j < pos_Vector[i].size(); j++)
        {
            qDebug() << "第 " << i + 1 << "控制点，第 " << j + 1 << "个值为：" << pos_Vector[i][j];
            ui.tableWidget->setItem(i, j + 1, new QTableWidgetItem(QString::number(pos_Vector[i][j])));
        }
    }

    file.close();
    emit send_path(pathmodel_SLR, pos_Vector);
}

//保存控制点
void Parameter_ui::save_path()
{
    int allRow = ui.tableWidget->rowCount();//获取控制点总数
    int temppath;
    //获取所保存的是什么路径
    QMessageBox msgBox;
    msgBox.setText("Choose an path:");
    msgBox.addButton("S_path", QMessageBox::ActionRole);
    msgBox.addButton("L_path", QMessageBox::ActionRole);
    msgBox.addButton("R_path", QMessageBox::ActionRole);
    // 设置消息框总在顶层显示
    msgBox.setWindowFlags(msgBox.windowFlags() | Qt::WindowStaysOnTopHint);
    msgBox.exec();

    // 获取用户选择的按钮
    QAbstractButton* selectedButton = msgBox.clickedButton();
    if (selectedButton) {
        QString buttonText = selectedButton->text();
        if (buttonText == "S_path") {
            temppath = 1;
        }
        else if (buttonText == "L_path") {
            temppath = 2;
        }
        else if (buttonText == "R_path") {
            temppath = 3;
        }
    }

    QString tempfilePath = QFileDialog::getSaveFileName(nullptr, "Save Binary File", QDir::currentPath() + "/扫查路径", "Binary Files (*.bin)");
    if (tempfilePath.isEmpty())
    {
        qDebug() << "User canceled the operation.";
        return;
    }

    QFile file(tempfilePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to open temp file for reading:" << file.errorString();
        file.close();
        return;
    }

    QDataStream out(&file);

    out << allRow;//记录有多少个控制点
    out << temppath;//保存扫查模式

    for (int row = 0; row < allRow; row++)
    {
        for (int column = 1; column < 5; column++)
        {
            QTableWidgetItem* item = ui.tableWidget->item(row, column);
            QString cellText = item->text();
            double value = cellText.toDouble();
            out << value;
        }
    }

    file.close();
}

Parameter_ui::~Parameter_ui()
{}
