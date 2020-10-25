#pragma once

namespace orbit
{

	// @brief: this helper file makes it easier to work with flags.
	//	since it is recommended to use enum class over enum, bitwise 
	//	operations always involve a lot of static_casts and are 
	//	really bothersome. This templated header file tries to make flag
	//	operations as easy as possible (and a lot more readable)
	// @example: 
	//	the example creates a derived object class, that updates when a 
	//	specific update necessary flag is being set (maybe by another object
	//	or a certain interaction). It also needs to be initialized with some 
	//	flags being set. That code would look really messy, when using
	//	enum classes (as recommended) and bitwise operations (as necessary 
	//	with flags).
	// @code:
	//enum class ObjFlag
	//{
	//	Obj_None = 0,
	//	Obj_SimpleObject = (1 << 0),
	//	Obj_RenderObject = (1 << 1),
	//	Obj_EventObject = (1 << 2),
	//	Obj_PhysXObject = (1 << 3),
	//	// ...
	//
	//	Obj_UpdateNecessary = (1 << 16) // <-- some important flag that may change
	//};
	//
	//class MyComplexObject : public Object
	//{
	//public:
	//	MyComplexObject(bool isEventDriven)
	//	{
	//		orbit::set_flag(
	//			_flags,
	//			ObjFlag::Obj_RenderObject,
	//			ObjFlag::Obj_PhysXObject,
	//			isEventDriven ? ObjFlag::Obj_EventObject : ObjFlag::Obj_None
	//		);
	//		// instead of the following:
	//		//_flags =
	//		//	static_cast<ObjFlag>(
	//		//	static_cast<unsigned>(_flags) |
	//		//	static_cast<unsigned>(ObjFlag::Obj_RenderObject) |
	//		//	static_cast<unsigned>(ObjFlag::Obj_PhysXObject) |
	//		//	isEventDriven ? static_cast<unsigned>(ObjFlag::Obj_EventObject) : static_cast<unsigned>(ObjFlag::Obj_None)
	//		//);
	//		// we got rid of all the static_casts and our code became much more readable
	//	}
	//	void Update(Time dt) override
	//	{
	//		if (orbit::is_flag_set(_flags, ObjFlag::Obj_UpdateNecessary))
	//		{
	//			// Update...
	//		}
	//		// instead of the following:
	//		//if ((static_cast<unsigned>(_flags) & static_cast<unsigned>(ObjFlag::Obj_UpdateNeccessary)) 
	//		//	== static_cast<unsigned>(ObjFlag::Obj_UpdateNeccessary))
	//		//{
	//		//	// Update...
	//		//}
	//		// we got rid of all the static_casts and our code got much more readable
	//	}
	//};

	// @brief: sets specific flags
	//	template parameters will be automatically derived
	// @template _Flag_type: can be a enum class or integral type
	// @template _Flag_enum...: list of enum, enum class types or integral types (mixes are also allowed)
	// @param flag: pointer to a flag that should be changed
	// @param flags: flags to be set (can also be a single flag)
	template<typename _Flag_type, typename... _Flag_enum>
	void set_flag(_Flag_type* flag, _Flag_enum... flags)
	{
		const auto bitMask = ((1 << static_cast<size_t>(flags)) | ...);
		*flag = static_cast<_Flag_type>(static_cast<size_t>(*flag) | bitMask);
	}

	// @brief: unsets specific flags
	//	template parameters will be automatically derived
	// @template _Flag_type: can be a enum class or integral type
	// @template _Flag_enum...: list of enum, enum class types or integral types (mixes are also allowed)
	// @param flag: pointer to a flag that should be changed
	// @param flags: flags to be unset (can also be a single flag)
	template<typename _Flag_type, typename... _Flag_enum>
	void unset_flag(_Flag_type* flag, _Flag_enum... flags)
	{
		const auto bitMask = ((1 << static_cast<size_t>(flags)) | ...);
		*flag = static_cast<_Flag_type>(static_cast<size_t>(*flag) & ~bitMask);
	}

	// @brief: tests whether a certain flag is set
	//	template parameters will be automatically derived
	// @template _Flag_type: can be a enum, enum class or integral type
	// @template _Flag_enum: can be a enum, enum class or integral type
	// @param flag: flag to be checked
	// @param toCheck: flag bit to check against 
	// @return: true if the flag is set, false otherwise
	template<typename _Flag_type, typename _Flag_enum>
	bool is_flag_set(const _Flag_type& flag, _Flag_enum toCheck)
	{
		auto flag_ = static_cast<size_t>(flag);
		return (flag_ & static_cast<size_t>(toCheck)) == static_cast<size_t>(toCheck);
	}

	// @brief: tests if a list of flags is set
	//	template parameters will be automatically derived
	// @template _Flag_type: can be a enum, enum class or integral type
	// @template _Flag_enum...: list of enum, enum class  types or integral types (mixes are also allowed)
	// @param flag: flag to be checked
	// @param flags: flags to be checked against
	// @return: true if all flags are set, false otherwise
	template<typename _Flag_type, typename... _Flag_enum>
	bool are_flags_set(const _Flag_type& flag, _Flag_enum... flags)
	{
		auto flag_ = static_cast<size_t>(flag);
		return (((flag_ & static_cast<size_t>(flags)) == static_cast<size_t>(flags)) && ...);

		// alternate implementation: the code above was up to 2x as performant in my benchmarks
		//	in release versions, the code above was even 3.3x times as fast
		//return ((is_flag_set(flag, flags)) && ...);
	}

}
