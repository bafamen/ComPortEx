/*******************************************************************************
TODO List
 ******************************************************************************/
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>

/*******************************************************************************
 * @fn isDarkMode
 * @brief   проверка цветовой темы приложения. для отладки
 * @param   none
 * @return  1-цветовой темы темная
 ******************************************************************************/
#include <QStyleHints>
inline bool isDarkMode() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    return scheme == Qt::ColorScheme::Dark;
#else
    const QPalette defaultPalette;
    const auto text = defaultPalette.color(QPalette::WindowText);
    const auto window = defaultPalette.color(QPalette::Window);
    return text.lightness() > window.lightness();
#endif // QT_VERSION
}

/*******************************************************************************
 * @fn on_dbgBtn_clicked
 * @brief   отладка
 * @return  none
 ******************************************************************************/
void MainWindow::on_dbgBtn_clicked()
{
    //qDebug("Form QMenu");
    qDebug() << "Debug Button";
    msgb.get()->setText("Test");
    msgb.get()->show();
}

/*******************************************************************************
 * @fn MainWindow
 * @brief   деструктор
 * @param   none
 * @return  none
 ******************************************************************************/
MainWindow::~MainWindow()
{
    QStringList lst;
    int count=SendWindow->QWKtemplate->rowCount();
    for(int index=0;index<count; index++){
        QTableWidgetItem *itm=SendWindow->QWKtemplate->item(index,0);
        if(itm==NULL) break;
        lst+=itm->text();
    }// for
    sett_cntl.SaveTemplateS(lst); // сохраним шаблоны в файл

    // освобождение памяти, занимаемое динамическими обьектами
    foreach (QAction *action, popmenu2->actions()) {
        delete action;
    }
    delete popmenu2; // всплывающие меню для нижнего меню

    // элементы нижнего меню
    int num=status_lay.get()->count()-1;
    for(auto cnt=num; cnt>=0; cnt--){
        QWidget *label = status_lay.get()->itemAt(cnt)->widget();
        delete status_lay.get()->itemAt(cnt)->widget();
    }

    delete cport;
    delete ui;
}

/*******************************************************************************
 * @fn MainWindow
 * @brief   конструктор
 * @param   none
 * @return  none
 ******************************************************************************/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SendWindow = new sendw(); // инициализируем окно шаблонов запросов

    // настройка таблиц отображения данных посылаемых
    QStringList THeadrL={"#","Time","Data"};
    ui->TSendW->setHorizontalHeaderLabels(THeadrL);
    ui->TSendW->horizontalHeader()->resizeSection(0,5);     // авто размер секций
    ui->TSendW->horizontalHeader()->resizeSection(1,150);
    //ui->TSendW->horizontalHeader()->resizeSection(2,250);  // заменил на
    ui->TSendW->horizontalHeader()->setStretchLastSection(true);
    ui->TSendW->verticalHeader()->setVisible(false); // Hide vertical header aka the line counter
    ui->TSendW->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

    ui->TSendW->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TSendW->setSelectionMode(QAbstractItemView::ContiguousSelection);

    // настроим pop menu для разных областей
    ui->TSendW->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->TSendW,SIGNAL(customContextMenuRequested(const QPoint &)),SLOT(STableWork_ConMenu(const QPoint &))); // сиuнал для раб зоны таблицы
    ui->TSendW->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->TSendW->horizontalHeader(),SIGNAL(customContextMenuRequested(const QPoint &)),SLOT(STableHead_ConMenu(const QPoint &))); // сиuнал для хедера

    // настройка таблиц отображения данных принимаемых
    ui->TRecW->setHorizontalHeaderLabels(THeadrL);
    ui->TRecW->horizontalHeader()->resizeSection(0,5);     // авто размер секций
    ui->TRecW->horizontalHeader()->resizeSection(1,150);
    ui->TRecW->horizontalHeader()->setStretchLastSection(true);
    ui->TRecW->verticalHeader()->setVisible(false); // Hide vertical header aka the line counter
    ui->TRecW->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

    ui->TRecW->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TRecW->setSelectionMode(QAbstractItemView::ContiguousSelection);

    // настроим pop menu
    ui->TRecW->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->TRecW,SIGNAL(customContextMenuRequested(const QPoint &)),SLOT(STableWork_ConMenu(const QPoint &))); // сиuнал для раб зоны таблицы
    ui->TRecW->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->TRecW->horizontalHeader(),SIGNAL(customContextMenuRequested(const QPoint &)),SLOT(STableHead_ConMenu(const QPoint &))); // сиuнал для хедера
    //ui->TRecW->setColumnHidden(0,true);

    cport = new ComPort(this,sett_cntl);

    popmenu2 = new QMenu(); // создаем обьект который будет отвечать за динамически создаваемые всплывающие меню
    // создаем бокс для сообщений
    this->msgb = std::make_unique<QMessageBox>();
    QObject::connect(this,&MainWindow::SigShowMessage,this,&MainWindow::SShowMessage); // сиuнал для вывода сообщений

    // обработка сигнала ком порта о готовности данных
    QObject::connect(cport,&ComPort::readDone,this,&MainWindow::SRXdataToUI);

    // кнопки посылки шаблонных сообщений
    QObject::connect(ui->PBSend1,&QPushButton::released,this,&MainWindow::SPBSend_clicked);
    QObject::connect(ui->PBSend2,&QPushButton::released,this,&MainWindow::SPBSend_clicked);
    QObject::connect(ui->PBSend3,&QPushButton::released,this,&MainWindow::SPBSend_clicked);
    QObject::connect(ui->PBSendN,&QPushButton::released,this,&MainWindow::SPBSend_clicked);

    // вызвать всплаывающее меню для окна sendw
    SendWindow->QWKtemplate->setContextMenuPolicy(Qt::CustomContextMenu);
    qDebug() << QObject::connect(SendWindow->QWKtemplate,&QWidget::customContextMenuRequested,this,&MainWindow::SendW_ConMenu);
    MakeDownMenu();

    if(cport->getSParByIdx(Settings::SPortName)!="") this->SCPcmd(); // запустим порт
    qDebug() << "MainWindow " << QThread::currentThreadId();
    //isDarkMode(); // debug

    // считаем шаблоны из файлов
    QStringList lst=sett_cntl.LoadTemplateS();
    for(int index=0;index<lst.count(); index++){
        SendWindow->QWKtemplate->setRowCount(index+1);
        SendWindow->QWKtemplate->setItem(index,0,new QTableWidgetItem(lst.at(index))); // row col
    }// for
}

/*******************************************************************************
 * @fn MakeDownMenu
 * @brief   создаем нижнее меню для управления настройками порта
 *
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::MakeDownMenu()
{
    this->status_wlay = std::make_unique<QWidget>();
    this->status_lay = std::make_unique<QHBoxLayout>(status_wlay.get());

    for(int count=0; count<NUM_DOWN_MENU; count++){
        QString str_init=cport->getSParByIdx(count); // получаем значения из структуры настройки порта и инициализируем этим лейблы
        QClickableLabel *lmenu = new QClickableLabel(this->dmenu_init[count]+str_init);
        lmenu->setObjectName(this->dmenu_names[count]);
        QObject::connect(lmenu,&QClickableLabel::clicked,this,&MainWindow::SPopMenu); // сиuнал изменения порта
        if(count==0)
            status_lay->addWidget(lmenu,1,Qt::AlignRight); // 1-виджет не будет растягиваться, выравнивает виджет по правому краю
        else
            status_lay->addWidget(lmenu);

        ui->statusbar->addWidget(status_wlay.get(),1);
    }
}

/*******************************************************************************
 * @fn SPopMenu  (slot popup menu)
 * @brief   вызывается контектсное меню для лейбла настройки порта
 *
 * @param   pos - позиция, где должно быть вызвано меню
 * @return  none
 ******************************************************************************/
void MainWindow::SPopMenu(QPoint pos)
{
    QClickableLabel *label = qobject_cast<QClickableLabel *>(sender());
    QStringList plist;
    cp_params par=cport->getPrepar();
    QString checked_par;

    if(label->objectName()==dmenu_names[LPORT]){
        plist = cport->searchComPorts(); // ищем все порты для добавления в меню
        checked_par=par.m_portName;
    }
    else if(label->objectName()==dmenu_names[LSPEED]){
        std::list<QSerialPort::BaudRate> ilist = cport->LstSpeed();
        foreach(qint32 speed , ilist )
            plist.append(QString::number(speed));
        checked_par=QString::number(par.m_baudrate);
    }
    else if(label->objectName()==dmenu_names[LNUMBITS]){
        std::list<QSerialPort::DataBits> ilist = cport->LstBits();
        foreach(qint32 speed , ilist )
            plist.append(QString::number(speed));
        checked_par=QString::number(par.m_dataBits);
    }
    else if(label->objectName()==dmenu_names[LPARITY]){
        std::list<QSerialPort::Parity> ilist = cport->LstParity();
        foreach(qint32 speed , ilist )
            plist.append(ComPort::iparityToString(speed));
        checked_par=ComPort::iparityToString(par.m_parity);
    }
    else if(label->objectName()==dmenu_names[LSTOPBIT]){
        std::list<QSerialPort::StopBits> ilist = cport->LstSBits();
        foreach(qint32 speed , ilist )
            plist.append(QString::number(speed));
        checked_par=QString::number(par.m_stopBits);
    }
    // удалим старые пункты
    foreach (QAction *action, popmenu2->actions()) {
        delete action;
    }
    // пересоздадим пункты меню
    for(const QString &pname : static_cast<const QStringList>(plist)){
        QAction *action = new QAction(pname,label);
        action->setObjectName(pname);
        action->setCheckable(true);
        if(pname==checked_par)
            action->setChecked(true);   // если этот порт выбран
        connect(action, &QAction::triggered, this, &MainWindow::SPMPortChange);
        popmenu2->addAction(action);
    }

    /* Вызываем контекстное меню */
    popmenu2->popup(label->parentWidget()->mapToGlobal(pos));
}

/*******************************************************************************
 * @fn SPMPortChange  (slot port change)
 * @brief   слот об изменении порта или скорости
 *          привязано к пунктам динамического меню
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::SPMPortChange()
{
    QAction *action = qobject_cast<QAction *>(sender());
    cp_params par=cport->getPrepar();     // настройки порта берем из класса
    if(action->parent()->objectName()==this->dmenu_names[LPORT]){
        par.m_portName=action->text();
        qobject_cast<QClickableLabel *>(action->parent())->setText("port: "+par.m_portName);
        sett_cntl.SavePortS(par.m_portName,Settings::SPortName);
    }
    else if(action->parent()->objectName()==this->dmenu_names[LSPEED]){
        par.m_baudrate=action->text().toInt();
        qobject_cast<QClickableLabel *>(action->parent())->setText("baud: "+action->text());
        sett_cntl.SavePortS(action->text(),Settings::SBaudRate);
    }
    else if(action->parent()->objectName()==this->dmenu_names[LNUMBITS]){
        par.m_dataBits=action->text().toInt();
        qobject_cast<QClickableLabel *>(action->parent())->setText("bits: "+action->text());
        sett_cntl.SavePortS(action->text(),Settings::SDatabits);
    }
    else if(action->parent()->objectName()==this->dmenu_names[LPARITY]){
        par.m_parity=ComPort::stringToParity(action->text());
        qobject_cast<QClickableLabel *>(action->parent())->setText("parity: "+action->text());
        sett_cntl.SavePortS(action->text(),Settings::SParity);
    }
    else if(action->parent()->objectName()==this->dmenu_names[LSTOPBIT]){
        par.m_stopBits=action->text().toInt();
        qobject_cast<QClickableLabel *>(action->parent())->setText("stop bits: "+action->text());
        sett_cntl.SavePortS(action->text(),Settings::SStopBits);
    }
    cport->setPrepar(par);
    SCPcmd(); // функция слот для изменения настроек компорта.
}

/*******************************************************************************
 * @fn on_actionPort_hovered (auto slot)
 * @brief   обработчик actionPort = QAction по выбору в меню
 *          формирует список портов для верхнего меню actionPort
 *
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::on_actionPort_hovered()
{
    QStringList plist= cport->searchComPorts(); // ищем все порты для добавления в меню
    QString checked_par=cport->getPrepar().m_portName;
    QMenu *menu = new QMenu(); // меню порта

    QWidget *label = status_lay.get()->itemAt(0)->widget();
    for(const QString &pname : static_cast<const QStringList>(plist)){
        QAction *action = new QAction(pname,label); // label==(QClickableLabel)lport нужен для обработчика
        action->setObjectName(pname);
        action->setCheckable(true);
        if(pname==checked_par)
            action->setChecked(true);   // если этот порт выбран
        connect(action, &QAction::triggered, this, &MainWindow::SPMPortChange);
        menu->addAction(action);
    }
    ui->actionPort->setMenu(menu);
}

/*******************************************************************************
 * @fn SRXdataToUI  (slot readDone)
 * @brief   слот обработка данных полученных от ком порта
 * @param   массив данных ком порта
 * @return  none
 ******************************************************************************/
void MainWindow::SRXdataToUI(const QByteArray &result)
{
    QByteArray data=result;
    QString str="";
    foreach(const uchar &sym , data){
        QString chr=QString::number(sym,16);
        if(chr.size()<2) chr="0"+chr;
        str+=" "+chr;
    }

    int cnt=ui->TRecW->rowCount(); // начинается с 0
    ui->TRecW->setRowCount(cnt+1); // увеличим счетчик строк
    ui->TRecW->setItem(cnt,0,new QTableWidgetItem(QString::number(cnt))); // индекс
    ui->TRecW->setItem(cnt,1,new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz "))); // время
    ui->TRecW->setItem(cnt,2,new QTableWidgetItem(str)); // данные
}

/*******************************************************************************
 * @fn SCPcmd (slot com port command)
 * @brief   функция слот для изменения настроек компорта.
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::SCPcmd()
{
    qDebug() << "SCPcmd " << QThread::currentThreadId();
    cp_params par=cport->getPrepar();
    ComPort::ErrType port_err=QSerialPort::NoError;
    port_err=cport->SetPort(par);
    // если порт не удалось установить, например он занят
    if(port_err){
        QWidget *label = status_lay.get()->itemAt(0)->widget();
        qobject_cast<QClickableLabel *>(label)->setText("port: --");

        QString str=ComPort::serialPortErrorToString(port_err);
        cp_params par=cport->getPrepar();     // настройки порта берем из класса
        par.m_portName="";
        cport->setPrepar(par);
        qDebug() << str << "port_err=" << port_err;
        emit SigShowMessage(str); // пошлем сигнал на вывод сообщения в messagebox.
    }
}

/*******************************************************************************
 * @fn SShowMessage (slot msgb)
 * @brief   вывод сообщения на бокс
 * @param  сообщение
 * @return none
 ******************************************************************************/
void MainWindow::SShowMessage(const QString &str){
    msgb.get()->setText(str);
    msgb.get()->show();
}

/*******************************************************************************
 * @fn on_PBClearRec_clicked (auto)
 * @brief   чистим таблицу принятых пакетов
 * @param  none
 * @return  none
 ******************************************************************************/
void MainWindow::on_PBClearRec_clicked()
{
    ui->TRecW->clear(); // чистит содержимое но не сбрасывает счетчик строк
    ui->TRecW->setRowCount(0);
    QStringList THeadrL={"#","Time","Data"};
    ui->TRecW->setHorizontalHeaderLabels(THeadrL);
}

/*******************************************************************************
 * @fn on_PBClearRec_clicked (auto)
 * @brief   чистим таблицу отправленных пакетов
 * @param  none
 * @return  none
 ******************************************************************************/
void MainWindow::on_PBClearSend_clicked()
{
    ui->TSendW->clear();
    ui->TRecW->setRowCount(0);
    QStringList THeadrL={"#","Time","Data"};
    ui->TSendW->setHorizontalHeaderLabels(THeadrL);
}

/*******************************************************************************
 * @fn on_PBStopRec_clicked (auto)
 * @brief   управление портом, вкл - откл. при помощи кнопки, быстро
 * @param  none
 * @return  none
 ******************************************************************************/
void MainWindow::on_PBStopRec_clicked()
{
    if(ui->PBStopRec->text()=="Stop"){
        cport->ClosePort();
        ui->PBStopRec->setText("Run");
    }
    else{
        SCPcmd(); // запустим порт
        ui->PBStopRec->setText("Stop");
    }
}

/*******************************************************************************
 * @fn on_actionST_triggered (auto slot)
 * @brief   обработчик подменю Settings template.
 *          по нажатию открывает окно настройки шаблонов запросов ком порта
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::on_actionST_triggered()
{
    SendWindow->show();
}

/*******************************************************************************
 * @fn SPBSend_clicked (slot)
 * @brief обработчик кнопок посылки шаблонов запросов
 * кнопок у нас 1,2,3 - посылают 1 2 3 последовательность шаблонов
 * * - будет посылать последовательность из шаблонов отмеченный *.
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::SPBSend_clicked()
{
    int index=0;
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    //if(button->objectName()=="PBSend1") index=0; // первый столбец автоматический, поэтому 1й имеет координаты 0,0
    if(button->objectName()=="PBSend2") index=1;
    if(button->objectName()=="PBSend3") index=2;
    if(button->objectName()=="PBSendN"){  // найдем строку с символом *
        int count=SendWindow->QWKtemplate->rowCount();
        // по всем шаблонам пройдем и удалим * в другом
        bool found=false;
        for(;index<count; index++){
            QTableWidgetItem *itm=SendWindow->QWKtemplate->item(index,0);
            if(itm==NULL) break;
            QChar ch=itm->text().front();
            if(ch=='*') {found=true; break;}
        }// for
        if(found==false) // маркер не обнаружен, не передаем
            return;
    }

    QTableWidgetItem *item=SendWindow->QWKtemplate->item(index,0);
    if(item==NULL) return;
    QString str=item->text();

    QByteArray data;
    // компоновка последовательности из заданной строки например 33 4D
    index=0;
    char ch_out;
    foreach(QChar ch, str){
        char cdeb;
        if((ch!=' ')&&(ch!='*')){
            cdeb=ch.unicode();
            index++;
        }
        switch(index){
            case 1:
                ch_out=(ch.toLatin1()<<4);  // первая половина (ex 4)
            break;
            case 2:
                cdeb=ch.toLatin1();
                ch_out|=(ch.toLatin1()&0x04);      // вторая половина (ex D)
                data.append(ch_out);
            default: index=0;
        }
    }

    index=cport->SendData(data);
    if(index<=0) return; // что то пошло не так, и передача не прошла
    // отобразим посланные данные в правом окне
    int cnt=ui->TSendW->rowCount(); // начинаетсся с 0
    ui->TSendW->setRowCount(cnt+1); // увеличим счетчик строк
    ui->TSendW->setItem(cnt,0,new QTableWidgetItem(QString::number(cnt))); // индекс
    ui->TSendW->setItem(cnt,1,new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz "))); // время
    ui->TSendW->setItem(cnt,2,new QTableWidgetItem(str)); // данные
}


