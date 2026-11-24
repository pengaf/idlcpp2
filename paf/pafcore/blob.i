#import "Object.i"

#{
#include "utility.h"
#}

namespace pafcore
{
	class(blob_type) #PAFCORE_EXPORT Blob : Object
	{
		//blob_t toBlob__() const;
#{
	public:
		Blob(size_t size);
		//Blob(void* data, size_t length);
		~Blob();
	public:
		void* data() const
		{
			return m_data;
		}
		size_t size() const
		{
			return m_size;
		}
	protected:
		void* m_data;
		size_t m_size;
#}
	};
}

