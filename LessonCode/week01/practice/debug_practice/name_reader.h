#ifndef NAME_READER_H
#define NAME_READER_H

class NameReader {
public:
    static const int kNameCapacity = 64;

    NameReader();
    bool readName();
    const char* getName() const { return m_name; }

private:
    char m_name[kNameCapacity];
};

#endif
