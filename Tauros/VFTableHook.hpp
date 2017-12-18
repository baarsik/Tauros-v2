#pragma once

#include <Windows.h>
#include <stdio.h>
#include <map>

namespace Hooks
{
    typedef DWORD** PPDWORD;
    class VFTableHook
    {
        VFTableHook(const VFTableHook&) = delete;
    public:
		template <typename T>
        VFTableHook(T classBase)
        {
	        const auto ppClass = reinterpret_cast<PPDWORD>(classBase);
            m_ppClassBase = ppClass;
            m_pOriginalVMTable = *ppClass;
	        
			m_dwVMTableSize = GetCount();
            m_pNewVMTable = new DWORD[m_dwVMTableSize];
            memcpy(m_pNewVMTable, m_pOriginalVMTable, m_dwVMTableSize * sizeof(DWORD));

            DWORD old;
            VirtualProtect(m_ppClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
            *m_ppClassBase = m_pNewVMTable;
            VirtualProtect(m_ppClassBase, sizeof(DWORD), old, &old);
        }

        ~VFTableHook()
        {
            RestoreTable();
            if(m_pNewVMTable) delete[] m_pNewVMTable;
        }

        void RestoreTable() const
        {
			if (m_ppClassBase == nullptr)
				return;

            DWORD old;
            VirtualProtect(m_ppClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
            *m_ppClassBase = m_pOriginalVMTable;
            VirtualProtect(m_ppClassBase, sizeof(DWORD), old, &old);
        }

        template<class T>
        T Hook(uint32_t index, T fnNew)
        {
			if (!m_pNewVMTable || !m_pOriginalVMTable || index > m_dwVMTableSize || index < 0)
				return nullptr;

	        auto dwOld = static_cast<DWORD>(m_pOriginalVMTable[index]);
            m_pNewVMTable[index] = reinterpret_cast<DWORD>(fnNew);
            m_vecHookedIndexes.insert(std::make_pair(index, static_cast<DWORD>(dwOld)));
            return reinterpret_cast<T>(dwOld);
        }

        void Unhook(uint32_t index)
        {
	        const auto it = m_vecHookedIndexes.find(index);
            if(it != m_vecHookedIndexes.end())
			{
                m_pNewVMTable[index] = static_cast<DWORD>(it->second);
                m_vecHookedIndexes.erase(it);
            }
        }

        template<class T>
        T GetOriginal(uint32_t index)
        {
            return reinterpret_cast<T>(m_pOriginalVMTable[index]);
        }

    private:
        DWORD GetCount() const
        {
            if(!m_pOriginalVMTable)
				return 0;

			MEMORY_BASIC_INFORMATION memory;
	        auto index = 0;
			while (true)
			{
				// NOTE: check for valid a memory region on a pointer otherwise the vmt count will fail or return wrong values
				if (VirtualQuery(reinterpret_cast<PVOID>(m_pOriginalVMTable[index]), &memory, sizeof(MEMORY_BASIC_INFORMATION)) < 0
					|| (memory.Protect != PAGE_EXECUTE
						&& memory.Protect != PAGE_EXECUTE_READ
						&& memory.Protect != PAGE_EXECUTE_READWRITE
						&& memory.Protect != PAGE_EXECUTE_WRITECOPY))
					break;

				++index;
			}
			return index;
        }

        std::map<uint32_t, DWORD> m_vecHookedIndexes;
        PPDWORD m_ppClassBase;
        PDWORD m_pOriginalVMTable;
        PDWORD m_pNewVMTable;
		DWORD m_dwVMTableSize;
    };
}