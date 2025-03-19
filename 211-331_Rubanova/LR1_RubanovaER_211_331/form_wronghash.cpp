#include "form_wronghash.h"
#include "ui_form_wronghash.h"

Form_wrongHash::Form_wrongHash(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form_wrongHash)
{
    ui->setupUi(this);
    this->setMinimumSize(450, 300);
    this->setMaximumSize(450, 300);
    connect(ui->pushButton, &QPushButton::clicked, qApp, &QApplication::quit);
}

Form_wrongHash::~Form_wrongHash()
{
    delete ui;
}

void Form_wrongHash::on_pushButton_clicked()
{
    //quit();
}

