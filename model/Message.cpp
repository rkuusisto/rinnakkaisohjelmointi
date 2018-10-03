//
// Created by kuusri on 30/09/2018.
//

#include "../stdafx.h"
#include "Message.h"
#include "../util.h"

using namespace std;

const string &Message::getBody() const {
    return body;
}

void Message::setBody(const string &body) {
    Message::body = body;
}

const string &Message::getTimestamp() const {
    return timestamp;
}

void Message::setTimestamp(const string &timestamp) {
    Message::timestamp = timestamp;
}

const string &Message::getSender() const {
    return sender;
}

void Message::setSender(const string &sender) {
    Message::sender = sender;
}

int Message::size() {
    return strlen(sender.c_str()) + strlen(body.c_str()) + strlen(timestamp.c_str());
}

std::string Message::serialize() {

    std::string out;

    out += sender + delimiter;
    out += body + delimiter;
    out += timestamp + delimiter;

    return out;
}

Message *Message::deserialize(std::string s) {

    auto *m = new Message();

    string temp[3];

    size_t pos = 0;
    int i = 0;

    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        temp[i++] = s.substr(0, pos);
        s.erase(0, pos + delimiter.length());
    }

    m->sender = temp[0];
    m->body = temp[1];
    m->timestamp = temp[2];

    return m;
}

Message *Message::create(std::string sender, std::string message) {
    auto *m = new Message;
    m->setBody(message);
    m->setSender(sender);
    m->setTimestamp(util::getTime());
    return m;
}
