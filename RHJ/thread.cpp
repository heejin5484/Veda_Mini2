#include "thread.h"
#include <QDebug>

Thread::Thread(QObject *obj) : QThread(obj){
}

void Thread::run()
{
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Thread::onTimeout);

    timer->start(10000); // 10초마다 timeout
    exec();
}

void Thread::onTimeout(){
    qDebug() << "aa";
    emit alert();
}
