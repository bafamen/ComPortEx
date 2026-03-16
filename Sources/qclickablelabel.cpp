#include "qclickablelabel.h"

void QClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked(this->pos());
}
