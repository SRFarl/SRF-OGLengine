#ifndef _COLLISIONINFOMANAGER_H_
#define _COLLISIONINFOMANAGER_H_

#include "Headers.h"

struct CollisionInfo 
{
	std::string ID1;
	std::string ID2;
};

class CollisionInfoManager
{
private:
	std::vector<CollisionInfo> m_collisionInfo;

public:
	void AddInfo(std::string ID1, std::string ID2)
	{
		//create a new collisioninfo object
		CollisionInfo temp;
		temp.ID1 = ID1;
		temp.ID2 = ID2;

		m_collisionInfo.push_back(temp);
	}

	std::vector<CollisionInfo> GetInfos()
	{
		//get all infos
		return m_collisionInfo;
	}

	std::vector<CollisionInfo> GetInfoOfID(std::string _ID)
	{
		//get infos that have a certain ID
		std::vector<CollisionInfo> temp;

		for (int i = 0; i < m_collisionInfo.size(); i++)
		{
			if (m_collisionInfo[i].ID1 == _ID || m_collisionInfo[i].ID2 == _ID)
				temp.push_back(m_collisionInfo[i]);
		}

		return temp;
	}

	void ClearInfo()
	{
		m_collisionInfo.clear();
	}

};

#endif // _COLLISIONINFOMANAGER_H_
