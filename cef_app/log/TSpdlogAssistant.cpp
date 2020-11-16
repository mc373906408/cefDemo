#include "TSpdlogAssistant.h"

TSpdlogAssistant::TSpdlogAssistant(QObject *parent) : QObject(parent)
{

}

QString TSpdlogAssistant::getTypeName(FDJLogType type)
{
    switch (type) {
    case AV_TYPE:
        return "AV_Type";
    case UI_EVENTType:
        return "UI_EventType";
    case UI_CLICK:
        return "UI_Click";
    case NET_Type:
        return "NET_Type";
    case H5_Type:
        return "H5_Type";
    case Other_Type:
        return "Other_Type";
    default:
        return "";
    }
}
