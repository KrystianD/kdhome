#ifndef __STORAGEENGINE_H__
#define __STORAGEENGINE_H__

classStorageEngine 
{
public:
	StorageEngine(const string& name) : m_name(name) { }
	~StorageEngine() { }

	bool load();
	bool save();

private:
	string m_name;
};

#endif
