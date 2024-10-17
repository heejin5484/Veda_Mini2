#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

LoginWindow::LoginWindow(NetworkManager *networkManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow),
    networkManager(networkManager)
{
    ui->setupUi(this);
    connect(networkManager, &NetworkManager::responseReceived, this, &LoginWindow::handleServerResponse);

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

// login success func
void LoginWindow::handleServerResponse(bool success, const QString &message) {
    if (success) {
        // JSON 응답을 파싱하여 사용자 정보 저장
        QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
        QJsonObject jsonObj = doc.object();

        // MainWindow에 사용자 정보 설정
        MainWindow *mainWindow = static_cast<MainWindow*>(parent());
        if (mainWindow) {
            mainWindow->currentUser.userid = jsonObj["userid"].toString();
            mainWindow->currentUser.name = jsonObj["name"].toString();
            mainWindow->currentUser.password = jsonObj["password"].toString(); // 비밀번호는 일반적으로 안전을 위해 저장하지 않는 것이 좋습니다.

            QMessageBox::information(this, "로그인 성공", "환영합니다, " + mainWindow->currentUser.name + "님!");
            mainWindow->setConnectButtonEnabled(true);
        }

        QMessageBox::information(this, "로그인 성공", "로그인에 성공했습니다.");
        accept(); // 로그인 성공 시 창 닫기
    } else {
        QMessageBox::warning(this, "로그인 실패", message);
    }
}

