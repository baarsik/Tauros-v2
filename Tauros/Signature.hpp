#pragma once
#include <cstdint>
#include "Utils.hpp"

class Signature
{
public:
	Signature(const char* module, const char* signature, const int offset = 0)
	{
		m_szModule = module;
		m_szSignature = signature;
		m_iOffset = offset;
	}

	uint64_t Find() const
	{
		return Utils::FindSignature(m_szModule, m_szSignature) + m_iOffset;
	}

	template<typename T>
	T Find() const
	{
		return reinterpret_cast<T>(Find());
	}

private:
	const char* m_szModule;
	const char* m_szSignature;
	int m_iOffset;
};
