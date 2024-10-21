#include "rtpprocess.h"
#include <QDir>

rtpProcess* rtpProcess::instance() {
    static rtpProcess instance;
    return &instance;
}

rtpProcess::rtpProcess() {}

void rtpProcess::startFFmpegProcess() {
    ffmpegProcess = new QProcess();

    #if 1 // window환경
    // FFmpeg 실행 경로 및 명령어 설정
    QString program = QDir::currentPath() + "/bin/ffmpeg.exe";
    qDebug() << program;
    QStringList arguments;
    #else // linux환경
    // 추가필요
    #endif

    // 멀티캐스트 IP 주소와 포트 설정
    arguments << "-f" << "dshow"
              << "-i" << "video=Integrated Webcam" // 웹캠 입력
              << "-vf" << "format=yuv420p"
              << "-s" << "640x480"
              << "-map" << "0:v"                // 첫 번째 출력:표준출력으로 매핑
              << "-pix_fmt" << "rgb24"          // 픽셀 포맷을 RGB로 설정
              << "-f" << "rawvideo"             //첫 번째 출력은 raw 비디오 형식으로
              << "pipe:1"                       //표준출력으로 전송
              << "-map" << "0:v"                //두 번째 출력: RTP 스트리밍
              << "-vcodec" << "libx264"         //H.264 인코딩
              << "-preset" << "ultrafast"       //인코딩 속도 최적화
              << "-tune" << "zerolatency"       //실시간 스트리밍 위한 저지연 설정
              << "-b:v" << "1M"                 //비트레이트 설정
              << "-f" << "rtp"                  // 두 번째 출력은 RTP형식으로 전송
              << "rtp://239.255.0.1:5000";      //rtp 멀티캐스트 주소
              //<< "-sdp_file" << "stream.sdp";        // SDP 파일 생성

    // 로그 파일 설정
    //ffmpegProcess->setStandardOutputFile("ffmpeg_output.log");
    //ffmpegProcess->setStandardErrorFile("ffmpeg_error.log");

    // 신호와 슬롯 연결: FFmpeg의 표준 출력이 준비될 때 processFFmpegOutput이 호출되도록 설정
    connect(ffmpegProcess, &QProcess::readyReadStandardOutput, this, &rtpProcess::processFFmpegOutput);

    // FFmpeg 실행
    ffmpegProcess->start(program, arguments);

    if (!ffmpegProcess->waitForStarted()) {
        qDebug() << "FFmpeg 실행 실패: " << ffmpegProcess->errorString();
    } else {
        qDebug() << "FFmpeg 멀티캐스트 스트리밍 시작 중...";
    }
}

void rtpProcess::processFFmpegOutput() {
    // raw 비디오 데이터 읽기 (640x480 해상도, RGB 포맷)
    buffer.append(ffmpegProcess->readAllStandardOutput());  // FFmpeg 표준 출력에서 데이터 추가

    // RAW 비디오 프레임 크기 계산: 640x480 해상도, RGB24는 3바이트 (8비트) per pixel
    int width = 640;
    int height = 480;
    int frameSize = width * height * 3;  // RGB24 포맷이므로 픽셀당 3바이트

    while (buffer.size() >= frameSize) {
        // 비디오 프레임 데이터를 QImage로 변환 (RGB888 포맷)
        QImage frame(reinterpret_cast<const uchar*>(buffer.data()), width, height, QImage::Format_RGB888);

        // QImage를 QLabel에서 표시
        emit newFrameAvailable(frame);

        // 버퍼에서 처리한 프레임만큼의 데이터 제거
        buffer.remove(0, frameSize);
    }
}

void rtpProcess::stopFFmpegProcess() {
    if (ffmpegProcess && ffmpegProcess->state() == QProcess::Running) {
        ffmpegProcess->terminate();  // FFmpeg 종료 요청
        if (!ffmpegProcess->waitForFinished(5000)) {
            ffmpegProcess->kill();  // 강제 종료
        }
        qDebug() << "FFmpeg 프로세스가 종료되었습니다.";
    } else {
        qDebug() << "FFmpeg 프로세스가 실행 중이 아닙니다.";
    }
}
