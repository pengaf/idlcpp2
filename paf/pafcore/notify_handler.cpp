#include "notify_handler.h"
#include "notify_handler.mh"
#include "notify_handler.ic"
#include "notify_handler.mc"

BEGIN_PAFCORE

void NotifyHandler::onDestroyNotifyHandlerList(Object* sender)
{
}

void NotifyHandler::onAttachNotifySource(Object* sender)
{
}

void NotifyHandler::onDetachNotifySource(Object* sender)
{
}


void PropertyChangedNotifyHandler::onPropertyChanged(Object* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator)
{

}

void PropertyChangedNotifyHandler::onPropertyAvailabilityChanged(Object* sender, string_t propertyName)
{

}

void PropertyChangedNotifyHandler::onDynamicPropertyChanged(Object* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator)
{

}

void PropertyChangedNotifyHandler::onUpdateDynamicProperty(Object* sender)
{
}

END_PAFCORE
