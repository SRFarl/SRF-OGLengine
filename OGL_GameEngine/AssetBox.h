#ifndef _ASSETBOX_H_
#define _ASSETBOX_H_
#include "Headers.h"
#include "Model.h"

class AssetBox
{
public:
	AssetBox(){};
	~AssetBox()
	{
		//clean asset box
		m_assetList.clear();
	}

	void LoadAsset(std::string _assetName, GLchar* modelPath, bool dynamicDrawing)
	{
		//create a model and then add it to the box
		Model temp(modelPath, _assetName, dynamicDrawing);
		m_assetList.push_back(temp);
	}

	void RemoveAsset(const std::string _assetName)
	{
		//remove an asset from the box
		for (std::vector<Model>::iterator it = m_assetList.begin(); it != m_assetList.end();)
		{
			if (it->GetName() == _assetName)
			{
				m_assetList.erase(it);
				return;
			}

			it++;
		}
	}

	Model* GetAsset(std::string _assetName)
	{
		//return an asset found with a name tag
		for (std::vector<Model>::iterator it = m_assetList.begin(); it != m_assetList.end();)
		{
			if (it->GetName() == _assetName)
				return &(*it);

			it++;
		}

		return NULL;
	}

private:
	std::vector<Model> m_assetList;

};

#endif