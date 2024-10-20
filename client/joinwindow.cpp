#include "joinwindow.h"
#include "ui_joinwindow.h"

JoinWindow::JoinWindow(NetworkManager *networkManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinWindow),
    networkManager(networkManager)
{
    ui->setupUi(this);
    connect(networkManager, &NetworkManager::responseReceived, this, &JoinWindow::on_signupResponse);
    connect(ui->togglePasswordButton, &QPushButton::clicked, this, &JoinWindow::togglePasswordVisibility);
    setupEmailDomainComboBox();
    this->setStyleSheet(
        "QPushButton {"
        "background-color: #4CAF50;"
        "color: white;"
        "border: none;"
        "border-radius: 5px;"
        "padding: 10px;"
        "font-size: 16px;"
        "}"
        "QLineEdit {"
        "border: 1px solid #ccc;"
        "border-radius: 5px;"
        "padding: 10px;"
        "font-size: 14px;"  // 텍스트 크기 (입력된 텍스트 크기)
        "}"
        "QLineEdit::placeholder {"
        "font-size: 12px;"  // 플레이스홀더 텍스트 크기
        "color: #888;"  // 플레이스홀더 텍스트 색상 (선택 사항)
        "}"
        "QMessageBox {"
        "font-size: 14px;"
        "}"
    );
    // 플레이스홀더 텍스트
    ui->nameLineEdit->setPlaceholderText("이름을 입력하세요");
    ui->phoneLineEdit->setPlaceholderText("전화번호를 입력하세요");
    ui->emailLineEdit->setPlaceholderText("이메일을 입력하세요");
    ui->idLineEdit->setPlaceholderText("사용자 ID를 입력하세요");
    ui->pwLineEdit->setPlaceholderText("비밀번호를 입력하세요");
    ui->confirmPwLineEdit->setPlaceholderText("비밀번호 확인");

        // 비밀번호 조건 표시
    ui->passwordStrengthLabel->setText("비밀번호는 8자리 이상, 특수문자 포함해야 합니다.");
    ui->passwordStrengthLabel->setStyleSheet("color: #888;");

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
        json["userid"] = userid; // 사용자 아이디디
        json["password"] = password; // 비밀번호

        networkManager->sendMessage(json);  // JSON 객체를 서버에 전송

}

void JoinWindow::setupEmailDomainComboBox() {
    // 이메일 도메인 목록
    QStringList emailDomains = {"@gmail.com", "@yahoo.com", "@outlook.com", "@naver.com", "@daum.net"};

    // QComboBox에 도메인 목록 추가
    ui->emailDomainComboBox->addItems(emailDomains);

    // 도메인 선택 시, 선택한 도메인을 이메일에 자동으로 추가
    connect(ui->emailDomainComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &JoinWindow::onDomainSelected);
}

void JoinWindow::onDomainSelected(int index) {
    // 사용자 이메일 입력란
    QString userEmail = ui->emailLineEdit->text();

    // 이메일 입력란에 도메인 추가 (기존에 입력된 이메일 앞에 추가)
    QString domain = ui->emailDomainComboBox->currentText();
    ui->emailLineEdit->setText(userEmail.split('@').first() + domain);
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

void JoinWindow::togglePasswordVisibility() {
    // 비밀번호 가시성 전환
    if (ui->pwLineEdit->echoMode() == QLineEdit::Password) {
        ui->pwLineEdit->setEchoMode(QLineEdit::Normal);  // 비밀번호 보이기
        ui->confirmPwLineEdit->setEchoMode(QLineEdit::Normal);  // 비밀번호 확인 보이기
    } else {
        ui->pwLineEdit->setEchoMode(QLineEdit::Password);  // 비밀번호 숨기기
        ui->confirmPwLineEdit->setEchoMode(QLineEdit::Password);  // 비밀번호 확인 숨기기
    }
}


void JoinWindow::on_signupResponse(bool success, const QString &message) {
    if (success) {
        QMessageBox::information(this, "Success", "회원가입이 완료되었습니다.");
        accept(); // 대화상자 닫기
    } else {
        QMessageBox::warning(this, "Failure", message); // 실패 메시지 표시
    }
}
