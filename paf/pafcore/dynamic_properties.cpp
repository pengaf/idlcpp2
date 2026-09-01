#include "dynamic_properties.h"
#include "dynamic_properties.mh"
#include "dynamic_properties.ic"
#include "dynamic_properties.mc"

BEGIN_PAFCORE

uint32_t DynamicProperties::dynamicPropertyCount() const
{
	return 0;
}

InstanceProperty* DynamicProperties::dynamicProperty(uint32_t index) const
{
	return nullptr;
}

END_PAFCORE
