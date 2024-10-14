#include "joinwindow.h"
#include "ui_joinwindow.h"

JoinWindow::JoinWindow(NetworkManager *networkManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinWindow),
    networkManager(networkManager)
{
    ui->setupUi(this);
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
    QString confirmPassword = ui->confirmPwLineEdit->text();

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


bool JoinWindow::isValidPassword(const QString &password)
{
    QRegularExpression specialCharRegex("[!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]+");

    if (password.length() < 8) {
        return false;  // 8자 이상이어야 함
    }

    if (!specialCharRegex.match(password).hasMatch()) {
        return false;  // 특수 문자가 포함되어 있지 않음
    }

    return true;
}


void JoinWindow::on_signupResponse(bool success, const QString &message) {
    if (success) {
        QMessageBox::information(this, "Success", "회원가입이 완료되었습니다.");
        accept(); // 대화상자 닫기
    } else {
        QMessageBox::warning(this, "Failure", message); // 실패 메시지 표시
    }
}
