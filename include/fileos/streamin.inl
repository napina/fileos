// Copyright (C) Ville Ruusutie, 2008

#pragma once
#ifndef fileos_streamin_inl
#define fileos_streamin_inl

namespace fileos {

inline size_t StreamIn::readLine(char* buffer, size_t bufferSize)
{
    size_t readCount = 0;
    char* buf = (char*)buffer;
    char ch;
    while(!isEos() && (readCount < bufferSize)) {
        read(ch);
        if(ch == 0x0d) {
            read(ch);
            // todo test if 0x0a
            break;
        }
        *buf = ch;
        ++buf;
        ++readCount;
    }
    *buf = 0;
    return readCount;
}
/*
inline void StreamIn::readSizeString(k::String& result)
{
    size_t size = read<uint32>();
    //result.reserve(size + 1);
    read(const_cast<char*>(result.c_str()), size);
}*/

template<typename T>
inline T StreamIn::read()
{
    T value;
    read(&value, sizeof(T));
    return value;
}

template<typename T>
inline void StreamIn::read(T& value)
{
    read(&value, sizeof(T));
}

} // fileos namespace

#endif
