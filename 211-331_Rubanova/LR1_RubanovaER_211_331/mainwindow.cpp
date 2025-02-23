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
        setupInfo();
        ui->stackedWidget->setCurrentWidget(ui->page_2);

    } else {
        //сед стоори

        ui->stackedWidget->setCurrentWidget(ui->page_3);
    }
}


void MainWindow::setupInfo()
{
    QFile jsonFile("../../credentials.json");
    if (jsonFile.open(QFile::ReadOnly | QFile::Text)) {
        QByteArray jsonData = jsonFile.readAll();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
        if (jsonError.error == QJsonParseError::NoError) {
            QJsonObject jsonRootObject = jsonDoc.object();
            QJsonArray jsonCredsArray = jsonRootObject["credentials"].toArray();
             ui->tableWidget->clear();
            for (int cred_i = 0; cred_i < jsonCredsArray.size(); cred_i++) {
                QJsonObject jsonCredElement = jsonCredsArray.at(cred_i).toObject();
                ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(jsonCredElement["hostname"].toString()));
                ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem("****"));

                QPushButton *button = new QPushButton("Copy");
                button->setObjectName(jsonCredElement["login_password"].toString()); // Назначаем идентификатор
                connect(button, &QPushButton::clicked, this, &MainWindow::copy);
                ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1, 2, button);

                ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 3, new QTableWidgetItem("****"));

                QPushButton *button1 = new QPushButton("Copy");
                button1->setObjectName(jsonCredElement["login_password"].toString()); // Назначаем идентификатор
                connect(button1, &QPushButton::clicked, this, &MainWindow::copy);
                ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1, 4, button1);
            }
        } else {
            qDebug() << "Ошибка при разборе JSON:" << jsonError.errorString();
        }
        jsonFile.close();
    } else {
        qDebug() << "Невозможно открыть файл";
    }
}


void MainWindow::copy()
{
    QClipboard *clipboard = QApplication::clipboard();
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    qDebug() << buttonSender->objectName();
    // Копируем текст в буфер обмена
    clipboard->setText(buttonSender->objectName());

}
