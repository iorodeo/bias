#ifndef BIAS_JSON_UTILS_HPP
#define BIAS_JSON_UTILS_HPP
#include <QByteArray>

namespace bias
{
    QByteArray prettyIndentJson(QByteArray jsonArray);
    void addIndentToByteArray(QByteArray &array, unsigned int width);
    void addNewLineToByteArray(QByteArray &array);
}

#endif // #ifndef BIAS_JSON_UTILS
