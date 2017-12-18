#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "XorStr.hpp"

/* 
 * IoC container is supposed to be the only Singleton class
 * Remove all other Singleton/static static classes and put them in container
 */

class Container
{
public:
	Container(const Container&) = delete;
	Container& operator=(const Container&) = delete;

	static Container& Instance()
	{
		static Container instance;
		return instance;
	}

	template<class T>
	void Register(std::shared_ptr<T> ptr)
	{
		auto typeId = &typeid(T);
		Register(typeId->raw_name(), ptr);
	}

	template<class T>
	void Register(const std::type_info* typeId, std::shared_ptr<T> ptr)
	{
		if (!typeId)
			throw std::runtime_error(XorStr("Invalid type id to register"));

		Register(typeId->raw_name(), ptr);
	}

	template<class T>
	void Register(const std::string& id, std::shared_ptr<T> ptr)
	{
		std::lock_guard<std::mutex> lock(m_mapMutex);
		const auto iter = m_mapping.find(id);

		if (iter == m_mapping.end())
			m_mapping[id] = ptr;
	}

	template<class T>
	std::shared_ptr<T> Resolve()
	{
		auto typeId = &typeid(T);
		return Resolve<T>(typeId->raw_name());
	}

	template<class T>
	std::shared_ptr<T> Resolve(const std::type_info* typeId)
	{
		if (!typeId)
			throw std::runtime_error(XorStr("Invalid type id to resolve"));

		return Resolve<T>(typeId->raw_name());
	}

	template<class T>
	std::shared_ptr<T> Resolve(const std::string& id)
	{
		std::lock_guard<std::mutex> lock(m_mapMutex);
		const auto iter = m_mapping.find(id);

		if (iter != m_mapping.end())
			return std::static_pointer_cast<T>(iter->second);

		throw std::runtime_error(XorStr("Could not locate type in IoC"));
	}

	template<class T>
	bool Contains()
	{
		auto typeId = &typeid(T);
		return Contains<T>(typeId->raw_name());
	}

	template<class T>
	bool Contains(const std::type_info* typeId)
	{
		if (!typeId)
			return false;

		return Contains<T>(typeId->raw_name());
	}

	template<class T>
	bool Contains(const std::string& id)
	{
		std::lock_guard<std::mutex> lock(m_mapMutex);
		return m_mapping.find(id) != m_mapping.end();
	}
private:
	Container() {};
	std::map<std::string, std::shared_ptr<void>> m_mapping;
	std::mutex m_mapMutex;
};

