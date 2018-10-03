//
// Created by kuusri on 30/09/2018.
//

#pragma once
#include <iostream>

#ifndef UNTITLED_MESSAGE_H
#define UNTITLED_MESSAGE_H

class Message {
    std::string sender;
    std::string body;
    std::string timestamp;
    inline static std::string delimiter = ";;;";
public:
    const std::string &getSender() const;

    void setSender(const std::string &sender);

    const std::string &getBody() const;
    void setBody(const std::string &body);
    const std::string &getTimestamp() const;
    void setTimestamp(const std::string &timestamp);

    int size();
    std::string serialize();
    static Message* deserialize(std::string s);
    static Message* create(std::string sender, std::string message);

};


#endif //UNTITLED_MESSAGE_H
