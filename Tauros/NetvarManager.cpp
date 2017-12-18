#include "NetvarManager.hpp"

#include <fstream>
#include <utility>
#include "SourceEngine/SDK.hpp"
#include "XorStr.hpp"

using namespace std;

//------------------------------------------------------------
// Classes to help the parsing of the netvars
//------------------------------------------------------------
#pragma region Helper classes

typedef unordered_map<string, unique_ptr<NetvarTable>>           TableMap;
typedef unordered_map<string, uint32_t>                          PropMap;

typedef unordered_map<string, unique_ptr<NetvarTable>>::iterator Iter;

struct NetvarTable
{
    TableMap  m_ChildTables;
    PropMap   m_ChildProps;
    uint32_t  m_uOffset = 0;

    bool IsEmpty() const
    {
        return m_ChildTables.size() == 0 && m_ChildProps.size() == 0;
    }
    void InsertTable(string&& name, unique_ptr<NetvarTable>&& pTable)
    {
        m_ChildTables.emplace(name, move(pTable));
    }
    void InsertProp(string&& name, uint32_t offset)
    {
        m_ChildProps.emplace(name, offset);
    }
};

class NetvarDatabase
{
public:
    TableMap m_Tables;

    void insert(string&& name, unique_ptr<NetvarTable>&& pTable)
    {
        m_Tables.emplace(name, move(pTable));
    }
    Iter find(const string& key) { return m_Tables.find(key); }
    Iter begin() { return m_Tables.begin(); }
    Iter end() { return m_Tables.end(); }

};

#pragma endregion

//------------------------------------------------------------
// Netvar Manager
//------------------------------------------------------------
NetvarManager* NetvarManager::instance = nullptr;

NetvarManager::NetvarManager()
{

}

NetvarManager::~NetvarManager()
{

}

void NetvarManager::CreateDatabase()
{
    m_pDatabase = make_unique<NetvarDatabase>();
    auto pClient = Interfaces::Client();

	if (!pClient)
		return;
        
	for (auto pClass = pClient->GetAllClasses(); pClass; pClass = pClass->m_pNext)
	{
		if (!pClass->m_pRecvTable)
			continue;
		
		//Insert new entry on the database
		m_pDatabase->insert(pClass->m_pRecvTable->m_pNetTableName, InternalLoadTable(pClass->m_pRecvTable, 0));
		m_tableCount++;
	}
}

void NetvarManager::Dump(ostream& output) const
{
    for(auto& entry : *m_pDatabase)
	{
        auto& pTable = entry.second;
        if(pTable->IsEmpty())
            continue;

        output << entry.first << endl;
        Dump(output, *pTable, 1);
        output << XorStr("==============================================================") << endl;
    }
}

void NetvarManager::Dump(const string& file) const
{
    ofstream output(file);
    Dump(output);
}

//------------------------------------------------------------
// Internal methods below. This is where the real work is done
//------------------------------------------------------------
unique_ptr<NetvarTable> NetvarManager::InternalLoadTable(RecvTable* pRecvTable, uint32_t offset)
{
    auto pTable = make_unique<NetvarTable>();
    pTable->m_uOffset = offset;

    for(auto i = 0; i < pRecvTable->m_nProps; ++i)
	{
		const auto pProp = &pRecvTable->m_pProps[i];

        //Skip trash array items
        if(!pProp || isdigit(pProp->m_pVarName[0])) continue;
        //We dont care about the base class
        if(strcmp(pProp->m_pVarName, XorStr("baseclass")) == 0) continue;

        //If this prop is a table
        if(pProp->m_RecvType == SendPropType::DPT_DataTable &&  //If it is a table AND
            pProp->m_pDataTable != nullptr &&                                //The DataTable isnt null AND
            pProp->m_pDataTable->m_pNetTableName[0] == 'D') {                //The Table name starts with D (this is because there are some shitty nested 
                                                                             //tables that we want to skip, and those dont start with D)

                                                                             //Load the table pointed by pProp->m_pDataTable and insert it
            pTable->InsertTable(pProp->m_pVarName, InternalLoadTable(pProp->m_pDataTable, pProp->m_Offset));
        } else {
            pTable->InsertProp(pProp->m_pVarName, pProp->m_Offset);
        }
        m_netvarCount++;
    }
    return pTable;
}

void NetvarManager::Dump(ostream& output, NetvarTable& table, int level) const
{
    char line[512] = "";
    char fmt[512] = "";

    //yo dawg, i hear u like formatting strings so i am formatting ur formatting string
    sprintf(fmt, "%%-%ds: 0x%%08X\n", 50 - level * 4);

    for(auto& prop : table.m_ChildProps)
	{
        for(auto i = 0; i < level; i++)
		{
			output << (i != level - 1 ? XorStr("    ") : XorStr("|___"));
        }
        sprintf(line, fmt, prop.first.c_str(), prop.second + table.m_uOffset);
        output << line;

    }
    for(auto& child : table.m_ChildTables)
	{
        for(auto i = 0; i < level; i++)
		{
            output << (i != level - 1 ? XorStr("    ") : XorStr("|___"));
        }
        sprintf(line, fmt, child.first.c_str(), child.second->m_uOffset);
        output << line;
        Dump(output, *child.second, level + 1);
    }
}

uint32_t NetvarManager::GetOffset(const string& szTableName, const initializer_list<string>& props) const
{
	const auto table = m_pDatabase->find(szTableName);
    if(table == m_pDatabase->end()) return -1;

	const int tableOffset = table->second->m_uOffset;
    if(props.size() == 0) return tableOffset;

	auto totalOffset = tableOffset;

	auto curTable = table->second.get();

	auto propssize = props.size();
    for(decltype(propssize) i = 0; i < propssize; i++) {
	    const auto propName = *(props.begin() + i);

        if(i + 1 < props.size()) {//This index is not the last one
	        const auto childTable = curTable->m_ChildTables.find(propName);
            if(childTable == curTable->m_ChildTables.end())
                throw runtime_error(XorStr("Prop not found"));
            
            totalOffset += childTable->second->m_uOffset;
            curTable = childTable->second.get();
        }
    	else
		{ //Last index, retrieve prop instead of table
			const auto childProp = curTable->m_ChildProps.find(propName);
            if(childProp == curTable->m_ChildProps.end())
                throw runtime_error(XorStr("Prop not found"));

            totalOffset += childProp->second;
        }
    }

    return totalOffset;
}
