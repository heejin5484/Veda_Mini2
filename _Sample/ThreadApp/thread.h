#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class Thread : public QThread
{
    Q_OBJECT
public:
    enum { Stop, Play };
    explicit Thread(QObject *obj = nullptr);
};

#endif // THREAD_H
