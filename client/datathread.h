#ifndef DATATHREAD_H
#define DATATHREAD_H
#include <QThread>
#include <QTimer>


class datathread : public QThread
{
    Q_OBJECT
public:
    explicit datathread(QObject *obj = nullptr);
    void run() override;

signals:
    void alert();

private slots:
    void onTimeout();

private :
    QTimer *timer;
};

#endif // DATATHREAD_H
