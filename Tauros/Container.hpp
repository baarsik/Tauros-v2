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

	template<class T, class... Arg>
	std::shared_ptr<T> Register(Arg... args)
	{
		auto typeId = &typeid(T);
		auto ptr = std::make_shared<T>(args...);
		Register(typeId->raw_name(), ptr);
		return ptr;
	}

	template<class T>
	void Register(const std::type_info& typeId, std::shared_ptr<T> ptr)
	{
		Register(typeId.raw_name(), std::move(ptr));
	}

	template<class T>
	void Register(const std::string& id, std::shared_ptr<T> ptr)
	{
		while (!m_mapMutex.try_lock()) {}
		const auto iter = m_map.find(id);

		if (iter == m_map.end())
			m_map[id] = ptr;

		m_mapMutex.unlock();
	}

	template<class T>
	std::shared_ptr<T> Resolve()
	{
		auto typeId = &typeid(T);
		return Resolve<T>(typeId->raw_name());
	}

	template<class T>
	std::shared_ptr<T> Resolve(const std::type_info& typeId)
	{
		return Resolve<T>(typeId.raw_name());
	}

	template<class T>
	std::shared_ptr<T> Resolve(const std::string& id)
	{
		while(!m_mapMutex.try_lock()) {}

		const auto iter = m_map.find(id);
		if (iter != m_map.end())
		{
			auto ptr = std::static_pointer_cast<T>(iter->second);
			m_mapMutex.unlock();
			return std::move(ptr);
		}

		throw std::runtime_error(XorStr("Could not locate type in IoC"));
	}

	template<class T>
	bool Contains()
	{
		auto typeId = &typeid(T);
		return Contains<T>(typeId->raw_name());
	}

	template<class T>
	bool Contains(const std::type_info& typeId)
	{
		return Contains<T>(typeId.raw_name());
	}

	template<class T>
	bool Contains(const std::string& id)
	{
		std::lock_guard<std::mutex> lock(m_mapMutex);
		return m_map.find(id) != m_map.end();
	}
private:
	Container() {};
	std::map<std::string, std::shared_ptr<void>> m_map;
	std::mutex m_mapMutex;
};

