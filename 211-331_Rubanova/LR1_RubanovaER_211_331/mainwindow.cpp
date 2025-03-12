#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumSize(600, 300);
    this->setMaximumSize(600, 300);
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
        QByteArray key;
        QByteArray initVec;

        genAESKeyInfo(pass, key, initVec);

        credsDecFile = decryptFile(key, initVec);
        QJsonArray creds = readFromJsonFile(credsDecFile);
        qDebug() << creds.size();
        setup(creds);
        //encryptFile(key, initVec);

        ui->stackedWidget->setCurrentWidget(ui->page_2);
    } else {
        //сед стоори

        ui->stackedWidget->setCurrentWidget(ui->page_3);
    }
}

QJsonArray MainWindow::readFromJsonFile(QByteArray& jsonData)
{
    QJsonArray jsonCredsArray;
    //QByteArray - расшифрованныеп даннеы в памяти
    //
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError) {
        QJsonObject jsonRootObject = jsonDoc.object();
        jsonCredsArray = jsonRootObject["credentials"].toArray();
    }
    return jsonCredsArray;
}


void MainWindow::setup(QJsonArray& creds)
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    for (int cred_i = 0; cred_i < creds.size(); cred_i++) {
        QJsonObject jsonCredElement = creds.at(cred_i).toObject();
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(jsonCredElement["hostname"].toString()));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem("****"));

        QPushButton *button = new QPushButton("Copy");
        button->setObjectName(jsonCredElement["login_password"].toString()); // Назначаем идентификатор
        connect(button, &QPushButton::clicked, this, &MainWindow::copyLogin);
        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1, 2, button);

        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 3, new QTableWidgetItem("****"));

        QPushButton *button1 = new QPushButton("Copy");
        button1->setObjectName(jsonCredElement["login_password"].toString()); // Назначаем идентификатор
        connect(button1, &QPushButton::clicked, this, &MainWindow::copyPass);
        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1, 4, button1);

        qDebug() << cred_i;
        qDebug() << jsonCredElement["hostname"].toString();
    }

}


void MainWindow::copyLogin()
{
    QClipboard *clipboard = QApplication::clipboard();
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    qDebug() << buttonSender->objectName();

    QString encInfo = buttonSender->objectName();
    QString decInfo = decryptInfo(encInfo);

    QStringList parts = decInfo.split(' ');

    // Копируем текст в буфер обмена
    clipboard->setText(parts[0]);
}


void MainWindow::copyPass()
{
    QClipboard *clipboard = QApplication::clipboard();
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    qDebug() << buttonSender->objectName();

    QString encInfo = buttonSender->objectName();
    QString decInfo = decryptInfo(encInfo);

    QStringList parts = decInfo.split(' ');

    // Копируем текст в буфер обмена
    clipboard->setText(parts[1]);
}


QJsonArray MainWindow::findCredentialsByHostname(const QJsonArray& credentials, const QString& hostname)
{
    QJsonArray jsonArray;

    for (const QJsonValue& value : credentials) {
        QJsonObject credential = value.toObject();
        if (credential["hostname"].toString().contains(hostname)) {
            qDebug() << "Найдены credentials для hostname" << hostname;
            qDebug() << "Hostname:" << credential["hostname"].toString();
            qDebug() << "Login/Password:" << credential["login_password"].toString();

            // Создаем QJsonObject
            QJsonObject item;
            item["hostname"] = credential["hostname"];
            item["login_password"] = credential["login_password"];

            // Добавляем объекты в массив
            jsonArray.append(item);
        }
    }
    qDebug() << "Credentials для hostname" << hostname << "не найдены.";
    return jsonArray;
}


void MainWindow::on_lineEdit_2_returnPressed()
{
    qDebug() << "поиск";
    QString hostname_to_search = ui->lineEdit_2->text();

    QJsonArray credentials = readFromJsonFile(this->credsDecFile);
    QJsonArray filter_info = findCredentialsByHostname(credentials, hostname_to_search);
    setup(filter_info);
}


bool MainWindow::encryptFile(const QByteArray &key, const QByteArray &iv) {
    QFile inputFile(decFileName);
    QFile outputFile(encFileName);

    if (!inputFile.open(QIODevice::ReadOnly) || !outputFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Ошибка открытия файлов!";
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qWarning() << "Ошибка создания контекста OpenSSL!";
        return false;
    }

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char*>(key.data()),
                       reinterpret_cast<const unsigned char*>(iv.data()));

    QByteArray buffer(4096, 0);
    QByteArray encryptedBuffer(4096 + EVP_CIPHER_block_size(EVP_aes_256_cbc()), 0);

    int outLen = 0;
    while (!inputFile.atEnd()) {
        int bytesRead = inputFile.read(buffer.data(), buffer.size());
        EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(encryptedBuffer.data()), &outLen,
                          reinterpret_cast<const unsigned char*>(buffer.constData()), bytesRead);
        outputFile.write(encryptedBuffer.constData(), outLen);
    }

    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(encryptedBuffer.data()), &outLen);
    outputFile.write(encryptedBuffer.constData(), outLen);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}


QByteArray MainWindow::decryptFile(const QByteArray &key, const QByteArray &iv) {
    QFile inputFile(encFileName);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Ошибка открытия файла!";
        return {};
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qWarning() << "Ошибка создания контекста OpenSSL!";
        return {};
    }

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char*>(key.data()),
                       reinterpret_cast<const unsigned char*>(iv.data()));

    QByteArray decryptedData;
    QByteArray buffer(4096, 0);
    QByteArray decryptedBuffer(4096 + EVP_CIPHER_block_size(EVP_aes_256_cbc()), 0);

    int outLen = 0;
    while (!inputFile.atEnd()) {
        int bytesRead = inputFile.read(buffer.data(), buffer.size());
        EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(decryptedBuffer.data()), &outLen,
                          reinterpret_cast<const unsigned char*>(buffer.constData()), bytesRead);
        decryptedData.append(decryptedBuffer.constData(), outLen);
    }

    EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(decryptedBuffer.data()), &outLen);
    decryptedData.append(decryptedBuffer.constData(), outLen);

    EVP_CIPHER_CTX_free(ctx);


    return decryptedData;
}


QString MainWindow::decryptInfo(const QString &encryptedData) {
    QByteArray encryptedBytes = QByteArray::fromHex(encryptedData.toUtf8());
    QByteArray decryptedData(encryptedBytes.size(), 0);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qWarning() << "Ошибка создания контекста OpenSSL!";
        return QString();
    }

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char*>(keyVecForDecrypt.data()),
                       reinterpret_cast<const unsigned char*>(initVecForDecrypt.data()));

    int outLen = 0;
    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(decryptedData.data()), &outLen,
                      reinterpret_cast<const unsigned char*>(encryptedBytes.constData()), encryptedBytes.size());

    int finalLen = 0;
    EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(decryptedData.data()) + outLen, &finalLen);

    EVP_CIPHER_CTX_free(ctx);

    decryptedData.resize(outLen + finalLen);
    return QString::fromUtf8(decryptedData);
}


void MainWindow::genAESKeyInfo(const QString &pass, QByteArray &key, QByteArray &iv) {
    QString s = "p[kwsacx";
    QByteArray salt = s.toUtf8();
    QByteArray pass_bytes = pass.toUtf8();
    QByteArray hash = QCryptographicHash::hash(salt + pass_bytes, QCryptographicHash::Sha256);

    key = hash.left(32);
    iv = hash.right(16);

    qDebug() << key.toHex();
    qDebug() << iv.toHex();

    s = "wmxewnc";
    salt = s.toUtf8();
    QByteArray hash2 = QCryptographicHash::hash(pass_bytes + salt, QCryptographicHash::Sha256);

    keyVecForDecrypt = hash2.left(32);
    initVecForDecrypt = hash2.right(16);

    qDebug() <<  keyVecForDecrypt.toHex();
    qDebug() << initVecForDecrypt.toHex();
}
