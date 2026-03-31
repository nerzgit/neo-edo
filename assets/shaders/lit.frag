#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec4 fragPosLightSpace;
in vec2 fragTexCoord;

uniform vec4      color;
uniform vec3      lightDir;       // ライトからシーンへの方向 (正規化済み)
uniform float     ambientStrength;
uniform sampler2D shadowMap;
uniform sampler2D albedoMap;
uniform int       hasAlbedoMap;  // 1 = テクスチャあり, 0 = color を使用

out vec4 FragColor;

// PCF 3x3 シャドウサンプリング
float computeShadow(vec4 fragPosLS, vec3 normal) {
    // パースペクティブ除算 → NDC座標 [-1, 1]
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    // [0, 1] にマッピング
    projCoords = projCoords * 0.5 + 0.5;

    // ライトフラスタム外は影なし
    if (projCoords.z > 1.0)
        return 0.0;

    // シャドウアクネ防止バイアス (法線とライト方向の角度で補正)
    vec3 toLight = normalize(-lightDir);
    float cosTheta = max(dot(normal, toLight), 0.0);
    float bias = max(0.005 * (1.0 - cosTheta), 0.001);

    // PCF 3x3 サンプリング
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
    vec3 normal  = normalize(fragNormal);
    vec3 toLight = normalize(-lightDir);

    // アンビエント + ディフューズ
    float diff   = max(dot(normal, toLight), 0.0);
    float shadow = computeShadow(fragPosLightSpace, normal);

    // 影の領域はアンビエントのみ
    float lighting = ambientStrength + (1.0 - shadow) * diff * (1.0 - ambientStrength);

    vec4 baseColor = (hasAlbedoMap != 0) ? texture(albedoMap, fragTexCoord) : color;
    FragColor = vec4(baseColor.rgb * lighting, baseColor.a);
}
