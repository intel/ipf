/******************************************************************************
** Copyright (c) 2020-2026 Intel Corporation All Rights Reserved
**
** Licensed under the Apache License, Version 2.0 (the "License"); you may not
** use this file except in compliance with the License.
**
** You may obtain a copy of the License at
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
** WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**
** See the License for the specific language governing permissions and
** limitations under the License.
**
******************************************************************************/

#pragma once

#include "Exceptions.h"
#include "esif_ccb.h"
#include "esif_ccb_random.h"
#ifdef _WIN32
#pragma comment(lib, "bcrypt")
#endif

#include <limits>
#include <mutex>

namespace Ipf
{

// Class version of handle generator from IPF Core (avoids linking issues with translation unit
// rules for C++ code)
class HandleGenerator
{

public:
	/// @brief Validate handle generation parameters at compile time. Use this in contexts where
	/// exceptions are unacceptable to guarantee no exceptions.
	/// @note See ClientApiC.cpp for an example of using this in a compile-time context.
	static constexpr bool ValidateParameters(
		esif_handle_t handle_min,
		esif_handle_t handle_max,
		esif_handle_t handle_seed,
		esif_handle_t handle_inc)
	{
		// Required ordering: handle_inc < handle_seed < handle_min < handle_max
		if (!(handle_inc < handle_seed && handle_seed < handle_min && handle_min < handle_max)) {
			return false;
		}

		// In the worst case the rollover path in Generate() returns
		// (handle_min + handle_seed + handle_inc), so that sum must stay strictly below handle_max
		// to keep every generated handle inside (handle_min, handle_max). The checks are written as
		// subtractions so the 64-bit additions cannot overflow for ranges near the top of the
		// address space. handle_seed < handle_max guarantees the first subtraction cannot underflow.
		const esif_handle_t max_minus_seed = handle_max - handle_seed;
		if (max_minus_seed <= handle_min) {
			// handle_min + handle_seed >= handle_max
			return false;
		}

		// handle_min + handle_seed + handle_inc < handle_max
		if ((max_minus_seed - handle_min) <= handle_inc) {
			return false;
		}

		// The steady-state path in Generate() computes (handle_factory_ + increment), where
		// handle_factory_ can be as large as (handle_max - 1) and increment as large as handle_inc.
		// If that addition overflows 64 bits it wraps to a small value that escapes the rollover
		// check and can fall below handle_min, so reject ranges where it could overflow. Written as
		// a subtraction so the bound itself cannot overflow. The max() call is parenthesized to
		// avoid the Windows min/max function-like macros.
		return handle_max - 1 <= (std::numeric_limits<esif_handle_t>::max)() - handle_inc;
	}

	/// @brief Construct a handle generator with the given parameters.
	///
	/// @param handle_min Min handle before random seed
	/// @param handle_max Max handle before rollover
	/// @param handle_seed Handle initial seed random mask
	/// @param handle_inc Handle increment random mask
	/// @throws InvalidArgumentException if parameters are invalid
	HandleGenerator(
		esif_handle_t handle_min,
		esif_handle_t handle_max,
		esif_handle_t handle_seed,
		esif_handle_t handle_inc)
		: handle_min(handle_min), handle_max(handle_max), handle_seed(handle_seed),
		  handle_inc(handle_inc)
	{
		if (!ValidateParameters(handle_min, handle_max, handle_seed, handle_inc)) {
			throw InvalidArgumentException("Invalid handle generation parameters");
		}
	}

	/// @brief Generate a unique handle within the range (handle_min+1) and (handle_max-1).
	///
	/// Seed initial handle with a random value between (handle_min+1) and (handle_min+handle_seed).
	/// Increment additional handles by a random value between 1 and handle_inc until rollover.
	///
	/// This effectively creates a handle pool of size
	/// ((handle_max-handle_min-handle_seed)/handle_inc) or ~140 trillion handles or ~4000 years
	/// worth of handles (before rollover) if generated every second.
	///
	/// @return A unique handle value
	esif_handle_t Generate()
	{
		// Generate Random Handle Seed/Increment values
		esif_handle_t seed = 1;
		esif_handle_t increment = 1;
		if (esif_ccb_random(&seed, sizeof(seed)) == ESIF_OK) {
			seed = (seed & handle_seed) + !(seed & handle_seed);
			increment = (seed & handle_inc) + !(seed & handle_inc);
		}

		std::lock_guard lock(factory_mutex_);
		esif_handle_t handle = (handle_factory_ ? handle_factory_ + increment : handle_min + seed);
		if (handle >= handle_max) {
			handle = handle_min + handle_seed + increment;
		}
		handle_factory_ = handle;

		return handle;
	}

private:
	const esif_handle_t handle_min;
	const esif_handle_t handle_max;
	const esif_handle_t handle_seed;
	const esif_handle_t handle_inc;

	std::mutex factory_mutex_{};
	esif_handle_t handle_factory_ = 0;
};

} // namespace Ipf
