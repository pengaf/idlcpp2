#import "object.i"
#import "notify_handler_list.i"
#import "iterator.i"

#{
#include "utility.h"
#}

namespace pafcore
{
	enum class PropertyChangedFlag
	{
		update, //property changed; iterator points to the changed item for container property, otherwise null
		remove, //before the container removes an element, the iterator points to that element.
		add, //add an item to a container property; iterator points to the newly added item.
		reset,//reset; for container property with multiple items changed, iterator is null
		candidates //the candidates for the property has changed (not the property value itself)
	};

	interface #PAFCORE_EXPORT NotifyHandler : Interface
	{
		virtual void onDestroyNotifyHandlerList(Object* sender);
		virtual void onAttachNotifySource(Object* sender);
		virtual void onDetachNotifySource(Object* sender);
	};

	class #PAFCORE_EXPORT PropertyChangedNotifyHandler : NotifyHandler
	{
		virtual void onPropertyChanged(Object* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator);
		virtual void onPropertyAvailabilityChanged(Object* sender, string_t propertyName);
		virtual void onDynamicPropertyChanged(Object* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator);
		virtual void onUpdateDynamicProperty(Object* sender);
	};
}
