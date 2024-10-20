#include "rtpprocess.h"

rtpProcess* rtpProcess::instance() {
    static rtpProcess instance;
    return &instance;
}

rtpProcess::rtpProcess() {}

void rtpProcess::startFFmpegProcess() {
    ffmpegProcess = new QProcess();

    // FFmpeg 실행 경로 및 명령어 설정
    QString program = "C:/ffmpeg/ffmpeg-n5.1-latest-win64-gpl-shared-5.1/bin/ffmpeg.exe";
    QStringList arguments;

    // 멀티캐스트 IP 주소와 포트 설정
    arguments << "-f" << "dshow"
              << "-i" << "video=Integrated Webcam"   // 웹캠 이름
              << "-vcodec" << "libx264"
              << "-preset" << "ultrafast"
              << "-tune" << "zerolatency"
              << "-b:v" << "1M"           // 2Mbps 대역폭
              << "-s" << "640x480"          // 해상도 설정
              << "-f" << "rtp"
              << "rtp://239.255.0.1:5000"           // 멀티캐스트 주소와 포트
              << "-sdp_file" << "stream.sdp";       // SDP 파일 생성

    // 로그 파일 설정
    ffmpegProcess->setStandardOutputFile("ffmpeg_output.log");
    ffmpegProcess->setStandardErrorFile("ffmpeg_error.log");

    // FFmpeg 실행
    ffmpegProcess->start(program, arguments);

    if (!ffmpegProcess->waitForStarted()) {
        qDebug() << "FFmpeg 실행 실패: " << ffmpegProcess->errorString();
    } else {
        qDebug() << "FFmpeg 멀티캐스트 스트리밍 시작 중...";
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
