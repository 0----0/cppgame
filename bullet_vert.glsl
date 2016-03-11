#version 330 core

uniform mat4 projection;
uniform mat4 camera;

uniform samplerBuffer bulletList;

uniform vec2 scale;

in vec3 vertPos;
in vec2 vertTex;

out vec2 fragTex;

void main() {
        vec2 bulletData = texelFetch(bulletList, gl_InstanceID).xy;
        vec4 bulletWorldPos = vec4(bulletData.x, 0, bulletData.y, 1);
        vec4 bulletCameraPos = camera * bulletWorldPos;
        vec3 _vertPos = vertPos;
        // _vertPos /= 4.0f;
        // _vertPos.y *= 16.0f;
        _vertPos *= vec3(scale,1);
        vec4 vertCameraPos = bulletCameraPos + vec4(_vertPos*bulletCameraPos.w,0);
        gl_Position = projection * vertCameraPos;
        fragTex = vertTex;
}
