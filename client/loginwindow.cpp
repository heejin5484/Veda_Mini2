#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

LoginWindow::LoginWindow(NetworkManager *networkManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow),
    networkManager(networkManager)
{
    ui->setupUi(this);
    applyStyles();
    connect(networkManager, &NetworkManager::responseReceived, this, &LoginWindow::handleServerResponse);
    connect(ui->togglePasswordButton, &QPushButton::clicked, this, &LoginWindow::togglePasswordVisibility);


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
void LoginWindow::handleServerResponse(bool success, const QString &message, const QString &userId, const QString &name)
{
    qDebug() << "성공 여부:" << success;
    qDebug() << "서버에서 수신한 메시지:" << message;
    qDebug() << "id:" << userId;
    qDebug() << "name:" << name;

    if (success) {
            // JSON 응답을 파싱하여 사용자 정보 저장
            QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
            QJsonObject jsonObj = doc.object();

            // MainWindow에 사용자 정보 설정
            MainWindow *mainWindow = static_cast<MainWindow*>(parent());
            if (mainWindow) {

                mainWindow->currentUser.userid = userId;
                mainWindow->currentUser.name = name;

                qDebug() << "로그인 성공! 유저 아이디: " << mainWindow->currentUser.userid;
                qDebug() << "로그인 성공! 유저 이름: " << mainWindow->currentUser.name;

                QMessageBox::information(this, "로그인 성공", "환영합니다, " + mainWindow->currentUser.name + "님!");

                mainWindow->setConnectButtonEnabled(true);
            }

            QMessageBox::information(this, "로그인 성공", "로그인에 성공했습니다.");
            accept(); // 로그인 성공 시 창 닫기
        }
        else {
            QMessageBox::warning(this, "로그인 실패", message);
        }
 }


void LoginWindow::togglePasswordVisibility() {
    if (ui->passwordLineEdit->echoMode() == QLineEdit::Password) {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);  // 비밀번호 보이기
    } else {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);  // 비밀번호 숨기기
    }
}

void LoginWindow::applyStyles()
{
    this->setStyleSheet(
        "QDialog {"
        "background-color: #f2f2f2;"  // 배경색
        "border-radius: 10px;"  // 둥근 모서리
        "padding: 20px;"
        "}"
        "QPushButton {"
        "background-color: #4CAF50;"  // 기본 버튼 색
        "color: white;"
        "border: none;"
        "border-radius: 5px;"  // 둥근 버튼
        "padding: 10px;"
        "font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "background-color: #45a049;"  // 호버 시 색상 변경
        "}"
        "QPushButton:pressed {"
        "background-color: #388e3c;"  // 클릭 시 색상 변경
        "}"
        "QLineEdit {"
        "border: 1px solid #ccc;"  // 입력 필드 테두리
        "border-radius: 5px;"  // 둥근 입력 필드
        "padding: 10px;"
        "background-color: white;"  // 입력 필드 배경색
        "font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "border-color: #4CAF50;"  // 포커스 시 테두리 색상 변경
        "}"
        "QLabel {"
        "font-size: 14px;"
        "color: #333;"
        "}"
        "QMessageBox {"
        "font-size: 14px;"
        "}"
    );
}
