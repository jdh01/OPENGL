#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID; //ID for every object created in OpenGL (Different to engine side)
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};