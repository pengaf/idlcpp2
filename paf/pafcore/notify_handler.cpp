#include "notify_handler.h"
#include "notify_handler.mh"
#include "notify_handler.ic"
#include "notify_handler.mc"

BEGIN_PAFCORE

void PropertyChangedNotifyHandler::onPropertyChanged(ObserverPtr<Object> sender, string_t propertyName, PropertyChangedFlag flag, ObserverPtr<Iterator> iterator)
{

}

void PropertyChangedNotifyHandler::onPropertyAvailabilityChanged(ObserverPtr<Object> sender, string_t propertyName)
{

}

void PropertyChangedNotifyHandler::onDynamicPropertyChanged(ObserverPtr<Object> sender, string_t propertyName, PropertyChangedFlag flag, ObserverPtr<Iterator> iterator)
{

}

void PropertyChangedNotifyHandler::onUpdateDynamicProperty(ObserverPtr<Object> sender)
{
}

END_PAFCORE
