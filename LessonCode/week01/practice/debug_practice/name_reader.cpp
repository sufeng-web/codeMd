#include "name_reader.h"
#include <iostream>
#include <cstring>

NameReader::NameReader() {
    m_name[0] = '\0';
}

bool NameReader::readName() {
    std::cout << "Enter student name: ";
    char temp[kNameCapacity];
    if (!std::cin.getline(temp, kNameCapacity)) {
        return false;
    }
    std::size_t len = std::strlen(temp);
    for (std::size_t i = 0; i < len; ++i) {
        m_name[i] = temp[i];
    }
    return true;
}
