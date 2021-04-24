#version 460 core

out vec4 FragColor;

// ������
uniform sampler2D gPosition;   // λ��(LightSpace)
uniform sampler2D gNormal;     // ����(LightSpace)
uniform sampler2D gFlux;       // ͨ��(LightSpace)
uniform sampler2D gDepthMap;   // ���(LightSpace)
uniform sampler2D gRandomMap;  // ����������ƶ�
uniform bool rsmOn;                 // �Ƿ��� RSM
uniform vec3 lightPos;              // ��Դλ��(��������ϵ)
uniform int randomSamples;          // ���������������
uniform vec3 viewPos;               // �ӵ�λ��(��������ϵ)
uniform float radius;               // �����뾶
uniform float indirectStrength;     // ��ӹ���ǿ��

in vec3 fColor;      // ��ɫ
in vec3 fPosLight;   // �Թ�ԴΪ�ӵ������ϵ�е�λ��
in vec3 fPos;        // ��������ϵ�е�λ��
in vec3 fNormal;     // ��������ϵ�еķ���


// ����ڰ�������Ӱ����
// ����Ⱦ������Ի�֮�������Ƚ�, ����ֵ���ÿ���
const float depthBias = 0.0005f;

// �����ӹ���
// normal ��λ���ķ�����
vec3 calcIndirectLight(vec3 normal) {
    vec3 indirectLight = vec3(0.0f);
    for(int i = 0;i < randomSamples; ++i) {
        // texture �� texelFetch ������
        //   texture: ��һ���� [0, 1], ���� blur
        //   texelFetch: ԭʼ����ռ�, û�о��� blur
        
        // ����ƫ��
        vec3 offset = texelFetch(gRandomMap, ivec2(i, 0), 0).xyz;
        // ���������Թ�ԴΪ�ӵ�Ĺ۲�����ϵ�е�λ��(���ڲ���)
        vec2 samplePosInLightSpace = fPosLight.xy + radius*offset.xy;

        // ����������������ϵ�е�λ�á����򡢹�ͨ��
        vec3 xp = texture(gPosition, samplePosInLightSpace).xyz;
        vec3 np = normalize(texture(gNormal, samplePosInLightSpace).xyz); // blur ����ǵ�λ��
        vec3 fp = texture(gFlux, samplePosInLightSpace).xyz;
        
        vec3 xMinusxp = normalize(fPos - xp);
        // �����ӹ���
        indirectLight += fp 
            * offset.z
            * max(dot(np, xMinusxp), 0.0f)
            * max(dot(normal, -xMinusxp), 0.0f)
            / pow(length(xMinusxp), 4.0f);
    }
    return clamp(
        indirectLight/randomSamples *40* indirectStrength, 
        0.0f, 1.0f);
}

// ������Ӱ֮��: 0.0
// ���򷵻� 1.0
// GLSL �����������ǰ�ֵ���ݵ�
float ShadowCalculation(vec3 projCoords) {
    // xyz:[-1, 1] => [0, 1]
    projCoords = projCoords * 0.5 + 0.5;
    // �����ͼ�л�ȡ�����Ϣ
    float closestDepth = texture(gDepthMap, projCoords.xy).r;
    // �ж��Ƿ�����Ӱ����
    // Bias �����Ӱ����������
    
    // ��β���ȡƽ�� PCF, �仯�������
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(gDepthMap, 0); // 0 �������������صľ���
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(gDepthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += ((projCoords.z - depthBias)> pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // Զ��ʼ�տɼ�
    if(projCoords.z > 1.0f) {
        shadow = 0.0f;
    }
    return 1.0f - shadow;
}


void main() {
    // �����Ƿ�����Ӱ֮��
    float notInShadow = ShadowCalculation(fPosLight);

    // (1) ֱ�ӹ���
    // ��Ϊ��ԴΪ�׹�

    // ������ ambient
    vec3 ambient = 0.05 * fColor;

    // ������� diffuse
    vec3 lightDir = normalize(lightPos - fPos);
    // ��һ��, fs �й�һ�������ڲ�ֵ�Ĺ����б�ɷǵ�λ����
    vec3 normal = normalize(fNormal);
    float diff = max(dot(lightDir, normal), 0.0f); // diff = N \cdot L
    vec3 diffuse = diff * fColor;

    // ����߹� specular
    vec3 viewDir = normalize(viewPos - fPos);
    // Blinn-Phong ģ�Ϳ��Ƿ��ߺͰ��ʸ���ļн�
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f); // 32.0 Ϊ���վ��淴��ϵ�� shiness
    vec3 specular = vec3(1.0f) * spec; // 1.0f Ϊ���ȵ���ϵ��

    // (2) ��ӹ���
    vec3 indirect = vec3(0.0f);
    if(rsmOn) {
        indirect = calcIndirectLight(normal);
    }
    // ��Ӱ��Ӱ���ӹ�ͻ�����
    FragColor = vec4(
        (diffuse + specular)*notInShadow + ambient + indirect
        , 1.0f);
    // ٤��У��
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/2.2));
}