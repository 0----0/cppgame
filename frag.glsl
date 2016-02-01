#version 330 core

uniform vec3 lightDirection = normalize(vec3(-1, -1, -1));
uniform vec3 cameraPos;
uniform sampler2D textureID;
uniform sampler2D normMapID;
// uniform sampler2D shadowMapID;
uniform sampler2DArray shadowmapArrayID;
uniform int numShadowLevels;
uniform mat4 shadowTransform;

in vec3 fragNormal;
in vec2 fragTex;
in vec3 fragTangent;
in vec3 fragBitangent;
in vec3 worldPos;

out vec4 fragColor;

vec2 poissonDiskSmall[10] = vec2[](
        vec2(-0.124351, -0.845406),
        vec2(-0.0640342, -0.221163),
        vec2(0.410229, -0.899541),
        vec2(-0.721987, -0.532098),
        vec2(0.575004, -0.381079),
        vec2(0.778012, 0.535681),
        vec2(-0.548979, 0.0780499),
        vec2(0.346748, 0.110258),
        vec2(0.0245056, 0.54319),
        vec2(0.958218, -0.0199161)
);

vec2 poissonDisk[36] = vec2[](
        vec2(-0.412506, -0.505401),
        vec2(-0.198678, -0.362386),
        vec2(0.0612825, -0.360698),
        vec2(-0.680803, -0.662816),
        vec2(-0.275453, -0.057909),
        vec2(-0.57524, -0.225002),
        vec2(-0.135255, -0.644076),
        vec2(-0.357393, -0.873324),
        vec2(0.087715, -0.83279),
        vec2(0.274221, -0.575671),
        vec2(0.012977, 0.0652626),
        vec2(-0.908303, -0.306803),
        vec2(-0.632239, 0.236252),
        vec2(-0.972579, 0.0551293),
        vec2(-0.907004, 0.400391),
        vec2(0.26208, -0.129809),
        vec2(0.515364, -0.235511),
        vec2(0.470022, -0.743824),
        vec2(0.62804, -0.459096),
        vec2(0.332402, 0.200743),
        vec2(0.83457, -0.0398735),
        vec2(0.645883, 0.24368),
        vec2(0.367844, 0.453785),
        vec2(0.700306, 0.512157),
        vec2(-0.414574, 0.413026),
        vec2(-0.51206, 0.680345),
        vec2(0.40368, 0.800573),
        vec2(-0.0603344, 0.708574),
        vec2(0.0684587, 0.385982),
        vec2(-0.209951, 0.191194),
        vec2(-0.273511, 0.918154),
        vec2(0.19039, 0.661354),
        vec2(0.511419, 0.0151699),
        vec2(0.962834, -0.256675),
        vec2(0.897324, 0.331031),
        vec2(0.00145936, 0.959284)
);

float shade(vec3 pos, vec3 normal) {
        vec4 shadowmapHCoords = shadowTransform * vec4(pos, 1.0f);
        vec3 shadowmapCoords = shadowmapHCoords.xyz / shadowmapHCoords.w;

        float shadowmapScalar = 1.0f;
        float shadowmapLevel = 0.0f;
        float safetyBorder = 32.0f / 16.0f * 8.0f/1024.0f * 2.0f;
        float maxLevel = float(numShadowLevels - 1);
        while ((shadowmapCoords.x > 1.0f - safetyBorder
            || shadowmapCoords.x < -1.0f + safetyBorder
            || shadowmapCoords.y > 1.0f - safetyBorder
            || shadowmapCoords.y < -1.0f + safetyBorder)
            && shadowmapLevel < maxLevel)
        {
                shadowmapScalar *= 0.5f;
                shadowmapCoords.xy *= 0.5f;
                shadowmapLevel += 1.0f;
        }


        shadowmapCoords *= 0.5f;
        shadowmapCoords += 0.5f;
        shadowmapCoords.z -= 3.0f*0.25f/1024.0f;

        float checkRadius = shadowmapScalar * 8.0f * 32.0f / 16.0f / 1024.0f;
        float radius = 0.0f;
        float divi = 0.0f;
        for (int i = 0; i < 10; i++) {
                vec2 coords = shadowmapCoords.xy + poissonDiskSmall[i] * checkRadius;
                float shadowSample = texture(shadowmapArrayID, vec3(coords, shadowmapLevel)).r;
                float smolRadius = shadowmapCoords.z - shadowSample;
                if (smolRadius < 0.0f) continue;
                radius += smolRadius;
                divi += 1.0f;
        }

        if (radius <= 0.0f || divi == 0.0f) { return 1.0f; }
        radius *= 512.0f;
        radius /= divi;
        radius = clamp(radius, 0.0f, 8.0f);
        radius *= shadowmapScalar;
        radius *= 32.0f / 16.0f;
        radius /= 1024.0f;

        float visibility = 0.0f;
        for (int i = 0; i < 36; i++) {
                vec2 coords = shadowmapCoords.xy + (poissonDisk[i])*radius;
                float shadowSample = texture(shadowmapArrayID, vec3(coords, shadowmapLevel)).r;
                if(shadowSample >= shadowmapCoords.z) {
                        visibility += 1/36.0f;
                }
        }

        return visibility;
}

void main() {
        vec2 _fragTex = fragTex;
        _fragTex.t = 1.0f-fragTex.t;

        vec3 normMap = texture(normMapID, _fragTex).xyz*2.0f - 1.0f;

        vec3 realNorm = normalize(normMap.x * fragTangent + normMap.y * fragBitangent + normMap.z * fragNormal);

        vec3 diffuseColor = texture(textureID, _fragTex).rgb;
        vec3 ambientColor = vec3(0.05, 0.1, 0.2);
        vec3 specularColor = vec3(0.6, 0.5, 0.3);
        float shininess = 64.0f;

        vec3 ambient = ambientColor;

        float diffuseIntensity = clamp(dot(-lightDirection, realNorm), 0.0f, 1.0f);

        vec3 diffuse = diffuseColor * diffuseIntensity;

        vec3 specular = vec3(0.0f);
        if (diffuseIntensity > 0.0f) {
                vec3 eyeVector = normalize(worldPos - cameraPos);

                vec3 reflAngle = reflect(lightDirection, realNorm);

                float specularIntensity = clamp(dot(-eyeVector, reflAngle), 0.0f, 1.0f);
                specularIntensity = pow(specularIntensity, shininess);
                specular = specularIntensity * specularColor;

                float visibility = shade(worldPos, fragNormal);
                // fragColor = vec4(vec3(diffuse.r, diffuse.g, shadeasdf(worldPos, fragNormal)), 1.0f);
                specular *= visibility;
                // if(visibility == 0.0f) { visibility += 0.4; }
                visibility = visibility*0.8f + 0.2f;
                diffuse *= visibility;
        }

        fragColor = vec4((ambient + diffuse + specular), 1.0);
        // fragColor = vec4(vec3(shadeasdf(worldPos, fragNormal)), 1.0);
}
