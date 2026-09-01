#import "object.i"

namespace pafcore
{
#{
	class InstanceProperty;
#}
	class(noncopyable) #PAFCORE_EXPORT DynamicProperties : Interface
	{
		virtual uint32_t dynamicPropertyCount() const;
		virtual InstanceProperty* dynamicProperty(uint32_t index) const;
	};
}