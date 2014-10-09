#include "Providers.h"

#include "common.h"
#include "StorageEngine.h"

IProvider::IProvider()
{
	m_storage = new StorageEngine();
}
IProvider::~IProvider()
{
	logger->logInfo("Deleting IProvider");
	delete m_storage;
}
