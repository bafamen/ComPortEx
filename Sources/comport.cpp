 #include "comport.h"
#include <thread>
#include <chrono>

/*******************************************************************************
 * @fn serialPortErrorToString
 * @brief   Функция для преобразования кода ошибки в строку
 * @param   error
 * @return  QString
 ******************************************************************************/
QString ComPort::serialPortErrorToString(ErrType error) {
    switch (error) {
    case QSerialPort::NoError:
        return "No error occurred.";
    case QSerialPort::DeviceNotFoundError:
        return "An error occurred while attempting to open a non-existing device.";
    case QSerialPort::PermissionError:
        return "An error occurred while attempting to open a device already opened by another process or insufficient permissions.";
    case QSerialPort::OpenError:
        return "An error occurred while attempting to open an already opened device in this object.";
    case QSerialPort::WriteError:
        return "An I/O error occurred while writing the data.";
    case QSerialPort::ReadError:
        return "An I/O error occurred while reading the data.";
    case QSerialPort::ResourceError:
        return "An I/O error occurred when a resource becomes unavailable, e.g., when the device is unexpectedly removed.";
    case QSerialPort::UnsupportedOperationError:
        return "The requested device operation is not supported or prohibited by the running operating system.";
    case QSerialPort::UnknownError:
        return "An unidentified error occurred.";
    case QSerialPort::TimeoutError:
        return "A timeout error occurred.";
    case QSerialPort::NotOpenError:
        return "This error occurs when an operation is executed that can only be successfully performed if the device is open.";
    default:
        return "Unknown error code.";
    }
}

/*******************************************************************************
 * @fn parityToString
 * @brief   вы полнить конвертацию перечисления Parity в string
 * @param   Parity
 * @return  QString
 ******************************************************************************/
QString ComPort::parityToString(QSerialPort::Parity parity) {
    switch (parity) {
    case QSerialPort::NoParity:
        return "No";
    case QSerialPort::EvenParity:
        return "Even";
    case QSerialPort::OddParity:
        return "Odd";
    case QSerialPort::SpaceParity:
        return "Space";
    case QSerialPort::MarkParity:
        return "Mark";
    default:
        return "Unknown Parity";
    }
}

/*******************************************************************************
 * @fn iparityToString
 * @brief   вы полнить конвертацию перечисления Parity в виде инта в string
 * @param   Parity int
 * @return  QString
 ******************************************************************************/
QString ComPort::iparityToString(qint32 parity) {
    QSerialPort::Parity par = static_cast<QSerialPort::Parity>(parity);
    switch (par) {
    case QSerialPort::NoParity:
        return "No";
    case QSerialPort::EvenParity:
        return "Even";
    case QSerialPort::OddParity:
        return "Odd";
    case QSerialPort::SpaceParity:
        return "Space";
    case QSerialPort::MarkParity:
        return "Mark";
    default:
        return "Unknown Parity";
    }
}

/*******************************************************************************
 * @fn stringToParity
 * @brief   вы полнить конвертацию string в перечисления Parity
 * @param   Parity
 * @return  QString
 ******************************************************************************/
QSerialPort::Parity ComPort::stringToParity(const QString &str) {
    if (str == "No") {
        return QSerialPort::NoParity;
    } else if (str == "Even") {
        return QSerialPort::EvenParity;
    } else if (str == "Odd") {
        return QSerialPort::OddParity;
    } else if (str == "Space") {
        return QSerialPort::SpaceParity;
    } else if (str == "Mark") {
        return QSerialPort::MarkParity;
    } else {
        return QSerialPort::NoParity; // Возвращаем NoParity, если строка не соответствует ни одному значению
    }
}

/*******************************************************************************
 * @fn parityToString
 * @brief   вы полнить конвертацию перечисления StopBits в string
 * @param   StopBits
 * @return  QString
 ******************************************************************************/
QString ComPort::stopBitsToString(QSerialPort::StopBits stopBit) {
    switch (stopBit) {
    case QSerialPort::OneStop:
        return "OneStop";
    case QSerialPort::OneAndHalfStop:
        return "OneAndHalfStop";
    case QSerialPort::TwoStop:
        return "TwoStop";
    default:
        return "Unknown StopBit";
    }
}

/*******************************************************************************
 * @fn getSParByIdx
 * @brief   возвращает параметр из стурктуры cp_params в виде строки, по индексу
 *          этого параметра в структуре
 * @param   StopBits
 * @return  QString
 ******************************************************************************/
QString ComPort::getSParByIdx(qint32 idx) {
    switch (idx) {
    case 0:
        return prepar.m_portName;
    case 1:
        return QString::number(prepar.m_baudrate);
    case 2:
        return QString::number(prepar.m_dataBits);
    case 3:
        return ComPort::iparityToString(prepar.m_parity);
    case 4:
        return QString::number(prepar.m_stopBits);
    default:
        return "--";
    }
}

/*******************************************************************************
 * @fn searchComPorts
 * @brief   создание списка всех компортов
 * @param   none
 * @return  список компортов
 ******************************************************************************/
QStringList ComPort::searchComPorts()
{
    QStringList m_listPorts;      // Список COM-портов в системе
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        m_listPorts.append(info.portName());
    }
    return m_listPorts;
}

/*******************************************************************************
 * @fn SetPort
 * @brief   инициализация порта с заданными параметрами
 * @param   структура настроки порта
 * @return  0 - нет ошибок
 ******************************************************************************/
ComPort::ErrType ComPort::SetPort(cp_params par){
    qDebug() << "SetPort " << QThread::currentThreadId();
    ErrType result=QSerialPort::NoError;
    ClosePort();
    qport->setPortName(par.m_portName);
    // Открываем COM-порт
    if (qport->open(QIODevice::ReadWrite)){
        // Выставляем настройки
        if ((qport->setBaudRate(par.m_baudrate)
             && qport->setDataBits((QSerialPort::DataBits)par.m_dataBits)
             && qport->setParity((QSerialPort::Parity)par.m_parity)
             && qport->setStopBits((QSerialPort::StopBits)par.m_stopBits)
             && qport->setFlowControl((QSerialPort::FlowControl)par.m_flowControl)))
        {
            setConnected(true);
            setmRead(true);
            auto fn = std::bind(&ComPort::ReadThread, this);
            RThread = QThread::create(fn); // загоняем в поток функцию обработки чтения
            RThread->start();
        }
    }

    if(isConnected() == false){
        result=qport->error();
        setErrCode(result); // если в потоке
    }
    else
        qport->clear();

    qDebug() << "SetPort out";
    return result;
}

/*******************************************************************************
 * @fn SetBaudRate
 * @brief   выставляем скорость потока
 * @param   параметры порта
 * @return  код ошибки
 ******************************************************************************/
ComPort::ErrType ComPort::SetBaudRate(cp_params par){
    ErrType result=QSerialPort::NoError;

    if(qport->setBaudRate(par.m_baudrate)==false)
        result=qport->error();
    setErrCode(result); // дублируем в потоко независимую переменную, впринципе не надо
    return result;
}

/*******************************************************************************
 * @fn SetNumBytes
 * @brief   выставляем количество бит
 * @param   параметры порта
 * @return  код ошибки
 ******************************************************************************/
ComPort::ErrType ComPort::SetNumBytes(cp_params par){
    ErrType result=QSerialPort::NoError;

    if(qport->setDataBits(static_cast<QSerialPort::DataBits>(par.m_dataBits))==false)
        result=qport->error();
    setErrCode(result);
    return result;
}

/*******************************************************************************
 * @fn SetParity
 * @brief   выставляем четность
 * @param   параметры порта
 * @return  код ошибки
 ******************************************************************************/
ComPort::ErrType ComPort::SetParity(cp_params par){
    ErrType result=QSerialPort::NoError;

    if(qport->setParity(static_cast<QSerialPort::Parity>(par.m_parity))==false)
        result=qport->error();
    setErrCode(result);
    return result;
}

/*******************************************************************************
 * @fn SetSBits
 * @brief   выставляем количество стоп бит
 * @param   параметры порта
 * @return  код ошибки
 ******************************************************************************/
ComPort::ErrType ComPort::SetSBits(cp_params par){
    ErrType result=QSerialPort::NoError;

    if(qport->setStopBits(static_cast<QSerialPort::StopBits>(par.m_stopBits))==false)
        result=qport->error();
    setErrCode(result);
    return result;
}

/*******************************************************************************
 * @fn ClosePort
 * @brief   закрытие порта.
 * @param   none
 * @return  none
 ******************************************************************************/
void ComPort::ClosePort(){
    setConnected(false);  // в ReadThread сделает m_read==false
    while(getmRead()){
        std::this_thread::sleep_for(std::chrono::milliseconds(READ_TIMEOUT)); // 100 ms
    }

    if(RThread!=NULL){
        RThread->quit();
        delete RThread;
        RThread=NULL;
    }
    qport->close();
}

/*******************************************************************************
 * @fn ReadThread
 * @brief   функция потока чтения.
 * @param   none
 * @return  массив данных принятых по ком порту
 ******************************************************************************/
void ComPort::ReadThread(){
    qDebug() << "ReadThread start " << QThread::currentThreadId();
    while(getmRead()){
        if(isConnected() == false){
            setmRead(false);
            break;
        }

        {
            qint64 num = qport->bytesAvailable();
            // Читаем ответ
            QByteArray responseFromPort = qport->readAll();

            while (qport->waitForReadyRead(10)) // вычитывам все что пришло
            {
                responseFromPort += qport->readAll();
            }

            if(responseFromPort.size())
                emit readDone(responseFromPort);  // Отправляем сигнал о том, что ответ получен
            responseFromPort.clear();
        }// data

        std::this_thread::sleep_for(std::chrono::milliseconds(READ_TIMEOUT)); // 100 ms
        int h=0;
    }// m_read
    qDebug() << "ReadThread end";
}

/*******************************************************************************
 * @fn SendData
 * @brief   послать данные
 * @param   массив данных
 * @return  none
 ******************************************************************************/
int ComPort::SendData(const QByteArray &data){
    int result=0;
    // Даем время на отправку
    if(isConnected() == false) return result;
    qport->clear(QSerialPort::Output); // чистим буфер выходной
    result=qport->write(data);
    qport->waitForBytesWritten(ComPort::WRITE_TIMEOUT);
    return result;
}
