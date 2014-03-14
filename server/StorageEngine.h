#ifndef __STORAGEENGINE_H__
#define __STORAGEENGINE_H__

#include "common.h"

using namespace std;

namespace libconfig
{
	class Config;
}

class StorageEngine 
{
public:
	StorageEngine();
	~StorageEngine();

	void setPath(const string& path);
	bool load();
	bool save();

	int getInt(const string& name, int def);
	int getInt(const string& name, int num, int def);

	void setInt(const string& name, int value);
	void setInt(const string& name, int num, int value);

private:
	string m_path;
	libconfig::Config *m_config;
};

#endif
