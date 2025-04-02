#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItem>
#include <QClipboard>

#include <QCryptographicHash>

#include <openssl/evp.h>
#include <openssl/sha.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ok_btn_clicked();
    void copyLogin();
    void copyPass();

    void on_lineEdit_2_returnPressed();

private:
    Ui::MainWindow *ui;
    QByteArray keyVecForDecrypt;
    QByteArray initVecForDecrypt;
    QByteArray credsDecFile;
    QString encFileName = "D:/secure_as/211-331_Rubanova/LR1_RubanovaER_211_331/creds.enc";
    QString decFileName =  "D:/secure_as/211-331_Rubanova/LR1_RubanovaER_211_331/credentials.json";

    QJsonArray findCredentialsByHostname(const QJsonArray& credentials, const QString& hostname);
    bool encryptFile(const QByteArray &key, const QByteArray &iv);
    QByteArray decryptFile(const QByteArray &key, const QByteArray &iv);
    QString decryptInfo(const QString &encryptedData);
    void genAESKeyInfo(const QString &pass, QByteArray &key, QByteArray &iv);
    QJsonArray readFromJsonFile(QByteArray& jsonData);
    void setup(QJsonArray& creds);
};
#endif // MAINWINDOW_H
