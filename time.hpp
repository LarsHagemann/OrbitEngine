#pragma once
#include <chrono>

namespace orbit
{

	// @brief: Actually this is more a conversion tool
	//	from microseconds to milliseconds and seconds.
	struct Time
	{
	private:
		// @member: stored time in microseconds
		int64_t _micros;
	public:
		// expicit constructor
		explicit Time(int64_t ms);
		Time(const Time& other) = default;
		Time(Time&& other) = default;
		Time& operator=(const Time& other) = default;
		Time& operator=(Time&& other) = default;
		// @method: returns the stored time in microseconds (1000 * 1000us = 1s)
		int64_t asMicroseconds();
		// @method: returns the stored time in milliseconds (1000ms = 1s)
		int32_t asMilliseconds();
		// @method: returns the stored time in seconds
		//	1s = 1000ms
		//	1s = 1000 * 1000us
		double asSeconds();
	};

	class Clock
	{
	protected:
		using TTarget = std::chrono::microseconds;
		using TClock = std::chrono::high_resolution_clock;
		// @member: time when the clock "began" counting
		int64_t _begin;
		// @member: time that the clock has been paused
		int64_t _pause;
		// @member: is the clock currently paused?
		bool _paused;
	protected:
		// @method: returns the current time in microseconds
		static int64_t now();
	public:
		// initializes the clock
		Clock();
		// initializes the clock with a certain time
		// @param us: time in microseconds
		Clock(int64_t us);
		Clock(const Clock& other) = default;
		Clock(Clock&& other) = default;
		Clock& operator=(const Clock& other) = default;
		Clock& operator=(Clock&& other) = default;

		// @method: pauses the clock
		void Pause();
		// @method: resumes the clock
		void Unpause();
		// @method: restarts the clock
		// @brief: this clock resets the internal state so that
		//	it can be reused. If the clock was paused it will now be unpaused
		// @return: the elapsed time until the restart
		Time Restart();
		// @method: returns the currently elapsed time
		Time GetElapsedTime() const;
	};

}
