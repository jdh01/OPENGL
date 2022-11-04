#pragma once

#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID; //ID for every object created in OpenGL (Different to engine side)
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count); //size means bytes, count means element count
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};