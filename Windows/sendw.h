#ifndef SENDW_H
#define SENDW_H

#include <QDialog>

#include "..\Sources\qtablewidgetkey.h"

namespace Ui {
class sendw;
}

class sendw : public QDialog
{
    Q_OBJECT

public:
    explicit sendw(QWidget *parent = nullptr);
    ~sendw();

    QTableWidgetKey *QWKtemplate;   // таким образом сделаем доступным компонент tWStemplate
private:
    Ui::sendw *ui;
};

#endif // SENDW_H
