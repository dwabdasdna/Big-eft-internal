#pragma once

#include <windows.h>
#include <cstdint>
#include <vector>
#include <functional>

#define EXPORT(Type, Name) \
	HMODULE monoDll = nullptr; \
	if(!monoDll) \
		monoDll = GetModuleHandleA("mono-2.0-bdwgc.dll"); \
	static const auto Name = reinterpret_cast<Type>(GetProcAddress(monoDll, #Name));

class MonoMethod
{
public:
	std::uintptr_t Compile()
	{
		EXPORT(std::uintptr_t(*)(void*), mono_compile_method);
		return mono_compile_method(this);
	}
};

class MonoField
{
public:
	std::uintptr_t GetOffset()
	{
		EXPORT(std::uintptr_t(*)(void*), mono_field_get_offset);
		return mono_field_get_offset(this);
	}
};

class MonoClass
{
public:
	std::uintptr_t GetMethodFromName(const char* Name, int ArgCount = -1) 
	{
		EXPORT(MonoMethod * (*)(void*, const char*, int), mono_class_get_method_from_name);

		static MonoMethod* Method = mono_class_get_method_from_name(this, Name, ArgCount);
		if(!Method)
			return {};

		return Method->Compile();
	}

	std::uintptr_t GetFieldFromName(const char* Name)
	{
		EXPORT(MonoField * (*)(void*, const char*), mono_class_get_field_from_name);

		static MonoField* Field = mono_class_get_field_from_name(this, Name);
		if(!Field)
			return {};

		return Field->GetOffset();
	}
};

class MonoImage
{
public:
	MonoClass* ClassFromName(const char* Namespace, const char* Class)
	{
		EXPORT(MonoClass* (*)(void*, const char*, const char*), mono_class_from_name);
		return mono_class_from_name(this, Namespace, Class);
	}
};

class MonoAssembly
{
public:
	MonoImage* GetImage()
	{
		EXPORT(MonoImage* (*)(void*), mono_assembly_get_image);
		return mono_assembly_get_image(this);
	}
};

class MonoDomain
{
private:
	inline static void enumerator(MonoAssembly* CallBack, std::vector<MonoAssembly*>* Vector)
	{
		Vector->push_back(CallBack);
	}
public:
	std::vector<MonoAssembly *> GetAssemblies()
	{
		EXPORT(void(*)(void*, void*), mono_assembly_foreach);

		std::vector<MonoAssembly *> PushedAssemblies;
		mono_assembly_foreach(enumerator, &PushedAssemblies);

		return PushedAssemblies;
	}

	static MonoDomain* GetRootDomain()
	{
		EXPORT(MonoDomain * (*)(), mono_get_root_domain);
		return mono_get_root_domain();
	}
};

class Mono
{
public:

	static MonoClass* GetClassFromName(const char* Namespace, const char* ClassName)
	{
		MonoDomain* Domain = MonoDomain::GetRootDomain();
		if(!Domain)
			return nullptr;

		for(MonoAssembly* Assembly : Domain->GetAssemblies())
		{
			if(!Assembly)
				continue;

			MonoImage* Image = Assembly->GetImage();
			if(!Image)
				continue;

			MonoClass* Class = Image->ClassFromName(Namespace, ClassName);
			if(!Class)
				continue;

			return Class;
		}

		return nullptr;
	}

};


