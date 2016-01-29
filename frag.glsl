#version 330 core

uniform vec3 lightDirection = normalize(vec3(-1, -1, -1));
uniform vec3 cameraPos;
uniform sampler2D textureID;
uniform sampler2D normMapID;
uniform sampler2DShadow shadowMapID;
uniform mat4 shadowTransform;

in vec3 fragNormal;
in vec2 fragTex;
in vec3 fragTangent;
in vec3 fragBitangent;
in vec3 worldPos;

out vec4 fragColor;

float shade() {
        vec4 shadowmapHCoords = shadowTransform * vec4(worldPos, 1.0f);
        vec3 shadowmapCoords = shadowmapHCoords.xyz / shadowmapHCoords.w;
        shadowmapCoords *= 0.5f;
        shadowmapCoords += 0.5f;
        shadowmapCoords.z -= 0.005;

        return texture(shadowMapID, shadowmapCoords);
        // float shadow = texture(shadowMapID, shadowmapCoords.xy).r;
        // if (shadow < shadowmapCoords.z - 0.005) {
        //         return 0.0f;
        // } else {
        //         return 1.0f;
        // }
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
        }

        float visibility = shade();
        specular *= visibility;
        // if(visibility == 0.0f) { visibility += 0.4; }
        visibility = visibility*0.8f + 0.2f;
        diffuse *= visibility;

        fragColor = vec4(ambient + diffuse + specular, 1.0);
}
