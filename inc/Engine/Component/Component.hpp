#pragma once
#include <cstdint>
#include <memory>

namespace orbit
{

	class Object;
	class DebugObject;
	using ObjectPtr = std::shared_ptr<Object>;

	class Component
	{
	private:
		// @member: flags for special components
		//	every flag that is not used by the orbit engine (
		//	see list below) may be used by the user, though in the future
		//	bits 2 - 15 may be used in other ways, so it is recommended to
		//	first use flags 16 upwards or your own flag variable.
		//	bit 0: set by the Renderable class
		//	bit 1: set by the EventDriven class
		//	bits >= 16 may be used by the user for 
		//	special components
		uint32_t _flags;
		// @member: the object this component is bound to
		ObjectPtr _boundObject;
	protected:
		friend class Renderable;
		friend class EventDriven;
		// @method: sets a specific flag bit
		// @param bitToBeModified: the flag bit to be set
		void set_flag(size_t bitToBeModified);
		// @method: unsets a specific flag bit
		// @param bitToBeModified: the flag bit to be unset
		void unset_flag(size_t bitToBeModified);
		// @member: flag used to mark this component as 
		// 	a Renderable object
		static constexpr auto RenderableFlag = 0u;
		// @member: flag used to mark this component as
		// 	a EventDriven object
		static constexpr auto EventDrivenFlag = 1u;
	public:
		// @constructs a component connected to a certain object
		Component(ObjectPtr boundObject);
		virtual ~Component() {}
		// @method: returns the flag status of a certain flag
		// @param bitToCheck: the position of the bit to check for
		// @return: true if the bit was set, false otherwise
		bool GetFlag(size_t bitToCheck);
		// @method: renders a debug GUI
		virtual void DebugGUI(std::shared_ptr<DebugObject>) const {}
	};

	using ComponentPtr = std::shared_ptr<Component>;

}
