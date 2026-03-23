#ifndef __DEBUG_PRACTICE_NAME_READER_H__
#define __DEBUG_PRACTICE_NAME_READER_H__

class NameReader 
{
public:
    static const int s_nameCapacity = 64;

    NameReader();
    bool readName();
    const char* getName() const { return m_name; }

private:
    char m_name[s_nameCapacity];
};

#endif
