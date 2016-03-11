#version 330 core

uniform sampler2D textureID;

in vec2 fragTex;

out vec4 fragColor;

void main() {
        vec2 _fragTex = fragTex;
        _fragTex.st = _fragTex.ts;
        // _fragTex.t = 1.0f-_fragTex.t;
        _fragTex = vec2(1.0f,1.0f) - _fragTex;

        vec4 diffuseColor = texture(textureID, _fragTex);
        fragColor = diffuseColor;
}
