#include "sendw.h"
#include "ui_sendw.h"

sendw::sendw(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::sendw)
{
    ui->setupUi(this);
    QWKtemplate=ui->tWStemplate;
}

sendw::~sendw()
{
    delete ui;
}
