#include "StorageEngine.h"

#include <libconfig.h++>
#include <iostream>

using namespace libconfig;

StorageEngine::StorageEngine()
{
	m_config = new Config();
}
StorageEngine::~StorageEngine()
{
	delete m_config;
}

bool StorageEngine::load()
{
	try
	{
		m_config->readFile(m_path.c_str());
		logger->logInfo(Format("Loading config from {}") << m_path);
		return true;
	}
	catch (FileIOException& e)
	{
		logger->logWarn(Format("Unable to load config from {}") << m_path);
		return false;
	}
}
bool StorageEngine::save()
{
	try
	{
		m_config->writeFile(m_path.c_str());
		return true;
	}
	catch (FileIOException& e)
	{
		logger->logWarn(Format("Unable to save config to {}") << m_path);
		return false;
	}
}

int StorageEngine::getInt(const string& name, int def)
{
	int val;
	if (!m_config->lookupValue(name, val))
		val = def;
	return val;
}
int StorageEngine::getInt(const string& name, int num, int def)
{
	stringstream ss;
	ss << name << "_" << num;
	return getInt(ss.str(), def);
}

void StorageEngine::setInt(const string& name, int value)
{
	int val;
	if (m_config->exists(name))
	{
		Setting& s = m_config->lookup(name);
		s = value;
	}
	else
	{
		Setting& s = m_config->getRoot().add(name, Setting::TypeInt);
		s = value;
	}
}
void StorageEngine::setInt(const string& name, int num, int value)
{
	stringstream ss;
	ss << name << "_" << num;
	setInt(ss.str(), value);
}
