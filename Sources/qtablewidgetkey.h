#ifndef QTABLEWIDGETKEY_H
#define QTABLEWIDGETKEY_H

#include <QTableWidget>
#include <QWidget>
#include <QKeyEvent>

class QTableWidgetKey : public QTableWidget
{
    Q_OBJECT
public:
    QTableWidgetKey(QWidget *parent = nullptr): QTableWidget(parent){
    };
protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // QTABLEWIDGETKEY_H
