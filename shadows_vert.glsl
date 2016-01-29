#version 330 core

uniform mat4 projView;
uniform mat4 obj;

in vec3 vertPos;

void main() {
        gl_Position = projView * obj * vec4(vertPos, 1.0f);
}
