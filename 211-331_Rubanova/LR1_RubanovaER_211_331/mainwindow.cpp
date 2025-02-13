#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
//
void MainWindow::on_ok_btn_clicked()
{
    QString pass = ui->pass->text();
    qDebug() << pass;
    if (pass == "1234") {
        //успешный вход

        //int index_page2 = ui->stackedWidget->indexOf(ui->page_2);
        //qDebug() << index_page2;  //index_page2 = 1
        ui->stackedWidget->setCurrentWidget(ui->page_2);
    } else {
        //сед стоори

        ui->stackedWidget->setCurrentWidget(ui->page_3);
    }
}

