#include "CK2World.h"
#include "..\Log.h"
#include "..\Configuration.h"



void CK2World::init(Object* obj)
{
	// get conversion date
	vector<Object*> dateObj = obj->getValue("date");
	if (dateObj.size() > 0)
	{
		date newDate( dateObj[0]->getLeaf() );
		endDate = newDate;
	}
	else
	{
		date newDate("1399.10.14");
	}

	// get dynasties
	printf("	Getting dynasties from save\n");
	vector<Object*> dynastyLeaves = obj->getValue("dynasties");
	dynastyLeaves = dynastyLeaves[0]->getLeaves();
	for (unsigned int i = 0; i < dynastyLeaves.size(); i++)
	{
		int number = atoi( dynastyLeaves[i]->getKey().c_str() );
		CK2Dynasty* newDynasty = new CK2Dynasty;
		newDynasty->init(dynastyLeaves[i]);
		dynasties.insert( make_pair(number, newDynasty) );
	}

	// get characters
	printf("	Getting characters\n");
	vector<Object*> characterLeaves = obj->getValue("character");
	characterLeaves = characterLeaves[0]->getLeaves();
	for (unsigned int i = 0; i < characterLeaves.size(); i++)
	{
		int number = atoi( characterLeaves[i]->getKey().c_str() );
		CK2Character* newCharacter = new CK2Character;
		newCharacter->init(characterLeaves[i], dynasties);
		characters.insert( make_pair(number, newCharacter) );
	}

	// get titles
	printf("	Getting titles\n");
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ( (key.substr(0, 2) == "e_") || (key.substr(0, 2) == "k_") || (key.substr(0, 2) == "d_") || (key.substr(0, 2) == "c_") || (key.substr(0, 2) == "b_") )
		{
			CK2Title* newTitle = new CK2Title;
			newTitle->init(leaves[i], characters);
			titles.insert( make_pair(newTitle->getTitleString(), newTitle) );
		}
	}

	// get provinces
	printf("	Getting provinces\n");
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if (atoi(key.c_str()) > 0)
		{
			CK2Province* newProvince = new CK2Province;
			newProvince->init(leaves[i], titles);
			provinces.insert(make_pair(atoi(key.c_str()), newProvince) );
		}
	}

	// create tree of vassal/liege relationships
	printf("	Relating vassals and lieges\n");
	string hreTitle = Configuration::getHRETitle();
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		string liege = i->second->getLiegeString();
		if (liege == "")
		{
			independentTitles.push_back(i->second);
		}
		else if (liege == hreTitle)
		{
			i->second->addToHRE();
			independentTitles.push_back(i->second);
			hreMembers.push_back(i->second);
		}
		else
		{
			i->second->addLiege(titles[liege]);
		}
	}

	log("There are a total of %d independent titles\n", independentTitles.size());
	log("There are a total of %d hre members\n", hreMembers.size());
}


void CK2World::addDynasties(Object* obj)
{
	vector<Object*> dynastyLeaves = obj->getLeaves();
	for (unsigned int i = 0; i < dynastyLeaves.size(); i++)
	{
		int number = atoi( dynastyLeaves[i]->getKey().c_str() );
		CK2Dynasty* newDynasty = new CK2Dynasty;
		newDynasty->init(dynastyLeaves[i]);
		dynasties.insert( make_pair(number, newDynasty) );
	}
}


date CK2World::getEndDate()
{
	return endDate;
}


vector<CK2Title*> CK2World::getIndependentTitles()
{
	return independentTitles;
}


map<int, CK2Province*> CK2World::getProvinces()
{
	return provinces;
}


CK2World::~CK2World()
{
/*	while (independentTitles.size() > 0)
	{
		CK2Title* currentTitle = independentTitles[independentTitles.size() - 1];
		delete currentTitle;
		independentTitles.pop_back();
	}*/
}