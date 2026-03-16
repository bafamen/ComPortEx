#include "qtablewidgetkey.h"

/*******************************************************************************
 * @fn keyPressEvent
 * @brief перегрузка обрабочика нажатия кнопки, по нажатию кнопки вниз
 *        создаем новую ячейку
 * @param   none
 * @return  none
 ******************************************************************************/
void QTableWidgetKey::keyPressEvent(QKeyEvent *event)
{
    QModelIndexList sel;
    QModelIndexList::Iterator it;
    int deb;
    switch(event->key()) {
    case Qt::Key_Down:
        //emit downEvent(currentIndex());
        sel = this->selectedIndexes();
        //for(QModelIndexList::Iterator it=sel.begin(); it!=sel.end(); it++)
        it=sel.end()-1; // последний в списке QModelIndexList
        deb=sel.size(); // size 1
        deb=it->row();  // index = 0
        deb=this->rowCount();
        if(it->row()==(this->rowCount()-1)) // если это последняя строка, создадим новую строку
            this->setRowCount(this->rowCount()+1);

        this->selectRow(this->rowCount()-1); // передвинем выделение на созданную строку эта функция в родителе QTableView
    break;
    case Qt::Key_Up:
    default: QTableWidget::keyPressEvent(event);
    }
}

