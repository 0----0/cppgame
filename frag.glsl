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

// vec2 poissonDisk[4] = vec2[](
//         vec2( -0.94201624, -0.39906216 ),
//         vec2( 0.94558609, -0.76890725 ),
//         vec2( -0.094184101, -0.92938870 ),
//         vec2( 0.34495938, 0.29387760 )
// );

// vec2 poissonDisk[10] = vec2[](
//         vec2(0.528611, 0.783841),
//         vec2(0.195879, 0.552738),
//         vec2(0.708068, 0.383469),
//         vec2(0.888947, 0.660722),
//         vec2(0.210757, 0.859638),
//         vec2(0.194944, 0.203398),
//         vec2(0.498866, 0.202286),
//         vec2(0.958745, 0.357894),
//         vec2(0.780851, 0.118539),
//         vec2(0.446792, 0.491815)
// );

// vec2 poissonDisk[18] = vec2[](
// vec2(0.547119, 0.310390),
// vec2(0.350803, 0.197655),
// vec2(0.206609, 0.327387),
// vec2(0.742105, 0.411957),
// vec2(0.370535, 0.396558),
// vec2(0.587989, 0.535413),
// vec2(0.398020, 0.0257985),
// vec2(0.820791, 0.236304),
// vec2(0.636634, 0.0606192),
// vec2(0.405776, 0.703943),
// vec2(0.122073, 0.579017),
// vec2(0.962447, 0.351133),
// vec2(0.819056, 0.589548),
// vec2(0.173270, 0.124762),
// vec2(0.566686, 0.867161),
// vec2(0.743633, 0.786623),
// vec2(0.225741, 0.829914),
// vec2(0.402693, 0.948379)
// );

vec2 poissonDisk[36] = vec2[](
vec2(0.395054, 0.684368),
vec2(0.578835, 0.797596),
vec2(0.402387, 0.830845),
vec2(0.551255, 0.647816),
vec2(0.225131, 0.642705),
vec2(0.303427, 0.493762),
vec2(0.26847, 0.813521),
vec2(0.473113, 0.473824),
vec2(0.692983, 0.688466),
vec2(0.629628, 0.92086),
vec2(0.474791, 0.96879),
vec2(0.801073, 0.849776),
vec2(0.336764, 0.953124),
vec2(0.409678, 0.329111),
vec2(0.603203, 0.277309),
vec2(0.606313, 0.439065),
vec2(0.789178, 0.60365),
vec2(0.0843016, 0.770825),
vec2(0.7893, 0.304821),
vec2(0.496446, 0.202179),
vec2(0.689997, 0.160255),
vec2(0.75387, 0.458074),
vec2(0.915102, 0.454391),
vec2(0.933195, 0.692088),
vec2(0.954158, 0.3314),
vec2(0.208091, 0.373336),
vec2(0.351545, 0.214896),
vec2(0.196868, 0.200271),
vec2(0.0327486, 0.47858),
vec2(0.0817929, 0.266491),
vec2(0.0931633, 0.64547),
vec2(0.827113, 0.164748),
vec2(0.487368, 0.0365336),
vec2(0.316865, 0.0886926),
vec2(0.629342, 0.0381712),
vec2(0.175543, 0.500409)
);

float shade(vec3 pos, vec3 normal) {
        vec4 shadowmapHCoords = shadowTransform * vec4(pos, 1.0f);
        vec3 shadowmapCoords = shadowmapHCoords.xyz / shadowmapHCoords.w;
        shadowmapCoords *= 0.5f;
        shadowmapCoords += 0.5f;
        // shadowmapCoords.z -= 0.0005;
        shadowmapCoords.z -= 6.0f*0.25f/1024.0f;

        // vec3 shadowNormal = vec3(shadowTransform * vec4(normal, 0.0f));
        // float shadowSlopeInv1 = -shadowNormal.x / shadowNormal.z;
        // float shadowSlopeInv2 = -shadowNormal.y / shadowNormal.z;
        // float p = 0.25f / 1024.0f;
        // shadowmapCoords.z -= p*max(abs(shadowSlopeInv1), abs(shadowSlopeInv2));

        // return texture(shadowMapID, shadowmapCoords);
        float visibility = 0.0f;
        for(int i = 0; i < 36; i++) {
                visibility += texture(shadowMapID, shadowmapCoords + vec3(poissonDisk[i]/64.0f,0.0f))/36.0f;
        }
        return visibility;
        // return shadowSlopeInv1;
        // float shadow = texture(shadowMapID, shadowmapCoords.xy).r;
        // if (shadow < shadowmapCoords.z - 0.005) {
        //         return 0.0f;
        // } else {
        //         return 1.0f;
        // }
}

float shadeasdf(vec3 pos, vec3 normal) {
        vec4 shadowmapHCoords = shadowTransform * vec4(pos, 1.0f);
        vec3 shadowmapCoords = shadowmapHCoords.xyz / shadowmapHCoords.w;
        shadowmapCoords *= 0.5f;
        shadowmapCoords += 0.5f;
        // shadowmapCoords.z -= 0.0005;

        vec3 shadowNormal = vec3(shadowTransform * vec4(normal, 0.0f));
        // if(shadowNormal.x  <  0.000001f && shadowNormal.x >= 0.0f) shadowNormal.x  =  0.000001f;
        // if(shadowNormal.x  > -0.000001f && shadowNormal.x < 0.0f) shadowNormal.x  = -0.000001f;
        // if(shadowNormal.y  <  0.000001f && shadowNormal.y >= 0.0f) shadowNormal.y  =  0.000001f;
        // if(shadowNormal.y  > -0.000001f && shadowNormal.y < 0.0f) shadowNormal.y  = -0.000001f;
        float shadowSlopeInv1 = -shadowNormal.x / shadowNormal.z;
        float shadowSlopeInv2 = -shadowNormal.y / shadowNormal.z;
        float p = 0.25f / 1024.0f;
        shadowmapCoords.z -= p*max(abs(shadowSlopeInv1), abs(shadowSlopeInv2));

        // return texture(shadowMapID, shadowmapCoords);
        return p*max(abs(shadowSlopeInv1), abs(shadowSlopeInv2));
        // return abs(shadowNormal.z / shadowNormal.x);
        // return shadowSlopeInv1;
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

                float visibility = shade(worldPos, fragNormal);
                // fragColor = vec4(vec3(diffuse.r, diffuse.g, shadeasdf(worldPos, fragNormal)), 1.0f);
                specular *= visibility;
                // if(visibility == 0.0f) { visibility += 0.4; }
                visibility = visibility*0.8f + 0.2f;
                diffuse *= visibility;
        }

        fragColor = vec4((ambient + diffuse + specular), 1.0);
}
