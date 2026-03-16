#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include ".\Sources\comport.h"
#include ".\Sources\qclickablelabel.h"
#include ".\Sources\settings.h"
#include ".\Windows\sendw.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QTableWidget>
#include <QApplication>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{    
    Q_OBJECT
enum E_CMD_THRD{NO_CMD,CHANGE_PORT,CHANGE_SPEED,CHANGE_BITS,CHANGE_PARITY,CHANGE_SBITS};
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    sendw *SendWindow;   // окно шаблонов запросов

    ComPort *cport;            // обьект порта
    Settings sett_cntl;        // обьект работы с настройками

    // динамические обьекты
    std::unique_ptr<QWidget> status_wlay;       // виджет лайота для хранения леблов с статусе
    std::unique_ptr<QHBoxLayout> status_lay;    // лайота для хранения леблов с статусе

    enum{LPORT,LSPEED,LNUMBITS,LPARITY,LSTOPBIT,NUM_DOWN_MENU}; // индексы подпунктов нижнего меню
    const QStringList dmenu_names = {"lport","lspeed","lnumbits","lparity","lstopbit"};  // имя динамических элементов нижнего меню. создаются динамически
    //const QStringList dmenu_init = {"port: --","baud: --","bits: --","parity: --","stop bits: --"}; // отображаемые значения меню при инициализации
    const QStringList dmenu_init = {"port: ","baud: ","bits: ","parity: ","stop bits: "};
    QMenu *popmenu2;                           // будет отвечать за динамически создаваемые всплывающее меню

    std::unique_ptr<QMessageBox> msgb;         // messagebox используемый для вывода сообщений
    void SaveTable(void);
    void ProcessColomn(void);
    void ProcessSendW(void);
    void closeEvent(QCloseEvent *ev){ QApplication::quit();} // ovveride closeEvent, без этого не закрывались модальные окна
signals:
    void SigShowMessage(const QString &str);

private slots:
    void MakeDownMenu();
    void SPopMenu(QPoint pos);          // вызов меню
    void SPMPortChange();               // обработка меню

    void SRXdataToUI(const QByteArray &result);
    void SShowMessage(const QString &str);
    void SCPcmd();  // слот обработки изменения ком порта
    void STableWork_ConMenu(const QPoint &pos);
    void STableHead_ConMenu(const QPoint &pos);
    void SendW_ConMenu(const QPoint &pos);
private slots:
    void on_dbgBtn_clicked();
    void on_PBClearRec_clicked();
    void on_PBClearSend_clicked();
    void on_PBStopRec_clicked();
    void on_actionST_triggered();
    void SPBSend_clicked(); // общая для 4х кнопок
    void on_actionPort_hovered();
};
#endif // MAINWINDOW_H

