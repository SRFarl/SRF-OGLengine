#include "OBJLoader_SRF.h"



bool obj::Load_OBJ(const char* filename, std::vector<glm::vec4> &vertices, std::vector<glm::vec2> &UVs, std::vector<glm::vec3> &normals, unsigned int &numOfVertexes)
{
	//arrays for storing and indexing the mesh data
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec4 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	std::cerr << "Starting Load " << filename << std::endl;

	//try to open file. fscanf version
	FILE * file;
	fopen_s(&file, filename, "r");
	if (file == NULL)
	{
		std::cerr << "Impossible to open the file !" << std::endl;
		return false;
	}

	//loop over line and store
	char lineHeader[128];
	while (1)
	{
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			break;
		}

		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec4 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.w = 1.0f;
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f \n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by this simple parser : Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}




		//ifstream file opening

		//std::ifstream in(filename, std::ios::in);
		//if (!in) 
		//{
		//	std::cerr << "Cannot open " << filename << std::endl; 
		//	return false;
		//}
		//if (line.substr(0,2) == "v ") 
		//{
		//	std::istringstream s(line.substr(2));
		//	glm::vec4 v;
		//	s >> v.x;
		//	s >> v.y;
		//	s >> v.z;
		//	v.w = 1.0f;
		//	temp_vertices.push_back(v);
		//} else if (line.substr(0,3) == "vn ") 
		//	{
		//		std::istringstream s(line.substr(3));
		//		glm::vec3 v;
		//		s >> v.x;
		//		s >> v.y;
		//		s >> v.z;
		//		temp_normals.push_back(v);
		//	}	else if (line.substr(0,2) == "f ")
		//		{
		//			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
		//			std::istringstream s(line.substr(2));
		//			s >> vertexIndex[0];
		//			s.ignore(1);
		//			s >> uvIndex[0];
		//			s.ignore(1);
		//			s >> normalIndex[0];
		//			s.ignore(1);
		//
		//			s >> vertexIndex[1];
		//			s.ignore(1);
		//			s >> uvIndex[1];
		//			s.ignore(1);
		//			s >> normalIndex[1];
		//			s.ignore(1);
		//
		//			s >> vertexIndex[2];
		//			s.ignore(1);
		//			s >> uvIndex[2];
		//			s.ignore(1);
		//			s >> normalIndex[2];
		//			s.ignore(1);
		//
		//			vertexIndices.push_back(vertexIndex[0]);
		//			vertexIndices.push_back(vertexIndex[1]);
		//			vertexIndices.push_back(vertexIndex[2]);
		//			//uvIndices.push_back(uvIndex[0]);
		//			//uvIndices.push_back(uvIndex[1]);
		//			//uvIndices.push_back(uvIndex[2]);
		//			normalIndices.push_back(normalIndex[0]);
		//			normalIndices.push_back(normalIndex[1]);
		//			normalIndices.push_back(normalIndex[2]);
		//		}

	}

	//store how many vertexes in the mesh
	numOfVertexes = temp_vertices.size();

	//index the vertices for opengl
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		vertices.push_back(temp_vertices[vertexIndices[i] - 1]);
	}

	//index the UVs for opengl
	for (unsigned int i = 0; i < uvIndices.size(); i++)
	{
		UVs.push_back(temp_uvs[uvIndices[i] - 1]);
	}

	//index the normals for opengl
	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{
		normals.push_back(temp_normals[normalIndices[i] - 1]);
	}

	std::cerr << "finished load " << filename << std::endl;
	fclose(file);
	return true;
}
