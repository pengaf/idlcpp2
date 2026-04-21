local holder_type = idlcpp.testsmart.TestHolder
local object_type = idlcpp.testsmart.TestObject

local holder = holder_type.New()
assert(holder ~= nil)

holder.primitiveProperty = 41
assert(holder.primitiveProperty == 41)
assert(holder:sumPrimitive(1) == 2)
assert(holder:makePrimitive() == 102)

local value = holder.valueProperty
value.x = 7
value.y = 8

local value_result = holder:addValue(value)
assert(value_result.x == 9)
assert(value_result.y == 11)

holder.valueProperty = value
local value_prop = holder.valueProperty
assert(value_prop.x == 7)
assert(value_prop.y == 8)

local object = object_type.New(123)
holder.observerProperty = object
assert(holder.observerProperty.id == 123)
assert(holder:acceptObserver(object).id == 123)
assert(holder:acceptShared(object).id == 123)
assert(holder.sharedField.id == 123)
assert(holder.sharedProperty.id == 13)

local shared_result = holder:makeShared()
assert(shared_result.id == 99)

assert(holder_type.staticAdd(10, 32) == 42)
