#include "json_utils.hpp"

namespace bias
{
    const unsigned int JSON_INDENT_STEP = 4;

    QByteArray prettyIndentJson(QByteArray jsonArray)
    {
        // Week and kludgey json pretty printer.
        QByteArray jsonArrayNew;
        unsigned int pos = 0;
        unsigned int indentLevel = 0;
        unsigned int indentStep = JSON_INDENT_STEP;
        unsigned int indent = 0;
        bool isInnerList = false;

        while (pos < jsonArray.size()) 
        {
            bool isSpecialChar = false;
            if (jsonArray[pos] == '}')
            {
                addNewLineToByteArray(jsonArrayNew);
                indentLevel -= 1;
                indent = indentLevel*indentStep;
                addIndentToByteArray(jsonArrayNew, indent);
                jsonArrayNew.append(jsonArray[pos]);
                isSpecialChar = true;
            }
            else if (jsonArray[pos] =='{') 
            {
                jsonArrayNew.append(jsonArray[pos]);
                addNewLineToByteArray(jsonArrayNew);
                indentLevel += 1;
                indent = indentLevel*indentStep;
                addIndentToByteArray(jsonArrayNew, indent);
                isSpecialChar = true;
            }
            else if (jsonArray[pos] == ',')
            {
                jsonArrayNew.append(jsonArray[pos]);
                if (!isInnerList)
                {
                    addNewLineToByteArray(jsonArrayNew);
                    indent = indentLevel*indentStep;
                    addIndentToByteArray(jsonArrayNew, indent);
                    isSpecialChar = true;
                }
            }
            else if (jsonArray[pos] == '[')
            {
                jsonArrayNew.append(jsonArray[pos]);
                for (unsigned int tempPos=pos+1; tempPos<jsonArray.size(); tempPos++)
                {
                    isInnerList = false;
                    if (jsonArray[tempPos] == ']')
                    {
                        isInnerList = true;
                        break;
                    }
                    if (jsonArray[tempPos] == '[')
                    {
                        isInnerList = false;
                        break;
                    }
                }
                if (!isInnerList)
                {
                    addNewLineToByteArray(jsonArrayNew);
                    indentLevel += 1;
                    indent = indentLevel*indentStep;
                    addIndentToByteArray(jsonArrayNew, indent);
                    isSpecialChar = true;
                }
            }
            else if (jsonArray[pos] == ']')
            {
                if (isInnerList)
                {
                    isInnerList = false;
                    jsonArrayNew.append(jsonArray[pos]);
                }
                else
                { 
                    addNewLineToByteArray(jsonArrayNew);
                    indentLevel -= 1;
                    indent = indentLevel*indentStep;
                    addIndentToByteArray(jsonArrayNew, indent);
                    jsonArrayNew.append(jsonArray[pos]);
                    isSpecialChar = true;
                }
            }
            else
            {
                jsonArrayNew.append(jsonArray[pos]);
            }


            if (isSpecialChar)
            {
                pos++;
                while (jsonArray[pos] == ' ')
                {
                    pos++;
                }

            }
            else
            {
                pos++;
            }
        }
        return jsonArrayNew;
    }

    // Old version
    // ------------------------------------------------------------------------
    //QByteArray prettyIndentJson(QByteArray jsonArray)
    //{
    //    // Week and kludgey json pretty printer.
    //    QByteArray jsonArrayNew;
    //    unsigned int pos = 0;
    //    unsigned int indentLevel = 0;
    //    unsigned int indentStep = JSON_INDENT_STEP;
    //    unsigned int indent = 0;

    //    while (pos < jsonArray.size()) 
    //    {
    //        bool isSpecialChar = false;
    //        if ((jsonArray[pos] == '}') || (jsonArray[pos] == ']'))
    //        {
    //            addNewLineToByteArray(jsonArrayNew);
    //            indentLevel -= 1;
    //            indent = indentLevel*indentStep;
    //            addIndentToByteArray(jsonArrayNew, indent);
    //            jsonArrayNew.append(jsonArray[pos]);
    //            isSpecialChar = true;
    //        }
    //        else if ((jsonArray[pos] =='{') || (jsonArray[pos] == '[') )
    //        {
    //            jsonArrayNew.append(jsonArray[pos]);
    //            addNewLineToByteArray(jsonArrayNew);
    //            indentLevel += 1;
    //            indent = indentLevel*indentStep;
    //            addIndentToByteArray(jsonArrayNew, indent);
    //            isSpecialChar = true;
    //        }
    //        else if (jsonArray[pos] == ',')
    //        {
    //            jsonArrayNew.append(jsonArray[pos]);
    //            addNewLineToByteArray(jsonArrayNew);
    //            indent = indentLevel*indentStep;
    //            addIndentToByteArray(jsonArrayNew, indent);
    //            isSpecialChar = true;
    //        }
    //        else
    //        {
    //            jsonArrayNew.append(jsonArray[pos]);
    //        }


    //        if (isSpecialChar)
    //        {
    //            pos++;
    //            while (jsonArray[pos] == ' ')
    //            {
    //                pos++;
    //            }

    //        }
    //        else
    //        {
    //            pos++;
    //        }
    //    }
    //    return jsonArrayNew;
    //}


    void addIndentToByteArray(QByteArray &array, unsigned int width)
    {
        for (unsigned int i=0; i<width; i++)
        {
            array.append(' ');
        }
    }


    void addNewLineToByteArray(QByteArray &array)
    {
#ifdef WIN32
        array.append('\r');
#endif
        array.append('\n');
    }


} // namespace bias
