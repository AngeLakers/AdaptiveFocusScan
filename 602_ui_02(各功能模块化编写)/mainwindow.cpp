#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //初始化各控件位置
    set_ui_position();

    //两个子窗口
    motion_ui = new Motion_ui;
    parameter_ui = new Parameter_ui;
    //让参数设置ui,运动设置ui图层显示位于顶部
    parameter_ui->setWindowFlags(Qt::WindowStaysOnTopHint);
    motion_ui->setWindowFlags(Qt::WindowStaysOnTopHint);

    //舵机控制模块
    Servo_model = new Servocontrol;
    //22s控制模块
    Pa22s_model = new Pa22scontrol;

    //运动控制线程
    Motionthread = new Motioncontrol;
    QThread* motionobjectthread = new QThread;
    Motionthread->moveToThread(motionobjectthread);
    motionobjectthread->start();
    //数据处理线程
    Dataprocessthread = new DataProcess;//不能指定父对象 
    QThread* dataobjectthread = new QThread;
    Dataprocessthread->moveToThread(dataobjectthread);
    dataobjectthread->start();
    //数据保存线程
    Datasavethread = new DataSave;
    QThread* datasaveobjectthread = new QThread;
    Datasavethread->moveToThread(datasaveobjectthread);
    datasaveobjectthread->start();


    //连接运动控制窗口各控件
    connectmotion_ui();
    //连接扫查设置窗口各控件
    connectparatemer_ui();
}

//初始化各控件位置
void MainWindow::set_ui_position()
{
    //ui控件位置大小设置
    setFixedSize(1300, 900);
    setWindowTitle("加热组件相控阵检测系统");

    //图形显示Label
    ui.A_Scan_widget->setGeometry(20, 10, 290, 600);
    ui.B_Scan_label->setGeometry(320, 10, 800, 600);
    ui.C_Scan_label->setGeometry(20, 620, 1100, 240);

    //GroupBox
    ui.control_groupBox->setGeometry(1140, 10, 140, 320);
    ui.live_status_groupBox->setGeometry(1140, 350, 140, 510);

    //控制栏按钮，先在ui设计界面中将按钮拖入到相应的QGroupBox中
    ui.Scan_pushButton->setGeometry(10, 20, 120, 40);
    ui.parameter_set_pushButton->setGeometry(10, 70, 120, 40);
    ui.motion_set_pushButton->setGeometry(10, 120, 120, 40);
    ui.save_data_pushButton->setGeometry(10, 170, 120, 40);
    ui.read_data_pushButton->setGeometry(10, 220, 120, 40);
    ui.stop_pushButton->setGeometry(10, 270, 120, 40);
    ui.stop_pushButton->setStyleSheet("background-color: pink;");

    //状态栏控件
    ui.scms_label->setGeometry(10, 20, 111, 31);
    ui.scmsxs_label->setGeometry(10, 50, 111, 31);
    ui.yscjl_label->setGeometry(10, 90, 111, 31);
    ui.yscjlxs_label->setGeometry(10, 120, 111, 31);
    ui.sfzt_label->setGeometry(10, 160, 111, 31);
    ui.sfztxs_label->setGeometry(10, 190, 111, 31);
    ui.djzt_label->setGeometry(10, 230, 111, 31);
    ui.djztxs_label->setGeometry(10, 260, 111, 31);
    ui.xkzwd_label->setGeometry(10, 300, 111, 31);
    ui.xkzwdxs_label->setGeometry(10, 330, 111, 31);

}

//连接扫查设置窗口各控件
void MainWindow::connectparatemer_ui()
{
    //扫查设置窗口显示
    connect(ui.parameter_set_pushButton, &QPushButton::clicked, [=]() {
        parameter_ui->setWindowFlags(Qt::WindowStaysOnTopHint);
        parameter_ui->show();
        parameter_ui->ui.tabWidget->setCurrentIndex(0);
        });
    //B_Label双击调出扫查设置
    connect(ui.B_Scan_label, &MouseEventLabel::doubleClicked, [=]() {
        if (!parameter_ui->isVisible())
            parameter_ui->show();
        parameter_ui->ui.tabWidget->setCurrentIndex(2);
        });

    //相关定时器
    refreshtimer = new QTimer(this);
    TimeCscantimer = new QTimer(this);

    //扫查模式选择
    //默认短边扫查
    scanmodel_SLR = 1;
    parameter_ui->ui.short_radioButton->setChecked(true);
    ui.scmsxs_label->setText(QString("短边扫查"));
    connect(parameter_ui->ui.short_radioButton, &QRadioButton::clicked, [=]() {
        scanmodel_SLR = 1;
        ui.scmsxs_label->setText(QString("短边扫查"));
        });
    connect(parameter_ui->ui.long_radioButton, &QRadioButton::clicked, [=]() {
        scanmodel_SLR = 2;
        ui.scmsxs_label->setText(QString("长边扫查"));
        });
    connect(parameter_ui->ui.R_radioButton, &QRadioButton::clicked, [=]() {
        scanmodel_SLR = 3;
        ui.scmsxs_label->setText(QString("R角扫查"));
        });
    //扫查模式  0-编码器  1-时基
    parameter_ui->ui.scanmodel_comboBox->setCurrentIndex(0);
    connect(parameter_ui->ui.scanmodel_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int dex) {
        scanmodel = dex;
        });
    // 编码器正向反向 0 - 正向  1 - 反向
    parameter_ui->ui.encodermodel_comboBox->setCurrentIndex(0);
    connect(parameter_ui->ui.encodermodel_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int dex) {
        Dataprocessthread->encoderdex = dex;
        });

    //连接相关指令
    //连接Pa22s
    connect(parameter_ui->ui.connect_to_22s_pushButton, &QPushButton::clicked, [=]() {
        Pa22s_model->connect_Pa22s();
        });
    //断开与Pa22s的连接
    connect(parameter_ui->ui.disconnect_to_22s_pushButton, &QPushButton::clicked, [=]() {
        Pa22s_model->disconnect_Pa22s();
        });
    //关机
    connect(parameter_ui->ui.close_22s_pushButton, &QPushButton::clicked, [=]() {
        Pa22s_model->turn_off_Pa22s();
        });
    //扫查信号开
    connect(parameter_ui->ui.open_22s_signal_pushButton, &QPushButton::clicked, [=]() {
        Pa22s_model->open_signal();
        refreshtimer->start(30);
        B_LabelMouseStatus = true;
        });
    //扫查信号关
    connect(parameter_ui->ui.close_22s_signal_pushButton, &QPushButton::clicked, [=]() {
        Pa22s_model->close_signal();
        refreshtimer->stop();
        B_LabelMouseStatus = false;
        });


    //设置数字增益
    connect(parameter_ui, &Parameter_ui::setdgain, Pa22s_model, &Pa22scontrol::setdgain);
    //设置发射电压
    connect(parameter_ui, &Parameter_ui::setvolt, Pa22s_model, &Pa22scontrol::setvolt);
    //设置脉冲宽度
    connect(parameter_ui, &Parameter_ui::setpulsewidth, Pa22s_model, &Pa22scontrol::setpulsewidth);
    //设置增益1，增益2
    connect(parameter_ui, &Parameter_ui::setvga, Pa22s_model, &Pa22scontrol::setvga);
    //设置重复频率
    connect(parameter_ui, &Parameter_ui::setprf, Pa22s_model, &Pa22scontrol::setprf);
    //设置压缩比
    connect(parameter_ui, &Parameter_ui::setrangeratio, Pa22s_model, &Pa22scontrol::setrangeratio);
    //设置数字滤波，0 ~ 15
    connect(parameter_ui, &Parameter_ui::setdfilter, Pa22s_model, &Pa22scontrol::setdfilter);
    //设置视频检波，0~4    //视频检波无法设置
    connect(parameter_ui, &Parameter_ui::setvideodetect, Pa22s_model, &Pa22scontrol::setvideodetect);
    //设置线扫法则
    connect(parameter_ui, &Parameter_ui::setpalscan, Pa22s_model, &Pa22scontrol::setpalscan);
    //编码器归零
    connect(parameter_ui, &Parameter_ui::resetencoder, Pa22s_model, &Pa22scontrol::resetencoder);



    // a扫波形初始化
    chart = new QChart;
    Xaixs = new QValueAxis;
    Yaixs = new QValueAxis;
    chart->addAxis(Xaixs, Qt::AlignBottom);
    chart->addAxis(Yaixs, Qt::AlignLeft);
    Yaixs->setRange(0, 256);
    Xaixs->setRange(0, 200);//为什么添加400个点内存占用就很明显
    Xaixs->hide();
    Yaixs->hide();
    chart->legend()->hide();
    ui.A_Scan_widget->setChart(chart);
    ui.A_Scan_widget->rotate(90);//翻转90度显示 

    //B扫闸门相关，闸门框初始化
    top_door = 200;
    bot_door = 400;
    left_door = 0;
    right_door = ui.B_Scan_label->width();
    BScanDoor = QRect(left_door, top_door, right_door - left_door, bot_door - top_door);
    BScanDoorPixmap = QPixmap(ui.B_Scan_label->size());
    BScanDoorPixmap.fill(Qt::transparent);//透明背景
    BScanPainter.begin(&BScanDoorPixmap);//设置绘制的对象
    BScanPainter.setPen(Qt::red);
    BScanPainter.drawRect(BScanDoor);
    BScanPainter.end();

    //开始扫查
    //开始扫查/结束扫查
    ui.Scan_pushButton->setCheckable(true);
    ui.Scan_pushButton->setEnabled(false);//未选择扫查路径前，将无法按下按钮
    connect(ui.Scan_pushButton, &QPushButton::toggled, [=](bool status) {
        if (status)//按钮处于按下状态，启动扫查
        {
            ui.Scan_pushButton->setText("结束扫查");
            ui.Scan_pushButton->setStyleSheet("background-color: red;");
            Dataprocessthread->Cscandex = 0;
            Dataprocessthread->CImage = QImage(36000, Dataprocessthread->CImageHeight, QImage::Format_RGB32);//先宽后高，确定临时C扫图像的大小
            Dataprocessthread->CImage.fill(Qt::white);//先填充白色
            ui.C_Scan_label->setPixmap(QPixmap::fromImage(Dataprocessthread->CscaledImage));//强制类型转换 
            Pa22s_model->open_signal();
            B_LabelMouseStatus = true;
            C_LabelMouseStatus = false;
            data_status = false;
            emit startsave(Dataprocessthread->signal_num, top_door, bot_door, left_door, right_door, ys);
            if (scanmodel == 0) {//编码器扫查
                Pa22s_model->resetencoder();
                Dataprocessthread->EncoderCscandex = 0;//扫查完后，编码器位置不会乱动
                Dataprocessthread->encoderlastpos = 0;//编码器历史位置归零
                Dataprocessthread->encoder_first_scanstatus = true;
                Dataprocessthread->EncoderCscanstatus = true;
            }
            else {//时基扫查
                //多线程C扫启动，此处可以设定时基C扫速率
                TimeCscantimer->start(20);
                TimeCscanstatus = true;
            }
            //启动自动扫查
            emit start_auto_scan(pos_Vector, pathmodel_SLR);

        }
        else//按钮恢复，结束扫查
        {
            //停止轴运动
            //紧急停止
                Motionthread->stop_all_axis();
                Motionthread->axis_stopstatus = true;

            //关闭实时B扫及停止数据采集
            Pa22s_model->close_signal();
            refreshtimer->stop();//时基
            ui.Scan_pushButton->setText("开始扫查");
            ui.Scan_pushButton->setStyleSheet("");
            B_LabelMouseStatus = false;

            if (scanmodel == 0) {
                Dataprocessthread->EncoderCscanstatus = false;//编码器
                Pa22s_model->resetencoder();//编码器
            }
            else {
                TimeCscantimer->stop();//时基
                TimeCscanstatus = false;//时基
            }
            //通知子线程停止存储数据
            emit stopsave(Dataprocessthread->Cscandex, Dataprocessthread->signal_num);

            //替换显示的图片
            CImagelength = Dataprocessthread->Cscandex + 1;
            CImage = Dataprocessthread->CImage.copy(0, 0, CImagelength/*实际照片长度*/, Dataprocessthread->CImage.height());//将图像复制
            if (Dataprocessthread->Cscandex >= ui.C_Scan_label->width())
            {
                showCImagedex = Dataprocessthread->Cscandex - ui.C_Scan_label->width() + 1;//照片起始显示索引，相当于CImagelength-ui.C_Scan_label->width()
                //（当CImagelength=1000，ui.C_Scan_label->width()=1000时，带入即可推导出对应关系）
                QImage copyImage = CImage.copy(showCImagedex, 0, ui.C_Scan_label->width(), CImage.height());
                CScaledImage = copyImage.scaled(ui.C_Scan_label->width(), ui.C_Scan_label->height());
                ui.C_Scan_label->setPixmap(QPixmap::fromImage(CScaledImage));//强制类型转换
            }
            else
            {
                QImage copyImage = CImage.copy(0, 0, CImagelength, CImage.height());
                CScaledImage = copyImage.scaled(copyImage.width(), ui.C_Scan_label->height());
                ui.C_Scan_label->setPixmap(QPixmap::fromImage(CScaledImage));//强制类型转换
            }
        }
        });

    //手动扫查
    parameter_ui->ui.start_Mt_scan_pushButton->setCheckable(true);
    connect(parameter_ui->ui.start_Mt_scan_pushButton, &QPushButton::toggled, [=](bool status) {
        if (status)//按钮处于按下状态，启动扫查
        {
            parameter_ui->ui.start_Mt_scan_pushButton->setText("结束扫查");
            parameter_ui->ui.start_Mt_scan_pushButton->setStyleSheet("background-color: red;");
            Dataprocessthread->Cscandex = 0;
            Dataprocessthread->CImage = QImage(36000, Dataprocessthread->CImageHeight, QImage::Format_RGB32);//先宽后高，确定临时C扫图像的大小
            Dataprocessthread->CImage.fill(Qt::white);//先填充白色
            ui.C_Scan_label->setPixmap(QPixmap::fromImage(Dataprocessthread->CscaledImage));//强制类型转换 
            Pa22s_model->open_signal();
            B_LabelMouseStatus = true;
            C_LabelMouseStatus = false;
            data_status = false;
            emit startsave(Dataprocessthread->signal_num, top_door, bot_door, left_door, right_door, ys);
            if (scanmodel == 0) {//编码器扫查
                Pa22s_model->resetencoder();
                Dataprocessthread->EncoderCscandex = 0;//扫查完后，编码器位置不会乱动
                Dataprocessthread->encoderlastpos = 0;//编码器历史位置归零
                Dataprocessthread->encoder_first_scanstatus = true;
                Dataprocessthread->EncoderCscanstatus = true;
            }
        }
        else//按钮恢复，结束扫查
        {
            //关闭实时B扫及停止数据采集
            Pa22s_model->close_signal();
            refreshtimer->stop();//时基
            parameter_ui->ui.start_Mt_scan_pushButton->setText("开始扫查");
            parameter_ui->ui.start_Mt_scan_pushButton->setStyleSheet("");
            B_LabelMouseStatus = false;

            if (scanmodel == 0) {
                Dataprocessthread->EncoderCscanstatus = false;//编码器
                Pa22s_model->resetencoder();//编码器
            }
            //通知子线程停止存储数据
            emit stopsave(Dataprocessthread->Cscandex, Dataprocessthread->signal_num);

            //替换显示的图片
            CImagelength = Dataprocessthread->Cscandex + 1;
            CImage = Dataprocessthread->CImage.copy(0, 0, CImagelength/*实际照片长度*/, Dataprocessthread->CImage.height());//将图像复制
            if (Dataprocessthread->Cscandex >= ui.C_Scan_label->width())
            {
                showCImagedex = Dataprocessthread->Cscandex - ui.C_Scan_label->width() + 1;//照片起始显示索引，相当于CImagelength-ui.C_Scan_label->width()
                //（当CImagelength=1000，ui.C_Scan_label->width()=1000时，带入即可推导出对应关系）
                QImage copyImage = CImage.copy(showCImagedex, 0, ui.C_Scan_label->width(), CImage.height());
                CScaledImage = copyImage.scaled(ui.C_Scan_label->width(), ui.C_Scan_label->height());
                ui.C_Scan_label->setPixmap(QPixmap::fromImage(CScaledImage));//强制类型转换
            }
            else
            {
                QImage copyImage = CImage.copy(0, 0, CImagelength, CImage.height());
                CScaledImage = copyImage.scaled(copyImage.width(), ui.C_Scan_label->height());
                ui.C_Scan_label->setPixmap(QPixmap::fromImage(CScaledImage));//强制类型转换
            }
        }
        });

    //显示刷新定时器
    connect(refreshtimer, &QTimer::timeout, [=]() {
        emit startshowBscan();//子线程中进行B扫数据处理
        showLiveAscan();//显示A扫波形

        // 显示闸门框，将临时的QPixmap叠加到BImage上
        QImage scaledimage = BImage.scaled(ui.B_Scan_label->width(), ui.B_Scan_label->height());
        QPixmap scaledImagePixmap = QPixmap::fromImage(scaledimage);
        QPainter combinedPainter(&scaledImagePixmap);
        combinedPainter.drawPixmap(0, 0, BScanDoorPixmap);
        // 在QLabel上显示叠加后的图片
        ui.B_Scan_label->setPixmap(scaledImagePixmap);

        //当C扫未开始时，将不会刷新
        if (TimeCscanstatus || Dataprocessthread->EncoderCscanstatus) {
            //对图像进行放大
            QImage scaledimage = CImage.scaled(Dataprocessthread->CscaledImage.width(), ui.C_Scan_label->height());
            ui.C_Scan_label->setPixmap(QPixmap::fromImage(scaledimage));
        }
        });

    //时基C扫定时器
    connect(TimeCscantimer, &QTimer::timeout, [=]() {
        Dataprocessthread->Cscandex++;
        emit startshowCscan();
        });

    //保存扫查数据
    connect(ui.save_data_pushButton, &QPushButton::clicked, [=]() {
        QString filePath = QFileDialog::getSaveFileName(nullptr, "Save Binary File", QDir::currentPath() + "/扫查数据", "Binary Files (*.bin)");
        emit savedata(filePath);
        });
    //读取扫查数据
    connect(ui.read_data_pushButton, &QPushButton::clicked, [=]() {
        data_status = true;
        read_data();
        });
    //加载工艺
    connect(parameter_ui->ui.readTechnique_pushButton, &QPushButton::clicked, [=]() {
        Pa22s_model->close_signal();
        parameter_ui->readTechnique();
        Pa22s_model->open_signal();
        });
    //保存工艺
    connect(parameter_ui->ui.saveTechnique_pushButton, &QPushButton::clicked, [=]() {
        parameter_ui->saveTechnique();
        });
    //重新处理按钮
    connect(parameter_ui->ui.reprocess_pushButton, &QPushButton::clicked, [=]() {
        if (data_status)
            showCImage();
        });
    //新建扫查路径
    connect(parameter_ui->ui.creat_path_pushButton, &QPushButton::clicked, [=]() {
        parameter_ui->creat_path();
        });
    //添加控制点
    connect(parameter_ui->ui.add_pos_pushButton, &QPushButton::clicked, [=]() {
        switch (scanmodel_SLR)
        {
        case 1:
            parameter_ui->add_pos(Motionthread->get_axis_position(0), Motionthread->get_axis_position(2), Motionthread->get_axis_position(3), motion_ui->ui.S_angle_doubleSpinBox->value());
            break;
        case 2:
            parameter_ui->add_pos(Motionthread->get_axis_position(0), Motionthread->get_axis_position(2), Motionthread->get_axis_position(3), motion_ui->ui.L_angle_doubleSpinBox->value());
            break;
        case 3:
            parameter_ui->add_pos(Motionthread->get_axis_position(0), Motionthread->get_axis_position(2), Motionthread->get_axis_position(4), motion_ui->ui.R_angle_doubleSpinBox->value());
            break;
        default:
            break;
        }
        });
    //删除控制点
    connect(parameter_ui->ui.delete_pos_pushButton, &QPushButton::clicked, [=]() {
        parameter_ui->delete_pos();
        });
    //加载控制点
    connect(parameter_ui->ui.read_path_pushButton, &QPushButton::clicked, [=]() {
        parameter_ui->read_path();
        //加载路径之后，自动扫查按钮为激活状态
        //ui.Scan_pushButton->setEnabled(true);
        });
    //保存控制点
    connect(parameter_ui->ui.save_path_pushButton, &QPushButton::clicked, [=]() {
        parameter_ui->save_path();
        });


    //在子线程中启动B扫数据处理
    connect(this, &MainWindow::startshowBscan, Dataprocessthread, &DataProcess::showBscan);
    //在子线程中启动时基C扫
    connect(this, &MainWindow::startshowCscan, Dataprocessthread, &DataProcess::showCscan);
    //在子线程中启动编码器C扫
    connect(Dataprocessthread, &DataProcess::startEncoderCscan, Dataprocessthread, &DataProcess::showCscan);
    //Dataprocess将数据传递到Datasave中进行保存
    qRegisterMetaType<QVector<int>>("QVector<int>"); //注册QVector<int>类型，以能在信号槽传参
    connect(Dataprocessthread, &DataProcess::sendDataToSave, Datasavethread, &DataSave::savetempdata);
    connect(this, &MainWindow::startsave, Datasavethread, &DataSave::startsave);
    connect(this, &MainWindow::stopsave, Datasavethread, &DataSave::stopsave);
    connect(this, &MainWindow::savedata, Datasavethread, &DataSave::savedata);
    //数据保存进度
    connect(Datasavethread, &DataSave::progressUpdated, this, &MainWindow::show_savedata_progress);
    //22s数据传递到Dataprocess线程
    connect(Pa22s_model, &Pa22scontrol::sendData, Dataprocessthread, &DataProcess::getdata);
    //将线扫法则数据从parameter_ui中传递到Dataprocess中,Line_rule为注册将不能作为参数在信号槽中传递
    qRegisterMetaType<Line_rule>("Line_rule"); //注册Line_rule类型，以能在信号槽传参
    connect(parameter_ui, &Parameter_ui::setpalscan, Dataprocessthread, &DataProcess::getLine_rule);
    //接收Dataprocess线程的数据，以显示A扫
    connect(Dataprocessthread, &DataProcess::sendAscanData, [=](QVector<int>data) {
        AscanData = data;
        });
    //接收Dataprocess线程中的B扫图
    connect(Dataprocessthread, &DataProcess::sendBscanImage, [=](QImage Image) {
        //BImage = Image;
        BImage = Image.copy(0, 0, Image.width(), Image.height());//不能直接赋值，直接赋值相当于引用，当Image销毁时，BImage将无法访问，导致内存访问冲突
        });
    //接收Dataprocess线程中的C扫图
    connect(Dataprocessthread, &DataProcess::sendCscanImage, [=](QImage Image) {
        CImage = Image.copy(0, 0, Image.width(), Image.height());
        });
    //延时，闸门改变时，进行相应操作
    connect(parameter_ui, &Parameter_ui::send_ys_door, this, &MainWindow::ys_door_changed);
    connect(parameter_ui, &Parameter_ui::send_ys_door, Dataprocessthread, &DataProcess::ys_door_changed);
    //传递编码器精度
    connect(parameter_ui, &Parameter_ui::send_encoder_accuracy, [=](double value){
        Dataprocessthread->encoder_accuracy = value;
        });
    //扫查路径传递
    qRegisterMetaType<QVector<QVector<double>>>("QVector<QVector<double>>"); //注册QVector<QVector<double>>类型
    connect(parameter_ui, &Parameter_ui::send_path, [=](int path, QVector<QVector<double>>vector) {
        pathmodel_SLR = path;
        pos_Vector = vector;
        ui.Scan_pushButton->setEnabled(true);
        qDebug() << "再次接收了数据 ";
        });
    connect(this, &MainWindow::start_auto_scan, Motionthread, &Motioncontrol::auto_scan);//启动自动扫查
    connect(Motionthread, &Motioncontrol::start_duoji, Servo_model,&Servocontrol::writeToclient);//接收舵机调姿信号


    //各类鼠标事件
    //鼠标滚轮移动事件
    connect(ui.C_Scan_label, &MouseEventLabel::mousewheel, this, [=](int value) {//value为鼠标移动的距离
        if (CImage.width() > ui.C_Scan_label->width() && !TimeCscantimer->isActive()) {//只有当C扫结束后且图片宽度大于500时
            for (int i = 0; i < 20; i++) {//让图片移动20次
                showCImage_usingwheel(value);
            }
        }
        });
    //接收B扫显示图x坐标，显示第几条数据的A扫
    connect(ui.B_Scan_label, &MouseEventLabel::mousePressd, this, [=](int x) {
        if (B_LabelMouseStatus)
        {
            if (refreshtimer->isActive())
            {
                LiveAscandex = (x / (ui.B_Scan_label->width() / Dataprocessthread->signal_num));//算出是第几条数据
            }
            if (data_status)
            {
                data_AWavedex = (x / (ui.B_Scan_label->width() / data_signal_num));//算出是第几条数据
                showAWave();
            }
        }
        });
    //接收读取数据时C扫图像的x坐标以显示B扫图像
    data_BScandex = 0;
    connect(ui.C_Scan_label, &MouseEventLabel::mousePressd, this, [=](int value) {
        if (C_LabelMouseStatus)
        {
            data_BScandex = value;
            showBImage();
        }
        });
    //B_Label双击调出扫查设置
    connect(ui.B_Scan_label, &MouseEventLabel::doubleClicked, [=]() {
        if (!parameter_ui->isVisible())
            parameter_ui->show();
        parameter_ui->ui.tabWidget->setCurrentIndex(2);
        });
}

//连接运动控制窗口各控件
void MainWindow::connectmotion_ui()
{
    // 运动控制窗口显示
    connect(ui.motion_set_pushButton, &QPushButton::clicked, [=]() {
        motion_ui->show();
        });

    //伺服运动速度设置
    //X
    motion_ui->ui.X_speed_spinBox->setValue(1);//初始速度为1mm/s
    motion_ui->ui.X_speed_spinBox->setRange(1, 200);//速度范围：1~200mm/s
    connect(motion_ui->ui.X_speed_spinBox, &QSpinBox::editingFinished, [=]() {
        X_speed = motion_ui->ui.X_speed_spinBox->value() * X_pitch;
        qDebug() << "X_speed= " << X_speed;
        });
    //Y
    motion_ui->ui.Y_speed_spinBox->setValue(Y_speed / Y_pitch);
    motion_ui->ui.Y_speed_spinBox->setRange(1, 20);//速度范围：1~20mm/s
    connect(motion_ui->ui.Y_speed_spinBox, &QSpinBox::editingFinished, [=]() {
        Y_speed = motion_ui->ui.Y_speed_spinBox->value() * Y_pitch;
        qDebug() << "Y_speed= " << Y_speed;
        });
    //Z1
    motion_ui->ui.Z1_speed_spinBox->setValue(Z1_speed / Z1_pitch);
    motion_ui->ui.Z1_speed_spinBox->setRange(1, 10);//速度范围：1~10mm/s
    connect(motion_ui->ui.Z1_speed_spinBox, &QSpinBox::editingFinished, [=]() {
        Z1_speed = motion_ui->ui.Z1_speed_spinBox->value() * Z1_pitch;
        qDebug() << "Z1_speed= " << Z1_speed;
        });
    //Z2
    motion_ui->ui.Z2_speed_spinBox->setValue(Z2_speed / Z2_pitch);
    motion_ui->ui.Z2_speed_spinBox->setRange(1, 10);//速度范围：1~10mm/s
    connect(motion_ui->ui.Z2_speed_spinBox, &QSpinBox::editingFinished, [=]() {
        Z2_speed = motion_ui->ui.Z2_speed_spinBox->value() * Z2_pitch;
        qDebug() << "Z2_speed= " << Z2_speed;
        });

    //伺服运动距离获取
    //X
    motion_ui->ui.X_distance_doubleSpinBox->setValue(0);
    motion_ui->ui.X_distance_doubleSpinBox->setSingleStep(1);
    connect(motion_ui->ui.X_distance_doubleSpinBox, &QDoubleSpinBox::editingFinished, [=]() {
        X_distance = motion_ui->ui.X_distance_doubleSpinBox->value() * X_pitch;
        qDebug() << "X_distance= " << X_distance;
        });
    //Y
    motion_ui->ui.Y_distance_doubleSpinBox->setValue(0);
    motion_ui->ui.Y_distance_doubleSpinBox->setSingleStep(1);
    connect(motion_ui->ui.Y_distance_doubleSpinBox, &QDoubleSpinBox::editingFinished, [=]() {
        Y_distance = motion_ui->ui.Y_distance_doubleSpinBox->value() * Y_pitch;
        qDebug() << "Y_distance= " << Y_distance;
        });
    //Z1
    motion_ui->ui.Z1_distance_doubleSpinBox->setValue(0);
    motion_ui->ui.Z1_distance_doubleSpinBox->setSingleStep(1);
    connect(motion_ui->ui.Z1_distance_doubleSpinBox, &QDoubleSpinBox::editingFinished, [=]() {
        Z1_distance = motion_ui->ui.Z1_distance_doubleSpinBox->value() * Z1_pitch;
        qDebug() << "Z1_distance= " << Z1_distance;
        });
    //Z2
    motion_ui->ui.Z2_distance_doubleSpinBox->setValue(0);
    motion_ui->ui.Z2_distance_doubleSpinBox->setSingleStep(1);
    connect(motion_ui->ui.Z2_distance_doubleSpinBox, &QDoubleSpinBox::editingFinished, [=]() {
        Z2_distance = motion_ui->ui.Z2_distance_doubleSpinBox->value() * Z2_pitch;
        qDebug() << "Z2_distance= " << Z2_distance;
        });

    //读取限位，轴位置定时器
    read_axis_inport_timer = new QTimer(this);
    connect(read_axis_inport_timer, &QTimer::timeout, [=]() {
        //读取限位开关信号
        read_axis_inport();
        //显示轴位置
        motion_ui->ui.X_position_label->setText(QString("X轴位置：\n%1").arg(Motionthread->get_axis_position(0)));
        motion_ui->ui.Y_position_label->setText(QString("Y轴位置：\n%1").arg(Motionthread->get_axis_position(2)));
        motion_ui->ui.Z1_position_label->setText(QString("Z1轴位置：\n%1").arg(Motionthread->get_axis_position(3)));
        motion_ui->ui.Z2_position_label->setText(QString("Z2轴位置：\n%1").arg(Motionthread->get_axis_position(4)));

        });

    //热复位
    connect(motion_ui->ui.reset_pushButton, &QPushButton::clicked, [=]() {
        Motionthread->axis_reset();
        });

    //使能
    connect(motion_ui->ui.enable_pushButton, &QPushButton::clicked, [=]() {
        Motionthread->axis_enable();
        read_axis_inport_timer->start(50);
        });

    //失能
    connect(motion_ui->ui.disable_pushButton, &QPushButton::clicked, [=]() {
        Motionthread->axis_disable();
        read_axis_inport_timer->stop();
        });

    //紧急停止
    connect(ui.stop_pushButton, &QPushButton::clicked, [=]() {
        Motionthread->stop_all_axis();
        Motionthread->axis_stopstatus = true;
        });

    //原点置零
    connect(motion_ui->ui.setzeroposition_pushButton, &QPushButton::clicked, [=]() {
        Motionthread->set_axis_pos_zero();
        });

    //X轴运动
    //正向运动,还有限位标志要设置,向西方向运动(实际反向运动)
    connect(motion_ui->ui.X_D_pushButton, &QPushButton::pressed, [=]() {
        if (!IO_0) {
            if (X_distance == 0) {//连续运动
                Motionthread->X_axis_vmove(X_speed, 0);
            }
            else {//定长运动
                Motionthread->X_axis_pmove(X_speed, (-1) * X_distance);
            }
        }
        });
    connect(motion_ui->ui.X_D_pushButton, &QPushButton::released, [=]() {
        Motionthread->X_axis_stop();
        });
    //反向运动，向东方向运动（实际正向运动）
    connect(motion_ui->ui.X_R_pushButton, &QPushButton::pressed, [=]() {
        if (!IO_1) {
            if (X_distance == 0) {//连续运动
                Motionthread->X_axis_vmove(X_speed, 1);
            }
            else {//定长运动
                Motionthread->X_axis_pmove(X_speed, X_distance);
            }
        }
        });
    connect(motion_ui->ui.X_R_pushButton, &QPushButton::released, [=]() {
        Motionthread->X_axis_stop();
        });


    //Y轴(2号轴)
    //正向运动,向左
    connect(motion_ui->ui.Y_D_pushButton, &QPushButton::pressed, [=]() {
        if (!IO_3) {
            //实际反向dir=0
            if (Y_distance == 0) {//连续运动
                Motionthread->other_axis_vmove(2, Y_speed, 0);
            }
            else {//定长运动
                Motionthread->other_axis_pmove(2, Y_speed, (-1) * Y_distance);
            }
        }
        });
    connect(motion_ui->ui.Y_D_pushButton, &QPushButton::released, [=]() {
        Motionthread->other_axis_stop(2);
        });
    //反向运动，向右
    connect(motion_ui->ui.Y_R_pushButton, &QPushButton::pressed, [=]() {
        if (!IO_2) {
            //实际正向dir=1        
            if (Y_distance == 0) {//连续运动
                Motionthread->other_axis_vmove(2, Y_speed, 1);
            }
            else {//定长运动
                Motionthread->other_axis_pmove(2, Y_speed, Y_distance);
            }
        }
        });
    connect(motion_ui->ui.Y_R_pushButton, &QPushButton::released, [=]() {
        Motionthread->other_axis_stop(2);
        });

    //Z1轴（3号轴）
    //正向运动，向上
    connect(motion_ui->ui.Z1_D_pushButton, &QPushButton::pressed, [=]() {
        if (!IO_4) {
            if (Z1_distance == 0) {//连续运动
                Motionthread->other_axis_vmove(3, Z1_speed, 1);
                Z1_stopstatus = true;//碰到限位停止后，可以再次运动，恢复限位监测状态，需要多点几次运动走出限位区
            }
            else {//定长运动
                Motionthread->other_axis_pmove(3, Z1_speed, Z1_distance);
                Z1_stopstatus = true;
            }
        }
        });
    connect(motion_ui->ui.Z1_D_pushButton, &QPushButton::released, [=]() {
        Motionthread->other_axis_stop(3);
        });
    //反向运动，向下
    connect(motion_ui->ui.Z1_R_pushButton, &QPushButton::pressed, [=]() {
        if (!IO_5) {
            if (Z1_distance == 0) {//连续运动
                Motionthread->other_axis_vmove(3, Z1_speed, 0);
                Z1_stopstatus = true;
            }
            else {//定长运动
                Motionthread->other_axis_pmove(3, Z1_speed, (-1) * Z1_distance);
                Z1_stopstatus = true;
            }
        }
        });
    connect(motion_ui->ui.Z1_R_pushButton, &QPushButton::released, [=]() {
        Motionthread->other_axis_stop(3);
        });

    //Z2轴（4号轴）
    //正向运动，向上
    connect(motion_ui->ui.Z2_D_pushButton, &QPushButton::pressed, [=]() {
        if (!IO_6) {
            if (Z2_distance == 0) {//连续运动
                Motionthread->other_axis_vmove(4, Z2_speed, 1);
            }
            else {//定长运动
                Motionthread->other_axis_pmove(4, Z2_speed, Z2_distance);
            }
        }
        });
    connect(motion_ui->ui.Z2_D_pushButton, &QPushButton::released, [=]() {
        Motionthread->other_axis_stop(4);
        });
    //反向运动，向下
    connect(motion_ui->ui.Z2_R_pushButton, &QPushButton::pressed, [=]() {
        if (!IO_7) {
            if (Z2_distance == 0) {//连续运动
                Motionthread->other_axis_vmove(4, Z2_speed, 0);
            }
            else {//定长运动
                Motionthread->other_axis_pmove(4, Z2_speed, (-1) * Z2_distance);
            }
        }
        });
    connect(motion_ui->ui.Z2_R_pushButton, &QPushButton::released, [=]() {
        Motionthread->other_axis_stop(4);
        });

    //临时，读取舵机位置定时器
    read_servo_timer = new QTimer(this);

    connect(read_servo_timer, &QTimer::timeout, [=]() {
        switch (scanmodel_SLR)
        {
        case 1:
            motion_ui->ui.S_angle_label->setText(QString("实际位置：%1").arg(Servo_model->readToclient(1)));
            break;
        case 2:
            motion_ui->ui.L_angle_label->setText(QString("实际位置：%1").arg(Servo_model->readToclient(2)));
            break;
        case 3:
            motion_ui->ui.R_angle_label->setText(QString("实际位置：%1").arg(Servo_model->readToclient(3)));
            break;
        }
        });

    //连接舵机
    connect(motion_ui->ui.connect_duoji_pushButton, &QPushButton::clicked, [=]() {
        Servo_model->connectToservo();
        read_servo_timer->start(50);
        //连接舵机后，设定三个舵机的初始值,出现了无法设定初始值的问题
        motion_ui->ui.S_duoji_horizontalSlider->setValue(Servo_model->readToclient(1) * (4095.0 / 360.0));
        motion_ui->ui.L_duoji_horizontalSlider->setValue(Servo_model->readToclient(2) * (4095.0 / 360.0));
        motion_ui->ui.R_duoji_horizontalSlider->setValue(Servo_model->readToclient(3) * (4095.0 / 360.0));

        });
    //断开舵机连接
    connect(motion_ui->ui.disconnect_duoji_pushButton, &QPushButton::clicked, [=]() {
        Servo_model->disconnectToservo();
        read_servo_timer->stop();
        });

    //舵机移动,如果需要移动的舵机与当前扫查模式相匹配，则可以移动舵机
    connect(motion_ui, &Motion_ui::writeToclient, [=](unsigned int regStartAddr, unsigned int number, unsigned int serverAddress) {
        if (serverAddress == scanmodel_SLR)
            Servo_model->writeToclient(regStartAddr, number, serverAddress);
        });

}

//读取限位状态与设置停止标志，伺服使能时启动，伺服失能时关闭
void MainWindow::read_axis_inport()
{
    IO_0 = dmc_read_inbit(0, 0);
    IO_1 = dmc_read_inbit(0, 1);
    IO_2 = dmc_read_inbit(0, 2);
    IO_3 = dmc_read_inbit(0, 3);
    IO_4 = dmc_read_inbit(0, 4);
    IO_5 = dmc_read_inbit(0, 5);
    IO_6 = dmc_read_inbit(0, 6);
    IO_7 = dmc_read_inbit(0, 7);

    //qDebug() << "Y左限位情况：" << IO_3;
    if (IO_0 || IO_1)
    {
        Motionthread->X_axis_stop();
        //停止标志设置
        Motionthread->axis_stopstatus = true;
        X_stopstatus = false;
    }

    if (IO_2 || IO_3)
    {
        Motionthread->other_axis_stop(2);
        //停止标志设置
        Motionthread->axis_stopstatus = true;
        Y_stopstatus = false;
    }

    if (IO_4 || IO_5)
    {
        if (Z1_stopstatus)
        {
            Motionthread->other_axis_stop(3);
            Motionthread->axis_stopstatus = true;
            Z1_stopstatus = false;//停止一次后，
        }
        //停止标志设置
        Motionthread->axis_stopstatus = true;
    }

    if (IO_6 || IO_7)
    {
        Motionthread->other_axis_stop(4);
        //停止标志设置
        Motionthread->axis_stopstatus = true;
        Z2_stopstatus = false;
    }
}

//实时A扫
void MainWindow::showLiveAscan()
{
    if (AscanData.isEmpty())//AscanData为400个数据为一个单位
        return;
    // 删除之前的 Series 对象
    if (chart->series().count() > 0)
    {
        QAbstractSeries* oldSeries = chart->series().at(0);//将上一个QSplineSeries对象的地址传递给oldSeries
        chart->removeSeries(oldSeries);
        delete oldSeries;
    }
    QSplineSeries* Series = new QSplineSeries;
    for (int i = ys; i < 200 + ys; i++)
    {
        Series->append(i, AscanData[i + LiveAscandex * 400]);
    }
    chart->addSeries(Series);//先添加曲线，再给曲线绑定坐标
    Series->attachAxis(Xaixs);//先绑定坐标的话将会提示曲线不在QChart中
    Series->attachAxis(Yaixs);
}

//延时，闸门改变时，相应的操作
void MainWindow::ys_door_changed(int ui_ys, int ui_top_door, int ui_bot_door, int ui_left_door, int ui_right_door)
{
    ys = ui_ys;
    top_door = ui_top_door;
    bot_door = ui_bot_door;
    left_door = ui_left_door;
    right_door = ui_right_door;
    Xaixs->setRange(ys, 200 + ys);//坐标范围进行相应改变
    //改变闸门框大小
    BScanDoorPixmap.fill(Qt::transparent);
    BScanDoor.setRect(left_door, top_door, right_door - left_door, bot_door - top_door);
    BScanPainter.begin(&BScanDoorPixmap);//设置绘制的对象
    BScanPainter.setPen(Qt::red);
    BScanPainter.drawRect(BScanDoor);
    BScanPainter.end();
    //数据读取时，相应A,B图像进行刷新
    if (data_status)
    {
        showAWave();//当延时改变时，刷新A扫
        showBImage();
    }
}

//查看C扫结束后的C扫图像（可以用滚轮前后移动查看）
void MainWindow::showCImage_usingwheel(int value)
{
    //时基扫描时，有一个TimeCImagedex,当图片的长的超过500时，这个dex就是所截取图片的起始x值
    if (value > 0)//滚轮正转，图片向右移动
    {
        //showCImagedex截取图片的起始x值，这个值必须小于整个图片结尾x值-499，否则，将内存访问冲突
        if (showCImagedex < CImagelength - ui.C_Scan_label->width())//Dataprocessthread->Cscandex+1=整个C扫图像的长度
        {
            QImage copyImage = CImage.copy(showCImagedex + 1, 0, ui.C_Scan_label->width(), CImage.height());
            CScaledImage = copyImage.scaled(ui.C_Scan_label->width(), ui.C_Scan_label->height());
            ui.C_Scan_label->setPixmap(QPixmap::fromImage(CScaledImage));
            showCImagedex = showCImagedex + 1;
        }
    }
    else//滚轮反转，图片向左移动
    {
        if (showCImagedex > 0)
        {
            QImage copyImage = CImage.copy(showCImagedex - 1, 0, ui.C_Scan_label->width(), CImage.height());
            CScaledImage = copyImage.scaled(ui.C_Scan_label->width(), ui.C_Scan_label->height());
            ui.C_Scan_label->setPixmap(QPixmap::fromImage(CScaledImage));
            showCImagedex = showCImagedex - 1;
        }
    }
}

//显示保存进度
void MainWindow::show_savedata_progress(int percentage)
{
    //// 在状态栏中显示保存进度
    statusBar()->showMessage("数据保存进度: " + QString::number(percentage) + "%", 5000);
    //QProgressBar* existingProgressBar = findChild<QProgressBar*>("saveProgressBar");
    //if (existingProgressBar) {
    //    existingProgressBar->setValue(percentage);  // 更新现有的进度条数值
    //}
    //else {
    //    // 创建一个新的进度条
    //    QProgressBar* progressBar = new QProgressBar();
    //    progressBar->setObjectName("saveProgressBar");
    //    progressBar->setRange(0, 100);
    //    progressBar->setValue(percentage);

    //    // 定义一个容器来包裹文本和进度条
    //    QWidget* container = new QWidget();
    //    QHBoxLayout* layout = new QHBoxLayout(container);
    //    layout->addWidget(new QLabel("数据保存进度: "));  // 添加文本标签
    //    layout->addWidget(progressBar);  // 添加进度条
    //    layout->setContentsMargins(10, 0, 0, 0);  // 设置边距为0

    //    // 在状态栏中添加新的进度条 - 在最左侧插入
    //    statusBar()->insertWidget(0, container);

    //    if (percentage == 100)
    //    {
    //        QTimer::singleShot(2000, this, [=]() {
    //            /*QStatusBar* bar = statusBar();
    //            QList<QWidget*> widgets = bar->findChildren<QWidget*>();
    //            for (QWidget* widget : widgets) {
    //                bar->removeWidget(widget);
    //                delete widget;*/
    //            //statusBar()->removeAction();
    //    });
    //}
    //}
}

//读取数据
void MainWindow::read_data()
{
    filePath = QFileDialog::getOpenFileName(nullptr, "Save Binary File", QDir::currentPath() + "/扫查数据", "Binary Files (*.bin)");
    if (filePath.isEmpty())
    {
        qDebug() << "User canceled the operation.";
        return;
    }

    B_LabelMouseStatus = true;
    C_LabelMouseStatus = true;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open temp file for reading:" << file.errorString();
        file.close();
        return;
    }

    QDataStream in(&file);
    in >> data_Clength;
    in >> data_signal_num;
    in >> top_door;
    in >> bot_door;
    in >> left_door;
    in >> right_door;
    in >> ys;
    file.close();

    CImagelength = data_Clength;//相当于整幅C扫图像的长度 

    //将相关Slider的值设置
    parameter_ui->ui.top_horizontalScrollBar->setValue(top_door);
    parameter_ui->ui.bot_horizontalScrollBar->setValue(bot_door);
    parameter_ui->ui.left_horizontalScrollBar->setValue(left_door);
    parameter_ui->ui.right_horizontalScrollBar->setValue(right_door);
    parameter_ui->ui.ys_horizontalScrollBar->setValue(ys);
    //重新设置series中的x坐标
    Xaixs->setRange(ys, 200 + ys);

    showCImagedex = 0;//显示C扫图片的起始坐标

    showCImage();//先将读出所有数据，存成一张CImage;
    showBImage();
    showAWave();
}

//读取二进制数据，并显示其A扫波形
void MainWindow::showAWave()
{
    //第几组B扫数据
    int dex = showCImagedex + data_BScandex;//2是放大倍数
    QVector<int>data;
    qint64 startPosition = (7 + dex * data_signal_num * 400 + data_AWavedex * 400) * sizeof(int);//直接将全波列数据取出，400个数据
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        if (!file.seek(startPosition))
        {
            qDebug() << "Failed to seek to the start position.";
            file.close();
            return;
        }
        QDataStream in(&file);
        int i = 0;
        while (i < 400)
        {
            int value;
            in >> value; // 读取一个整数
            data.append(value);
            i++;
        }
    }
    file.close();
    //显示A波
    if (chart->series().count() > 0)
    {
        QAbstractSeries* oldSeries = chart->series().at(0);
        chart->removeSeries(oldSeries);
        delete oldSeries;
    }
    QSplineSeries* Series = new QSplineSeries;
    for (int i = ys; i < 200 + ys; i++)
    {
        Series->append(i, data[i]);
    }
    chart->addSeries(Series);
    Series->attachAxis(Xaixs);
    Series->attachAxis(Yaixs);
}

//读取二进制数据，并将其B扫成像
void MainWindow::showBImage()
{
    QVector<int>BScanData;
    //第几组B扫数据
    int dex = showCImagedex + data_BScandex;
    QImage Bimage((data_signal_num + (data_signal_num - 1) * 9), 200, QImage::Format_RGB32);//取200个点
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        for (int x = 0; x < data_signal_num; x++)
        {
            QVector<int>data;
            qint64 startPosition = (7 + dex * data_signal_num * 400 + x * 400) * sizeof(int);//直接将全波列数据取出，400个数据
            if (!file.seek(startPosition))
            {
                qDebug() << "Failed to seek to the start position.";
                file.close();
                return;
            }
            QDataStream in(&file);
            int i = 0;
            while (i < 400)//禁用ys延时功能，将全部点读出来
            {
                int value;
                in >> value; // 读取一个整数
                data.append(value);
                i++;
            }
            for (int y = 0; y < 200; y++)
            {
                int value = data[y + ys];//开启延时功能
                BScanData.append(value);
                Bimage.setPixelColor(x * 10, y, Dataprocessthread->valuetocolor(value));
            }
        }
    }
    file.close();

    //添加插值点
    for (int i = 0; i < data_signal_num - 1; i++)
    {
        for (int x = 1; x < 10; x++)
        {
            for (int y = 0; y < 200; y++)
            {
                int value = BScanData[y + 200 * i] * (10 - x) / 10 + BScanData[y + 200 * i + 200] * x / 10;//int型能够四舍五入
                Bimage.setPixelColor(x + i * 10, y, Dataprocessthread->valuetocolor(value));
            }
        }
    }

    //BScanDoorPixmap.fill(Qt::transparent);
    //BScanDoor.setRect(left_door, top_door, right_door - left_door, bot_door - top_door);
    //BScanPainter.begin(&BScanDoorPixmap);//设置绘制的对象
    //BScanPainter.setPen(Qt::red);
    //BScanPainter.drawRect(BScanDoor);
    //BScanPainter.end();
    // 将临时的QPixmap叠加到BscaledImage上   //图像缩放
    QImage BscaledImage = Bimage.scaled(ui.B_Scan_label->width(), ui.B_Scan_label->height());
    QPixmap scaledImagePixmap = QPixmap::fromImage(BscaledImage);
    QPainter combinedPainter(&scaledImagePixmap);
    combinedPainter.drawPixmap(0, 0, BScanDoorPixmap);
    // 在QLabel上显示叠加后的图片
    ui.B_Scan_label->setPixmap(scaledImagePixmap);
    qDebug() << "调用了showdatabscan";

}

//读取二进制数据，并将其C扫成像
void MainWindow::showCImage()
{
    data_CscanStartdex = left_door / (ui.B_Scan_label->width() / data_signal_num);
    data_CscanStopdex = right_door / (ui.B_Scan_label->width() / data_signal_num);
    data_CImageHeight = data_CscanStopdex - data_CscanStartdex;//根据闸门的左右宽度，算出有几条数据进行C扫成像，原来是+1的
    //考虑延时，C扫图像，每个点的起始点，共400个点，从第100个点开始选择最大值成像
    data_startdex = top_door / (ui.B_Scan_label->height() / 200) + ys;
    int data_signallength = bot_door / (ui.B_Scan_label->height() / 200) - top_door / (ui.B_Scan_label->height() / 200);//C扫成像范围宽度

    CImage = QImage(data_Clength, data_CImageHeight, QImage::Format_RGB32);
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {

        for (int x = 0; x < data_Clength; x++)
        {
            for (int y = 0; y < data_CImageHeight; y++)
            {
                QVector<int>data;
                qint64 startPosition = (7 + x * data_signal_num * 400 + (y + data_CscanStartdex) * 400 + (data_startdex - 1)) * sizeof(int);//取数据中第40~80位进行成像
                if (!file.seek(startPosition))
                {
                    qDebug() << "Failed to seek to the start position.";
                    file.close();
                    return;
                }
                // 读取数据
                QDataStream in(&file);
                int i = 0;
                while (i < data_signallength)
                {
                    int value;
                    in >> value; // 读取一个整数
                    data.append(value);
                    i++;
                }
                //获取最大值存入图像
                int  value = *std::max_element(data.constBegin(), data.constEnd());
                CImage.setPixelColor(x, y, Dataprocessthread->valuetocolor(value));
            }
        }
    }
    file.close();
    //读取完图像，进行显示
    if (data_Clength > ui.C_Scan_label->width())
    {
        QImage copyImage = CImage.copy(0, 0, ui.C_Scan_label->width(), CImage.height());
        CScaledImage = copyImage.scaled(ui.C_Scan_label->width(), ui.C_Scan_label->height());
        ui.C_Scan_label->setPixmap(QPixmap::fromImage(CScaledImage));//强制类型转换
    }
    else
    {
        QImage copyImage = CImage.copy(0, 0, CImage.width(), CImage.height());
        CScaledImage = copyImage.scaled(copyImage.width(), ui.C_Scan_label->height());
        ui.C_Scan_label->setPixmap(QPixmap::fromImage(CScaledImage));
    }
}

MainWindow::~MainWindow()
{}
