#pragma once

namespace orbit
{

    static constexpr size_t operator ""_KiB(size_t in) { return in * 1024; }
	static constexpr size_t operator ""_KB(size_t in)  { return in * 1000; }
	static constexpr size_t operator ""_MiB(size_t in) { return in * 1024 * 1024; }
	static constexpr size_t operator ""_MB(size_t in)  { return in * 1000 * 1000; }

}