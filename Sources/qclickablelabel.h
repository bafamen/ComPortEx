#ifndef QCLICKABLELABEL_H
#define QCLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>

class QClickableLabel: public QLabel
{
    Q_OBJECT
public:
    explicit QClickableLabel(const QString &text,QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()): QLabel(text,parent){};
    ~QClickableLabel(){};
signals:
    void clicked(QPoint pos);
protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif // QCLICKABLELABEL_H
