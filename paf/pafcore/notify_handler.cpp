#include "notify_handler.h"
#include "notify_handler.mh"
#include "notify_handler.ic"
#include "notify_handler.mc"

BEGIN_PAFCORE

void PropertyChangedNotifyHandler::onPropertyChanged(Reference* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator)
{

}

void PropertyChangedNotifyHandler::onPropertyAvailabilityChanged(Reference* sender, string_t propertyName)
{

}

void PropertyChangedNotifyHandler::onDynamicPropertyChanged(Reference* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator)
{

}

void PropertyChangedNotifyHandler::onUpdateDynamicProperty(Reference* sender)
{
}

END_PAFCORE
