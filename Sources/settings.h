#ifndef SETTINGS_H
#define SETTINGS_H
#include <QMetaEnum>
#include <QSettings>
#include <qdir.h>

// класс сохранения настроек проги
class Settings : public QObject
{
    Q_OBJECT    // без этого Q_ENUM не сработает
    std::unique_ptr<QSettings> settings;
public:
    enum SetNamesGroups {SPortSettings, STemplate};
    Q_ENUM(SetNamesGroups)
    // Settings::SPortName
    enum SetNamesFileds {SPortName, SBaudRate, SDatabits, SParity, SStopBits};
    Q_ENUM(SetNamesFileds)

    QString STemplateFName = "template.ini";

    // преобразуем enum в стринг
    template<typename TemplateNameVar>
    QString EToS (const TemplateNameVar value){
        return QString(QMetaEnum::fromType<TemplateNameVar>().valueToKey(value));
    }

public:
    Settings(){
        settings = std::make_unique<QSettings>("settings.ini",QSettings::IniFormat);
    }

    // сохранение настроек порта, ключ + значение через QSettings
    void SavePortS(QString data,SetNamesFileds name){
        settings.get()->beginGroup(EToS(SPortSettings));
        settings.get()->setValue(EToS(name),data);
        settings.get()->endGroup();
    }

    QString LoadPortS(SetNamesFileds name){
        settings.get()->beginGroup(EToS(SPortSettings));
        QString data=settings.get()->value(EToS(name)).toString();
        settings.get()->endGroup();
        return data;
    }

    // сохранение шаблонов через тектовик, tablewidget tWStemplate
    void SaveTemplateS(QStringList  data){
        QFile file(STemplateFName);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream stream(&file);
            foreach(QString str, data){
                stream << str << "\n";
                //file.write(str.toStdString().c_str());
            }
            file.close();
        }
    }

    QStringList LoadTemplateS(){
        QStringList data;
        QFile file(STemplateFName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&file);
            while (!stream.atEnd())
                data += stream.readLine();

            file.close();
        }
        return data;
    }
};

#endif // SETTINGS_H
