#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QTimer>


class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(QObject *obj = nullptr);
    void run() override;

signals:
    void alert();

private slots:
    void onTimeout();

private :
    QTimer *timer;
};


#endif // THREAD_H
