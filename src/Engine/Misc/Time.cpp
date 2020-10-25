#include "Engine/Misc/Time.hpp"

namespace orbit
{

	Time::Time(int64_t micros) : 
		_micros(micros)
	{
	}

	int64_t Time::asMicroseconds()
	{
		return _micros;
	}

	int32_t Time::asMilliseconds()
	{
		return static_cast<int32_t>(_micros / 1000);
	}

	double Time::asSeconds()
	{
		return static_cast<double>(_micros) / (1000 * 1000.);
	}

	int64_t Clock::now()
	{
		return std::chrono::duration_cast<TTarget>(TClock::now().time_since_epoch()).count();
	}

	Clock::Clock() :
		_paused(false),
		_pause(0),
		_begin(0)
	{
		Restart();
	}

	Clock::Clock(int64_t us) :
		Clock()
	{
		_begin = now() - us;
	}

	void Clock::Pause()
	{
		_paused = true;
		_pause = now();
	}

	void Clock::Unpause()
	{
		if (_paused)
		{
			_begin += now() - _pause;
			_paused = false;
		}
	}

	Time Clock::Restart()
	{
		auto elapsed = GetElapsedTime();
		_paused = false;
		_begin = now();
		return elapsed;
	}

	Time Clock::GetElapsedTime() const
	{
		if (_paused)
			return Time(_pause - _begin);

		return Time(now() - _begin);
	}

}