#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <thread>

namespace offset 
{
	// Client Offsets 
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDE997C;
	constexpr ::std::ptrdiff_t dwForceJump = 0x52BAC28;

	// Player Offsets 
	constexpr ::std::ptrdiff_t m_iHealth = 0x100;
	constexpr ::std::ptrdiff_t m_fFlags = 0x104;

}

// Hack Function
void BunnyHop(const HMODULE instance) noexcept 
{
	const auto client = reinterpret_cast<std::uintptr_t>(GetModuleHandle("client.dll"));

	// Loop
	while (!GetAsyncKeyState(VK_END))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));


		// Making sure space is pressed else does nothing
		if (!GetAsyncKeyState(VK_SPACE))
			continue;

		// get local player info
		const auto localPlayer = *reinterpret_cast<std::uintptr_t*>(client + offset::dwLocalPlayer);

		// Check if local player is valid 
		if (!localPlayer)
			continue;

		// get health value 
		const auto health = *reinterpret_cast<std::int32_t*>(localPlayer + offset::m_iHealth);

		// check if the player is actually alive and if so continue 
		if (!health)
			continue;

		const auto flags = *reinterpret_cast<std::int32_t*>(localPlayer + offset::m_fFlags);

		// check if on ground 
		(flags & (1 << 0)) ?
			*reinterpret_cast<std::uintptr_t*>(client + offset::dwForceJump) = 6 : //force jump
			*reinterpret_cast<std::uintptr_t*>(client + offset::dwForceJump) = 4; // reset jump
	}

	// Uninject 
	FreeLibraryAndExitThread(instance, 0);



}


// Entry Point For DLL 

int __stdcall DllMain(
	const HMODULE instance,
	const std::uintptr_t reason,
	const void* reserved
)

// THREAD_DLL_ATTACH 

{
	if (reason == 1) 
	{
		DisableThreadLibraryCalls(instance);

		//creating hack thread
		const auto thread = CreateThread(
			nullptr,
			0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(BunnyHop),
			instance,
			0,
			nullptr
		);

		if (thread)
			CloseHandle(thread);

	}


	return 1;
}