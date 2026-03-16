#include "..\mainwindow.h"
#include <QMenu>
#include <QFileDialog>

/*******************************************************************************
 * @fn STableWork_ConMenu (slot table work context menu)
 * @brief   вызов контекстного меню для таблиц, рабочие поля
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::STableWork_ConMenu(const QPoint &pos)
{
    QMenu *popmenu;
    QStringList plist={"Save"};
    QTableWidget *qtw = qobject_cast<QTableWidget *>(sender());
    // удалим старые пункты
    foreach (QAction *action, popmenu2->actions()) {
        delete action;
    }
    // пересоздадим пункты меню
    for(const QString &pname : static_cast<const QStringList>(plist)){
        QAction *action = new QAction(pname,qtw);
        action->setObjectName(pname);
        connect(action, &QAction::triggered, this, &MainWindow::SaveTable);
        popmenu2->addAction(action);
    }

    popmenu2->popup(qtw->mapToGlobal(pos));
}

/*******************************************************************************
 * @fn SaveTable (slot)
 * @brief   сохраним в файл инфу с QTableWidget
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::SaveTable(void)
{
    QAction *action = qobject_cast<QAction *>(sender());
    QTableWidget *qtw = qobject_cast<QTableWidget *>(action->parent());

    // вызов модального окна
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    QString::fromUtf8("Save file"),
                                                    QDir::currentPath(),
                                                    "Text (*.txt);;All files (*.*)");

    QFile file(fileName); // устанавливаем указатель
    file.open(QFile::WriteOnly | QFile::Text);  // Открываем файл логирования
    QTextStream out(&file); // Открываем поток записи в файл

    int rowCount = qtw->rowCount();
    int columnCount = qtw->columnCount();
    // Перебор всех строк
    for (int row = 0; row < rowCount; ++row) {
        QStringList rowData;
        // Перебор всех столбцов в строке
        for (int column = 0; column < columnCount; ++column) {
            // Получаем текст из ячейки
            QTableWidgetItem *item = qtw->item(row, column);
            if (item)
                rowData << item->text()+" ";  // Добавляем текст в список
            else
                rowData << " ";  // Если ячейка пуста, добавляем пустую строку
        }
        // Записываем строку в поток
        out << rowData.join("\t") << "\n";  // Разделяем данные табуляцией и переходим на новую строку
    }
    file.close();
}

/*******************************************************************************
 * @fn STableHead_ConMenu (slot table head context menu)
 * @brief   вызов контекстного меню для таблиц, поля head
 *          цель отключение столбцов
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::STableHead_ConMenu(const QPoint &pos)
{
    int head_cnt=0; // счетчик столбцов в заголовке
    QMenu *popmenu;
    QStringList plist={"#","Time","Data"};
    QTableWidget *qtw = qobject_cast<QTableWidget *>(sender()->parent());
    // удалим старые пункты
    foreach (QAction *action, popmenu2->actions()) {
        delete action;
    }
    // пересоздадим пункты меню
    for(const QString &pname : static_cast<const QStringList>(plist)){
        QAction *action = new QAction(pname,qtw);
        action->setObjectName(pname);
        action->setCheckable(true);
        if(qtw->isColumnHidden(head_cnt))
            action->setChecked(false);
        else
            action->setChecked(true);
        connect(action, &QAction::triggered, this, &MainWindow::ProcessColomn);
        popmenu2->addAction(action);
        head_cnt++;
    }

    popmenu2->popup(qtw->mapToGlobal(pos));
}

/*******************************************************************************
 * @fn ProcessColomn (slot)
 * @brief   отображаем и скрываем колонку в таблице в зависимости от выставленной
 *          галочки в меню STableHead_ConMenu
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::ProcessColomn(void)
{
    QAction *action = qobject_cast<QAction *>(sender());
    QTableWidget *qtw = qobject_cast<QTableWidget *>(action->parent());
    QStringList plist={"#","Time","Data"};
    int head_cnt=plist.indexOf(action->objectName()); // номер столбца в заголовке

    if(action->isChecked())
        qtw->setColumnHidden(head_cnt,false); // скрываем столбец. инверсия!!!
    else
        qtw->setColumnHidden(head_cnt,true); // показываем столбец.
}

/*******************************************************************************
 * @fn SendW_ConMenu (slot table head context menu)
 * @brief   вызов контекстного меню для дополнительного окна шаблонов посылок
 * окно sendw. рабочий виджет <QTableWidget>tWStemplate .
 * Меню
 * добавить строку
 * удалить строку
 * сделать маркер *
 * убрать маркер *
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::SendW_ConMenu(const QPoint &pos)
{
    QMenu *popmenu;
    QStringList plist={"Add","Delete","Set*","Remove*"};
    QTableWidget *qtw = qobject_cast<QTableWidget *>(sender());
    // удалим старые пункты
    foreach (QAction *action, popmenu2->actions()) {
        delete action;
    }
    // пересоздадим пункты меню
    for(const QString &pname : static_cast<const QStringList>(plist)){
        QAction *action = new QAction(pname,qtw);
        action->setObjectName(pname);
        action->setCheckable(false);
        connect(action, &QAction::triggered, this, &MainWindow::ProcessSendW);
        popmenu2->addAction(action);
    }

    popmenu2->popup(qtw->mapToGlobal(pos));
}


/*******************************************************************************
 * @fn ProcessSendW
 * @brief   обработчик всплывающего меню для окна шаблонов sendw
 * @param   none
 * @return  none
 ******************************************************************************/
void MainWindow::ProcessSendW(void)
{
    QAction *action = qobject_cast<QAction *>(sender());
    QStringList plist={"Add","Delete","Set*","Remove*"};
    QTableWidget *tbl=SendWindow->QWKtemplate;
    if(action->objectName()==plist[0]){ // Add
        tbl->setRowCount(tbl->rowCount()+1);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
        tbl->setSelectionMode(QAbstractItemView::SingleSelection);
        tbl->selectRow(tbl->rowCount()-2);
    }
    if(action->objectName()==plist[1]){ // Delete
        QList lst=tbl->selectedItems();
        if(lst.empty()) return;
        QTableWidgetItem *itm=lst.last();
        int dbg=itm->row();
        tbl->removeRow(dbg);
    }

    if(action->objectName()==plist[2]){ // Set*
        int count=tbl->rowCount();
        QTableWidgetItem *cur_itm=tbl->currentItem();

        // по всем шаблонам пройдем и удалим * в другом
        for(int cnt=0; cnt<count; cnt++){
            QTableWidgetItem *itm=tbl->item(cnt,0);
            if(itm==NULL) break;
            QString str=itm->text();
            QChar ch = str.front();
            if(ch=='*') // удалим  символ
                str.remove(0,1);
            itm->setText(str);
        }// for
        QString str=cur_itm->text();
        str.insert(0,'*');
        cur_itm->setText(str);
    }

    if(action->objectName()==plist[3]){ // Remove*
        QTableWidgetItem *cur_itm=tbl->currentItem();
        if(cur_itm==NULL) return;
        QString str=cur_itm->text();
        QChar ch = str.front();
        if(ch=='*') // удалим  символ
            str.remove(0,1);
        cur_itm->setText(str);
    }
}
