#ifndef CLIENTITEM_H
#define CLIENTITEM_H

#include <iostream>

using namespace std;

#include <QWidget>

class QLabel;

class ClientItem : public QWidget
{
    Q_OBJECT
public:
    explicit ClientItem(int id = 0, string = "", string = "", string = "");
    ~ClientItem();

    string getName() const;
    void setName(string&);
    string getPhoneNumber() const;
    void setPhoneNumber(string&);
    string getAddress() const;
    void setAddress(string&);
    int id() const;
    bool operator==(const ClientItem &other) const;

private:
    int m_id;
    string m_name;
    string m_phoneNumber;
    string m_address;

    QLabel* labelName;
    QLabel* labelNumber;
    QLabel* labelAddress;
};

#endif // CLIENTITEM_H
