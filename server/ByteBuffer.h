#ifndef __BYTEBUFFER_H__
#define __BYTEBUFFER_H__ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <vector>
#include <string>
using namespace std;

class ByteBuffer : public vector<char>
{
public:
	ByteBuffer() : m_pos(0) { }
	ByteBuffer(const char* data, size_t len) : m_pos(0) { insert(end(), data, data + len); }

	template<typename T>
	bool append(const T& val)
	{
		insert(end(), (char*)&val, (char*)&val + sizeof(T));
		return true;
	}
	template<typename T>
	bool append(const vector<T>& array)
	{
		uint16_t len = array.size();
		append(len);
		for (int i = 0; i < len; i++)
		{
			const T& val = array[i];
			insert(end(), (char*)&val, (char*)&val + sizeof(T));
		}
		return true;
	}

	template<typename T>
	bool fetch(T& val)
	{
		if (m_pos + sizeof(T) > size())
			return false;
		memcpy(&val, data() + m_pos, sizeof(T));
		m_pos += sizeof(T);
		return true;
	}
	bool fetch(string& val)
	{
		uint16_t len;
		fetch(len);
		val = "";
		while (len--)
		{
			char c;
			fetch(c);
			val += c;
		}
		return true;
	}

	void rewind()
	{
		m_pos = 0;
	}

	void print()
	{
		for (int i = 0; i < size(); i++)
			printf("0x%02x, ", (unsigned char)(*this)[i]);
		printf("\n");
	}

	const char* ptr() { return data(); }

private:
	int m_pos;
};

#endif
