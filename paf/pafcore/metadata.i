#import "object.i"

namespace pafcore
{
#{
	enum class ErrorCode
	{
		s_ok,
		e_invalid_namespace,
		e_name_conflict,
		e_null_variant,
		e_is_not_type,
		e_is_not_class,
		e_invalid_subscript_type,
		e_member_not_found,
		e_index_out_of_range,
		e_is_not_scalar_property,
		e_is_not_collection_property,
		e_is_not_array_property,
		e_is_not_dynamic_array_property,
		e_is_not_list_property,
		e_property_is_not_readable,
		e_property_is_not_writable,
		e_property_is_not_iterable,
		e_property_is_not_enumerable,
		e_field_is_not_scalar,
		e_field_is_not_array,
		e_invalid_type,
		e_invalid_object_type,
		e_invalid_field_type,
		e_invalid_property_type,
		e_invalid_arg_num,
		e_no_match_overload,
		e_ambiguous_overload,
		e_invalid_this_type,
		e_invalid_arg_type_1,
		e_invalid_arg_type_2,
		e_invalid_arg_type_3,
		e_invalid_arg_type_4,
		e_invalid_arg_type_5,
		e_invalid_arg_type_6,
		e_invalid_arg_type_7,
		e_invalid_arg_type_8,
		e_invalid_arg_type_9,
		e_invalid_arg_type_10,
		e_invalid_arg_type_11,
		e_invalid_arg_type_12,
		e_invalid_arg_type_13,
		e_invalid_arg_type_14,
		e_invalid_arg_type_15,
		e_invalid_arg_type_16,
		e_invalid_arg_type_17,
		e_invalid_arg_type_18,
		e_invalid_arg_type_19,
		e_invalid_arg_type_20,
		e_not_implemented,
		e_script_error,
		e_script_dose_not_override,
	};

	extern const char* g_errorStrings[];

	PAFCORE_EXPORT const char* ErrorCodeToString(ErrorCode errorCode);

	const size_t max_parameter_count = 20;

	struct Attribute
	{
		const char* name;
		const char* content;
	};
	struct Attributes
	{
		size_t count;
		Attribute* attributes;
	};
#}
	enum class MetadataKind ##: uint8_t
	{
		primitive_instance,
		enum_instance,
		class_instance,
		enum_member,
		instance_field,
		static_field,
		instance_property,
		static_property,
		instance_method,
		static_method,
		function_parameter,
		function_result,
		primitive_type,
		enum_type,
		class_type,
		type_alias,
		name_space,
		//dummy
		dummy_metadata,
		dummy_type,
	};

	enum class TypeCompound ## : uint8_t
	{
		none,
		ref,
		raw_ptr,
		shared_ptr,
		observer_ptr,
		raw_array,
		shared_array,
		observer_array,
	};

	enum class PropertyKind ## : uint8_t
	{
		scalar_property,
		fixed_array_property,
		dynamic_array_property,
		list_property,
	};

	class(dummy_metadata) #PAFCORE_EXPORT Metadata : Object
	{
		string_t _name_ { get };
		MetadataKind _kind_ { get };
		size_t _attributeCount_ { get };
		string_t _getAttributeName_(size_t index) const;
		string_t _getAttributeContent_(size_t index) const;
		string_t _getAttributeContentByName_(string_t attributeName) const;
		bool _hasAttribute_(string_t attributeName) const;
#{
	public:
		Metadata(const char* name, Attributes* attributes = 0);
		//Metadata(const Metadata&) = default;
		//Metadata& operator=(const Metadata&) = default;
	public:
		bool operator < (const Metadata& arg) const;
	public:
		const char* m_name;
		Attributes* m_attributes;
#}
	};

#{

inline size_t Metadata::_attributeCount_() const
	{
		return m_attributes ? m_attributes->count : 0;
	}

inline string_t Metadata::_name_() const
	{
		return m_name;
	}

	class CompareMetaDataPtrByName
	{
	public:
		bool operator()(const Metadata* m1, const Metadata* m2);
	};

#}

}
