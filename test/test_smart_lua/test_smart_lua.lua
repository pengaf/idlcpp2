local holder_type = idlcpp.testsmart.TestHolder
local object_type = idlcpp.testsmart.TestObject
local lifecycle_type = idlcpp.testsmart.TestLifeCycle
local int_box_alias = idlcpp.testsmart.TestIntBox
local float_box_alias = idlcpp.testsmart.TestFloatBox
local int_box_type = int_box_alias
local float_box_type = float_box_alias
local holder = holder_type.create(0)
assert(holder ~= nil)

holder.primitiveField = 9
assert(holder.primitiveField == 9)

holder.nativeField = 609
assert(holder.nativeField == 609)

holder.noCodeField = 610
assert(holder.noCodeField == 610)

assert(#holder.fixedValues == 3)
assert(holder.fixedValues[0] == 10)
holder.fixedValues[1] = 202
assert(holder.fixedValues[1] == 202)

assert(#holder_type.staticFixedValues == 3)
holder_type.staticFixedValues[1] = 712
assert(holder_type.staticFixedValues[1] == 712)

assert(#holder.fixedProperty == 3)
assert(holder.fixedProperty[1] == 21)
holder.fixedProperty[1] = 221
assert(holder.fixedProperty[1] == 221)

assert(#holder.dynamicProperty == 3)
assert(holder.dynamicProperty[2] == 32)
holder.dynamicProperty[2] = 322
assert(holder.dynamicProperty[2] == 322)

assert(holder.listProperty[1] == 41)
holder.listProperty[1] = 411
assert(holder.listProperty[1] == 411)

assert(#holder_type.staticFixedProperty == 3)
assert(holder_type.staticFixedProperty[1] == 821)
holder_type.staticFixedProperty[1] = 921
assert(holder_type.staticFixedProperty[1] == 921)

assert(#holder_type.staticDynamicProperty == 3)
holder_type.staticDynamicProperty[2] = 932
assert(holder_type.staticDynamicProperty[2] == 932)

assert(holder_type.staticListProperty[1] == 841)
holder_type.staticListProperty[1] = 941
assert(holder_type.staticListProperty[1] == 941)

holder.primitiveProperty = 41
assert(holder.primitiveProperty == 41)
holder.nativeProperty = 641
assert(holder.nativeProperty == 641)
assert(holder:sumPrimitive(1) == 10)
assert(holder:makePrimitive() == 110)
assert(holder:nativeMethod(6) == 2006)

holder.valueField.x = 17
holder.valueField.y = 18
assert(holder.valueField.x == 17, "holder.valueField.x is " .. tostring(holder.valueField.x))
assert(holder.valueField.y == 18, "holder.valueField.y is " .. tostring(holder.valueField.y))

local value = holder.valueProperty
value.x = 7
value.y = 8
assert(value.x == 7, "value.x is " .. tostring(value.x))
assert(value.y == 8, "value.y is " .. tostring(value.y))
holder.valueProperty = value

local value_prop = holder.valueProperty
assert(value_prop.x == 7, "value_prop.x is " .. tostring(value_prop.x))
assert(value_prop.y == 8, "value_prop.y is " .. tostring(value_prop.y))

local large = holder.largeValueProperty
large.v0 = 70
large.v19 = 89
holder.largeValueProperty = large
local large_prop = holder.largeValueProperty
assert(large_prop.v0 == 70, "large_prop.v0 is " .. tostring(large_prop.v0))
assert(large_prop.v19 == 89, "large_prop.v19 is " .. tostring(large_prop.v19))

local observer = object_type.create(77)
assert(observer.id == 77)
holder.observerProperty = observer
assert(holder.observerProperty.id == 77)

local accepted = holder:acceptObserver(observer)
assert(accepted.id == 77)
assert(holder:makeObserver().id == 77)

local maybe = holder:maybeObserver(observer)
assert(maybe.id == 77)

local shared = holder_type.createSharedObject(101)
assert(shared.id == 101)

local sum, product = holder:splitPrimitive(6, 7)
assert(sum == 13, "sum is " .. tostring(sum))
assert(product == 42, "product is " .. tostring(product))

assert(holder_type.staticAdd(10, 32) == 42)
local static_sum, static_product = holder_type.staticSplitPrimitive(8, 9)
assert(static_sum == 17, "static_sum is " .. tostring(static_sum))
assert(static_product == 72, "static_product is " .. tostring(static_product))

holder_type.staticPrimitiveField = 901
assert(holder_type.staticPrimitiveField == 901)

holder_type.staticPrimitiveProperty = 902
assert(holder_type.staticPrimitiveProperty == 902)

holder_type.staticRefProperty = 903
assert(holder_type.staticRefProperty == 903)

assert(int_box_type ~= nil)
assert(float_box_type ~= nil)

assert(int_box_type.staticAdd(6, 7) == 13)

assert(float_box_type.staticAdd(3.0, 4.0) == 7.0)

lifecycle_type.resetCounters()
do
	local value = lifecycle_type.New(22)
	assert(value.id == 22)
	assert(value.serial == 1)
	assert(lifecycle_type.getLiveCount() == 1)
	assert(lifecycle_type.getCreatedCount() == 1)
	assert(lifecycle_type.getDestroyedCount() == 0)
	value = nil
	collectgarbage("collect")
	collectgarbage("collect")
	assert(lifecycle_type.getLiveCount() == 0)
	assert(lifecycle_type.getDestroyedCount() == 1)
end

lifecycle_type.resetCounters()
do
	local value = lifecycle_type(11)
	assert(value.id == 11)
	assert(value.serial == 1)
	assert(lifecycle_type.getLiveCount() == 1)
	assert(lifecycle_type.getCreatedCount() == 1)
	assert(lifecycle_type.getDestroyedCount() == 0)
	value = nil
	collectgarbage("collect")
	collectgarbage("collect")
	assert(lifecycle_type.getLiveCount() == 0)
	assert(lifecycle_type.getDestroyedCount() == 1)
end

lifecycle_type.resetCounters()
do
	local values = lifecycle_type.NewArray(3)
	assert(#values == 3)
	assert(values[0].id == -1)
	assert(values[0].serial == 1)
	assert(values[2].id == -1)
	assert(values[2].serial == 3)
	assert(lifecycle_type.getLiveCount() == 3)
	assert(lifecycle_type.getCreatedCount() == 3)
	assert(lifecycle_type.getDestroyedCount() == 0)
	values = nil
	collectgarbage("collect")
	collectgarbage("collect")
	assert(lifecycle_type.getLiveCount() == 0)
	assert(lifecycle_type.getDestroyedCount() == 3)
end
