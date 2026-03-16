/*******************************************************************************
 * @brief ресурсы
 * https://habr.com/ru/articles/246445/  Qt + MVP + QThread.
 * https://doc.qt.io/qt-6/qserialport.html
 * https://doc.qt.io/qt-6/qthread.html
 * https://zhitenev.ru/about-threading-in-qt-and-how-to-create-a-thread/  пример использования bind
 ******************************************************************************/
#ifndef COMPORT_H
#define COMPORT_H

#include <QtSerialPort/QtSerialPort>
#include "settings.h"

/*******************************************************************************
 * @class cp_params
 * @brief параметры работы компорта
 ******************************************************************************/
struct cp_params{
    // Настройки связи
    QString m_portName;           // Имя COM-порта
    int m_baudrate;
    int m_dataBits;
    int m_parity;
    int m_stopBits;
    int m_flowControl;
};

/*******************************************************************************
 * @class ComPort
 * @brief основной класс работы
 ******************************************************************************/
class ComPort : public QObject
{
    Q_OBJECT
public:
    using ErrType = QSerialPort::SerialPortError;

    ComPort(QObject *parent,Settings &sett_cntl){
        qport = new QSerialPort(parent);

        prepar.m_portName=sett_cntl.LoadPortS(Settings::SPortName);
        prepar.m_baudrate=sett_cntl.LoadPortS(Settings::SBaudRate).toInt();
        prepar.m_dataBits=sett_cntl.LoadPortS(Settings::SDatabits).toInt();
        prepar.m_parity=sett_cntl.LoadPortS(Settings::SParity).toInt();
        prepar.m_stopBits=sett_cntl.LoadPortS(Settings::SStopBits).toInt();
        prepar.m_flowControl=QSerialPort::NoFlowControl;

        setConnected(false);
        setErrCode(QSerialPort::NoError);
        m_read=false;
        RThread=NULL;
    }
    ~ComPort(){
        qDebug() << "ComPort end";
        ClosePort();
        delete qport;
    }

    static QString serialPortErrorToString(ErrType error);
    static QString parityToString(QSerialPort::Parity parity);
    static QString iparityToString(qint32 parity);
    static QString stopBitsToString(QSerialPort::StopBits stopBit);
    static QSerialPort::Parity stringToParity(const QString &str);
    QString getSParByIdx(qint32 idx);

    QStringList searchComPorts();
    // похоже чтобы перенастроить ком порт его надо перезапустить, поэтому смысла в этих функциях нет
    ErrType SetPort(cp_params par);
    ErrType SetBaudRate(cp_params par);
    ErrType SetNumBytes(cp_params par);
    ErrType SetParity(cp_params par);
    ErrType SetSBits(cp_params par);

    void ClosePort();
    int SendData(const QByteArray &data);

    std::list<QSerialPort::BaudRate> LstSpeed(){
         std::list<QSerialPort::BaudRate> lst{QSerialPort::Baud1200,QSerialPort::Baud4800,QSerialPort::Baud9600,
                                                QSerialPort::Baud19200,QSerialPort::Baud38400,QSerialPort::Baud57600,QSerialPort::Baud115200};
        return lst;
    }
    std::list<QSerialPort::DataBits> LstBits(){
        std::list<QSerialPort::DataBits> lst{QSerialPort::Data5,QSerialPort::Data6,QSerialPort::Data7,QSerialPort::Data8};
        return lst;
    }
    std::list<QSerialPort::Parity> LstParity(){
        std::list<QSerialPort::Parity> lst{QSerialPort::NoParity,QSerialPort::EvenParity,QSerialPort::OddParity,
                                           QSerialPort::SpaceParity,QSerialPort::MarkParity};
        return lst;
    }
    std::list<QSerialPort::StopBits> LstSBits(){
        std::list<QSerialPort::StopBits> lst{QSerialPort::OneStop,QSerialPort::OneAndHalfStop,QSerialPort::TwoStop};
        return lst;
    }

    // Сеттер для m_connected
    void setConnected(bool connected) {
        m_connected = connected;
    }
    // Геттер для m_connected
    bool isConnected() const {
        return m_connected;
    }

    // Геттер для переменной m_read
    bool getmRead() const {
        return m_read;
    }
    // Сеттер для переменной m_read
    void setmRead(bool read) {
        m_read = read;
    }

    // Сеттер для err_code
    void setErrCode(ErrType code) {
        QMutexLocker locker(&mtx_err_code);
        err_code = code;
    }
    // Геттер для err_code
    ErrType getErrCode() {
        QMutexLocker locker(&mtx_err_code);
        return err_code;
    }
    // Сеттер для prepar
    void setPrepar(cp_params prepar_in) {
        prepar = prepar_in;
    }
    // Геттер для prepar
    cp_params getPrepar() {
        return prepar;
    }

signals:
    void readDone(const QByteArray &result); // получили пакет - передаем

private:
    void ReadThread();
private:
    const int READ_TIMEOUT=100;    // define времени блокировки потока чтения в мс
    const int WRITE_TIMEOUT=5;    // define ожидания завершения записи данных при отправке новых
    QSerialPort *qport;           // основной обьект QtSerialPort

    bool m_connected;             // Есть ли соединение с COM-портом
    bool m_read;                  // управление потоком чтения

    QMutex mtx_err_code;
    ErrType  err_code;   // асинхронный код ошибки компорта

    cp_params prepar;             // предустановленные параметры работы компорта

    QThread *RThread;              // поток для чтения
};

#endif // COMPORT_H
