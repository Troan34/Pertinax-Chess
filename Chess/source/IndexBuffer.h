#pragma once
#include <vector>
class IndexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
	std::vector<int> data;
public:
	IndexBuffer(unsigned int numOfQuads);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};
