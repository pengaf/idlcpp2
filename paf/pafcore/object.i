#import "reference.i"

namespace pafcore
{
	class(noncopyable) #PAFCORE_EXPORT Object : Reference
	{
#{
	public:
		virtual bool serializable() const
		{
			return true;
		}

		virtual bool editable() const
		{
			return true;
		}
		
		virtual bool propertySerializable(const char* propertyName) const
		{
			return true;
		}

		virtual bool propertyEditable(const char* propertyName) const
		{
			return true;
		}

#}
	};
}