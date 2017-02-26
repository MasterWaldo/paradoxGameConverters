/*Copyright (c) 2017 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "PortraitMapper.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"



portraitMapper* portraitMapper::instance = nullptr;



portraitMapper::portraitMapper()
{
	LOG(LogLevel::Info) << "Reading portrait mappings";

	auto fileObj = parser_UTF8::doParseFile("cultureGroupToPortraits.txt");
	if (fileObj == nullptr)
	{
		return;
	}

	auto cultureGroupObjs = fileObj->getLeaves();
	for (auto cultureGroupObj: cultureGroupObjs)
	{
		string cultureGroup = cultureGroupObj->getKey();
		auto cultureGroupMappings = mappings.element.find(cultureGroup);
		if (cultureGroupMappings == mappings.element.end())
		{
			cultureGroupToPortraitsMap newCultureGroupMappings;
			mappings.element.insert(make_pair(cultureGroup, newCultureGroupMappings));
			cultureGroupMappings = mappings.element.find(cultureGroup);
		}

		auto ideologyObjs = cultureGroupObj->getLeaves();
		for (auto ideologyObj: ideologyObjs)
		{
			string ideology = ideologyObj->getKey();
			auto ideologyMapping = cultureGroupMappings->second.element.find(ideology);
			if (ideologyMapping == cultureGroupMappings->second.element.end())
			{
				vector<string> newPortaits;
				cultureGroupMappings->second.element.insert(make_pair(ideology, newPortaits));
				ideologyMapping = cultureGroupMappings->second.element.find(ideology);
			}

			for (auto portraitStr: ideologyObj->getTokens())
			{
				ideologyMapping->second.push_back(portraitStr);
			}
		}
	}
}


string portraitMapper::GetPortrait(string cultureGroup, string ideology)
{
	vector<string> portraits = GetPortraits(cultureGroup, ideology);

	std::uniform_int_distribution<int> firstNameGen(0, portraits.size() - 1);
	return portraits[firstNameGen(rng)];
}


vector<string> portraitMapper::GetPortraits(string cultureGroup, string ideology)
{
	auto mapping = mappings.element.find(cultureGroup);
	if (mapping != mappings.element.end())
	{
		auto portraits = mapping->second.element.find(ideology);
		if (portraits != mapping->second.element.end())
		{
			return portraits->second;
		}
	}

	vector<string> genericPortait;
	genericPortait.push_back("gfx/leaders/leader_unknown.dds");
	return genericPortait;
}