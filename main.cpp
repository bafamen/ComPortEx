#include "mainwindow.h"
#include <QApplication>

/*******************************************************************************
 * @fn messageHandler
 * @brief   Реализация перехватчика лог фалйа
 *  https://evileg.com/ru/post/154/
 * @param   none
 * @return  none
 ******************************************************************************/
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QScopedPointer<QFile>   m_logFile;
    QString path = QDir::currentPath();
    QString name = path+"/qdebug.txt";
    m_logFile.reset(new QFile(name)); // устанавливаем указатель
    m_logFile.data()->open(QFile::Append | QFile::Text);  // Открываем файл логирования
    // Открываем поток записи в файл
    QTextStream out(m_logFile.data());
    // Записываем дату записи
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    // По типу определяем, к какому уровню относится сообщение
    switch (type)
    {
    case QtInfoMsg:     out << "INF "; break;
    case QtDebugMsg:    out << "DBG "; break;
    case QtWarningMsg:  out << "WRN "; break;
    case QtCriticalMsg: out << "CRT "; break;
    case QtFatalMsg:    out << "FTL "; break;
    }
    // Записываем в вывод категорию сообщения и само сообщение
    //out << context.category << ": " << msg << Qt::endl;

    fprintf(stderr, "%s\n",msg.toStdString().c_str()); //for console
    fflush(stderr);
    //out.flush();    // Очищаем буферизированные данные
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // qInstallMessageHandler - QtLogging функция указывает системе логирования, использовать наш обработчик. для записи в файл
    qInstallMessageHandler(messageHandler);
    a.setWindowIcon(QIcon(":/Images/port.png")); // иконка отображаемая в окне

    MainWindow w;
    w.show();
    return a.exec();
}
