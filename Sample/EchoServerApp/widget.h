#ifndef WIDGET_H
#define WIDGET_H

#include <QLabel>
#include <QTcpServer>
#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);

private slots:
    void clientConnect( );                       /* 에코 서버 */
    void echoData( );

private:
    QLabel *infoLabel;
    QTcpServer *tcpServer;
};

#endif // WIDGET_H
