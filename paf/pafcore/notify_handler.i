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

	override class #PAFCORE_EXPORT NotifyHandler : Object
	{
		//override virtual void onDestroyNotifyHandlerList(NotifyHandlerList* sender);
		//override virtual void onAttachNotifySource(NotifyHandlerList* sender);
		//override virtual void onDetachNotifySource(NotifyHandlerList* sender);
	};

	override class #PAFCORE_EXPORT PropertyChangedNotifyHandler : NotifyHandler
	{
		override virtual void onPropertyChanged(Object* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator);
		override virtual void onPropertyAvailabilityChanged(Object* sender, string_t propertyName);
		override virtual void onDynamicPropertyChanged(Object* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator);
		override virtual void onUpdateDynamicProperty(Object* sender);
	};
}
