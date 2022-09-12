 
#include <Windows.h>
#include <Cstdint>

#include "Mono.h"

BOOL DllMain(void* hMod, std::uint32_t CallReason, [[maybe_unused]] void* Reserved)
{
	if(CallReason != DLL_PROCESS_ATTACH)
		return false;

	const auto Test = [&](const char* Cl) -> std::add_pointer_t<MonoClass>
	{
		static MonoClass* Class = Mono::GetClassFromName("EFT", Cl);
		return Class;
	};

	Test("Player");

	return true;
}
