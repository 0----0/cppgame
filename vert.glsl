#version 330 core

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 obj;

in vec3 position;
in vec3 vertNormal;
in vec2 vertTex;
in vec3 vertTangent;
in vec3 vertBitangent;

out vec3 worldPos;
out vec3 fragNormal;
out vec2 fragTex;
out vec3 fragTangent;
out vec3 fragBitangent;

void main() {
        vec4 world = obj * vec4(position, 1.0);
        worldPos = world.xyz / world.w;
        gl_Position = projection * camera * world;
        fragNormal = normalize((obj * vec4(vertNormal, 0)).xyz);
        fragTex = vertTex;
        fragTangent = vertTangent;
        fragBitangent = vertBitangent;
}
