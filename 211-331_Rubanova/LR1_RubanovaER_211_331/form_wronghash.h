#ifndef FORM_WRONGHASH_H
#define FORM_WRONGHASH_H

#include <QWidget>

namespace Ui {
class Form_wrongHash;
}

class Form_wrongHash : public QWidget
{
    Q_OBJECT

public:
    explicit Form_wrongHash(QWidget *parent = nullptr);
    ~Form_wrongHash();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Form_wrongHash *ui;
};

#endif // FORM_WRONGHASH_H
