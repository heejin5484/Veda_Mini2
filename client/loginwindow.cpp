#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>

LoginWindow::LoginWindow(NetworkManager *networkManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow),
    networkManager(networkManager)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_LoginButton_clicked()
{
    QString userId = ui->userIDlineEdit->text(); // 사용자 ID 가져오기
        QString password = ui->passwordLineEdit->text(); // 비밀번호 가져오기

        if (userId.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "모든 필드를 채워주세요."); // 입력 오류 메시지
            return;
        }

        // 로그인 요청 JSON 객체 생성
        QJsonObject json;
        json["type"] = "L"; // 로그인 유형
        json["userid"] = userId; // 사용자 ID
        json["password"] = password; // 비밀번호

        // 네트워크 매니저를 통해 서버에 로그인 요청 전송
        networkManager->sendMessage(json);

}

