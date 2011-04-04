#ifndef UTILS_H
#define UTILS_H

inline QString QStr(const std::string &s)
{
    return QString().fromStdString(s);
}


#endif // UTILS_H
