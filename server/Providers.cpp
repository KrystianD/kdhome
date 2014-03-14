#include "Providers.h"

#include "StorageEngine.h"

IProvider::IProvider()
{
	m_storage = new StorageEngine();
}
IProvider::~IProvider()
{
	delete m_storage;
}
