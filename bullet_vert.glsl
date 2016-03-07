#version 330 core

uniform mat4 projection;
uniform mat4 camera;

uniform samplerBuffer bulletList;

in vec3 vertPos;
in vec2 vertTex;

out vec2 fragTex;

void main() {
        vec2 bulletData = texelFetch(bulletList, gl_InstanceID).xy;
        vec4 bulletWorldPos = vec4(bulletData.x, 0, bulletData.y, 1);
        vec4 bulletCameraPos = camera * bulletWorldPos;
        vec4 vertCameraPos = bulletCameraPos + vec4(vertPos*bulletCameraPos.w,0);
        gl_Position = projection * vertCameraPos;
        fragTex = vertTex;
}
