#include "IndexBuffer.h"
#include "Renderer.h"
#include <iostream>
IndexBuffer::IndexBuffer(unsigned int numOfQuads)
    : m_Count(numOfQuads*6)
{
    //calculates the data
    unsigned int KeepTheCount = 0;
    unsigned int KeepTheCountForData = 0;
    while (numOfQuads != KeepTheCount)
    {
        data.push_back(KeepTheCountForData);
        data.push_back(KeepTheCountForData+1);
        data.push_back(KeepTheCountForData+2);
        data.push_back(KeepTheCountForData+2);
        data.push_back(KeepTheCountForData+3);
        data.push_back(KeepTheCountForData);
        KeepTheCountForData += 4;
        KeepTheCount += 1;
    }
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * numOfQuads * sizeof(unsigned int), &data.front(), GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}
void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}
void IndexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}