#include "clientitem.h"

#include <QLabel>
#include <iostream>

using namespace std;

ClientItem::ClientItem(int id, string name, string phoneNumber, string address)
    : m_id(id), m_name(name), m_phoneNumber(phoneNumber), m_address(address)
{
    labelName = new QLabel(name.c_str(), this);
    labelName->setGeometry(40, 0, 60, 18);

    labelNumber = new QLabel(phoneNumber.c_str(), this);
    labelNumber->setGeometry(100, 0, 100, 18);

    labelAddress = new QLabel(address.c_str(), this);
    labelAddress->setGeometry(200, 0, 200, 18);
}

ClientItem::~ClientItem()
{
    if(labelName != nullptr)
        delete labelName;
    if(labelNumber != nullptr)
        delete labelNumber;
    if(labelAddress != nullptr)
        delete labelAddress;
}

string ClientItem::getName() const
{
    return m_name;
}

void ClientItem::setName(string& name)
{
    m_name = name;
    if(labelName != nullptr)
        labelName->setText(name.c_str());
}

string ClientItem::getPhoneNumber() const
{
    return m_phoneNumber;
}

void ClientItem::setPhoneNumber(string& phoneNumber)
{
    m_phoneNumber = phoneNumber;
    if(labelNumber != nullptr)
        labelNumber->setText(phoneNumber.c_str());
}

string ClientItem::getAddress() const
{
    return m_address;
}

void ClientItem::setAddress(string& address)
{
    m_address = address;
    if(labelAddress != nullptr)
        labelAddress->setText(address.c_str());
}

int ClientItem::id() const
{
    return m_id;
}

// Define copy assignment operator.
bool ClientItem::operator==(const ClientItem &other) const {
    return (this->m_name == other.m_name);
}
