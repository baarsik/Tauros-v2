#pragma once
class BinaryReader
{
public:
	BinaryReader(unsigned addr)
	{
		m_baseAddress = addr;
		m_currentOffset = 0;
	}

	void SetOffset(unsigned offset)
	{
		m_currentOffset = offset;
	}

	void Skip(unsigned length)
	{
		m_currentOffset += length;
	}

	int ReadByte()
	{
		return *reinterpret_cast<char*>(m_baseAddress + m_currentOffset++);
	}

	bool ReadBool()
	{
		return *reinterpret_cast<bool*>(m_baseAddress + m_currentOffset++);
	}

	std::string ReadString()
	{
		char buffer[256];
		const auto length = *reinterpret_cast<char*>(m_baseAddress + m_currentOffset++);
		memcpy(buffer, reinterpret_cast<void*>(m_baseAddress + m_currentOffset), length > 255 ? 255 : length);
		buffer[length > 255 ? 255 : length] = '\0';
		m_currentOffset += length + 1;
		return std::string(buffer);
	}
private:
	unsigned m_baseAddress;
	unsigned m_currentOffset;
};