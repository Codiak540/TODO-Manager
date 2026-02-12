#ifndef BOXES_H
#define BOXES_H

#include <string>
#include <iostream>
#include <vector>
#define PADDING 2

class boxes {
public:
    static u_long padding(u_long length, u_long size);
    static std::string box(std::string header, const std::string& body);
    static std::string box(std::string header, const std::vector<std::string>& contents, const std::string& bodyColor, const std::string& barColor);
    static std::string box(std::string header, const std::vector<std::string>& contents);
    static std::string spacedContent(const std::string& toSpace, u_long size);
    static std::string namedHeader(const std::string& toSpace, u_long size);
    static std::string header(u_long length);
    static std::string footer(u_long size);
    static std::string indent(const std::string& text, const std::string& prefix);
};

#endif