#import "object.i"

namespace pafcore
{
#{
	class InstanceProperty;
#}
	class(noncopyable) #PAFCORE_EXPORT DynamicObject : Object
	{
#{
	public:

		virtual uint32_t dynamicInstancePropertyCount() const
		{
			return 0;
		}

		virtual InstanceProperty* dynamicInstanceProperty(uint32_t index) const
		{
			return 0;
		}

#}
	};
}