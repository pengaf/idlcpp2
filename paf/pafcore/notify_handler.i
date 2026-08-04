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
		update, //����ֵ�����������������ԣ��� iterator ָ������, ����Ϊnull
		remove, //���������Ƴ�һ��֮ǰ��iterator ָ�򼴽��Ƴ�����
		add, //������������һ��֮��iterator ָ������ӵ���
		reset,//�������Զ����任��iterator Ϊ null
		candidate_list //���Ժ�ѡ�б���� (��������ֵ���)
	};

	class #PAFCORE_EXPORT NotifyHandler : Object
	{
		virtual void onDestroyNotifyHandlerList(NotifyHandlerList* sender);
		virtual void onAttachNotifySource(NotifyHandlerList* sender);
		virtual void onDetachNotifySource(NotifyHandlerList* sender);
	};

	class #PAFCORE_EXPORT PropertyChangedNotifyHandler : NotifyHandler
	{
		virtual void onPropertyChanged(Object* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator);
		virtual void onPropertyAvailabilityChanged(Object* sender, string_t propertyName);
		virtual void onDynamicPropertyChanged(Object* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator);
		virtual void onUpdateDynamicProperty(Object* sender);
	};
}
