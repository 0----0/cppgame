#version 330 core

uniform sampler2D textureID;

in vec2 fragTex;

out vec4 fragColor;

void main() {
        vec2 _fragTex = fragTex;
        _fragTex.t = 1.0f-fragTex.t;

        vec4 diffuseColor = texture(textureID, _fragTex);
        fragColor = diffuseColor;
}
