#include "joinwindow.h"
#include "ui_joinwindow.h"

JoinWindow::JoinWindow(NetworkManager *networkManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinWindow),
    networkManager(networkManager)
{
    ui->setupUi(this);
    connect(ui->joinBtn, &QPushButton::clicked, this, &JoinWindow::on_joinBtn_clicked);  // 올바른 연결
    connect(ui->loginBtn, &QPushButton::clicked, this, &JoinWindow::on_loginBtn_clicked);  // 로그인 버튼 연결
    connect(networkManager, &NetworkManager::signupResponse, this, &JoinWindow::on_signupResponse);

}

JoinWindow::~JoinWindow()
{
    delete ui;
}

void JoinWindow::on_joinBtn_clicked()
{
    QString name = ui->nameLineEdit->text();
    QString phone = ui->phoneLineEdit->text();
    QString email = ui->emailLineEdit->text();
    QString userid = ui->idLineEdit->text();
    QString password = ui->pwLineEdit->text();
    QString confirmPassword = ui->configPwLineEdit->text();

    if (name.isEmpty() || phone.isEmpty() || email.isEmpty() || userid.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "모든 필드를 채워주세요.");
        return;
    }

    if (!isValidPassword(password)) {
        QMessageBox::warning(this, "Invalid Password",
                                     "Password must be at least 8 characters long and contain at least one special character.");
                return;
    }

    if (password != confirmPassword) {
            QMessageBox::warning(this, "Password Mismatch", "비밀번호가 일치하지 않습니다."); // 비밀번호 불일치 확인
            return;
        }

    QJsonObject json;
        json["type"] = "J"; // 회원가입 유형
        json["name"] = name; // 이름
        json["phone"] = phone; // 전화번호
        json["email"] = email; // 이메일
        json["username"] = userid; // 사용자 아이디디
        json["password"] = password; // 비밀번호

        networkManager->sendMessage(json);  // JSON 객체를 서버에 전송


}



bool JoinWindow::isValidPassword(const QString &password) {
    // 비밀번호는 8자 이상이어야 하며, 하나 이상의 특수문자를 포함해야 함
    QRegularExpression specialCharRegex("[!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]+");

    if (password.length() < 8) {
        return false;  // 8자 이상이어야 함
    }

    if (!specialCharRegex.match(password).hasMatch()) {
        return false;  // 특수 문자가 포함되어 있지 않음
    }

    return true;
}


void JoinWindow::on_loginBtn_clicked()
{
    this->hide();  // 회원가입 창을 숨김

        loginWindow *loginWin = new loginWindow(networkManager, this);  // 로그인 창 생성
        loginWin->show();  // 로그인 창 표시
}

void JoinWindow::on_signupResponse(bool success) {
    if (success) {
        QMessageBox::information(this, "Success", "회원가입이 성공적으로 완료되었습니다.");
        accept(); // 대화상자를 닫음
    } else {
        QMessageBox::warning(this, "Error", "회원가입에 실패했습니다. 사용자 이름이 이미 사용 중일 수 있습니다.");
    }
}
