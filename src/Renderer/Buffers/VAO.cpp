#include "VAO.h"

VAO::VAO() {
	glGenVertexArrays(1, &ID);
}
void VAO::LinkAttrib(uint32 location, uint32 size, uint32 stride, uint32 offset) {
	glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glEnableVertexAttribArray(location);
}
void VAO::Bind() {
	glBindVertexArray(ID);
}
void VAO::Unbind() {
	glBindVertexArray(0);
}
void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}
VAO::~VAO() {
	Delete();
}