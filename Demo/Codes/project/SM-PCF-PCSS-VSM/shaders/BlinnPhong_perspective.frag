#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace; // �Թ�ԴΪ�ӵ�Ĺ۲�����ϵ�е��λ��
} fs_in;

//uniform sampler2D floorTexture;
uniform sampler2D depthMap;
uniform sampler2D d_d2_filter;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 color;
uniform int SMType;
uniform float lightWidth;  // ��Դ�Ŀ��
uniform float SMDiffuse;   // ��Ӱ����ɢ�̶�
uniform float PCF_SampleRadius;       // PCF �����뾶

#define BIAS 0.005
#define NEAR_PLANE 2.0
#define FAR_PLANE 20.0

#define PCF_RADIUS 6
#define BLOCK_RADIUS 5


// ��������� [-1, 1] ��͸�����
// ���Ϊ [0, 1] ���������
float getLinearizeDepth(float depth) {
    // ��ͨ������ [-1, 1] ��֤���Ϊ [NEAR_PLANE, FAR_PLANE]
    float z = (2.0 * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - depth * (FAR_PLANE - NEAR_PLANE));
    return (z - NEAR_PLANE)/(FAR_PLANE - NEAR_PLANE);
}

// ������Ӱ֮��: 0.0
// ���򷵻� 1.0
float ShadowCalculation(vec4 fragPosLightSpace) {
    // ת��Ϊ��׼�������, z:[-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // xyz:[-1, 1] => [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    // �����ͼ�л�ȡ�����Ϣ
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // ��ǰƬԪ�����
    float currentDepth = projCoords.z;
    // �ж��Ƿ�����Ӱ����
    float shadow = currentDepth-BIAS > closestDepth  ? 1.0 : 0.0;
    return 1.0 - shadow;
}

// PCF �㷨
float PCF(vec4 fragPosLightSpace, float radius) {
    // ������������
    radius *= PCF_SampleRadius;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float shadow = 0.0;
    float currentDepth = projCoords.z;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0); // 0 �������������صľ���
    for(int x = -PCF_RADIUS; x <= PCF_RADIUS; ++x) {
        for(int y = -PCF_RADIUS; y <= PCF_RADIUS; ++y) {
            float shadowMapDepth = texture(depthMap, projCoords.xy + radius*vec2(x, y) * texelSize).r; 
            shadow += currentDepth-BIAS > shadowMapDepth  ? 1.0 : 0.0;
        }
    }
    float total = (2*PCF_RADIUS+1);
    return 1.0 - shadow/(total*total);
}

// PCSS �㷨

/**
 * uv: depthMap �е�����
 * zRecerver: ����õ����������ֵ [0, 1]
 */
float findBlocker(vec2 uv, float zReceiver) {
    int blockers = 0;
    float ret = 0.0;
    float r = lightWidth * (zReceiver - NEAR_PLANE/FAR_PLANE) / zReceiver;
    // ��ɢ��������
    r *= SMDiffuse;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0); // 0 �������������صľ���
    for(int x = -BLOCK_RADIUS; x <= BLOCK_RADIUS; ++x) {
        for(int y = -BLOCK_RADIUS; y <= BLOCK_RADIUS; ++y) {
            // [0, 1]
            float shadowMapDepth = texture(depthMap, uv + r*vec2(x, y) * texelSize).r;
            // [0, 1] => [-1, 1]
            shadowMapDepth = getLinearizeDepth(shadowMapDepth * 2.0 - 1.0);
            if(zReceiver - BIAS > shadowMapDepth) {
                ret += shadowMapDepth;
                ++blockers;
            }
        }
    }
    // û�� blocker 
    if(blockers == 0) {
        return -1.0;
    }
    return ret/blockers;
}

float PCSS(vec4 fragPosLightSpace){
    
    // depthMap �е�����
    // => [-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // ת��Ϊ�������
    float depth = getLinearizeDepth(projCoords.z);
    // [-1, 1] => [0, 1]
    projCoords = projCoords * 0.5 + 0.5;
    // STEP 1: avgblocker depth
    float avgDepth = findBlocker(projCoords.xy, depth);
    // û���ڵ���
    if(avgDepth == -1.0) {
        return 1.0; 
    }

    // STEP 2: penumbra size
    float penumbra = (depth - avgDepth) / avgDepth * lightWidth;
    float filterRadius = penumbra * NEAR_PLANE / (depth);

    // STEP 3: filtering
    return PCF(fs_in.FragPosLightSpace, filterRadius);
}


// VSM �㷨
float VSM(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // ת��Ϊ�������
    float depth = getLinearizeDepth(projCoords.z);
    // [-1, 1] => [0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    vec2 d_d2 = texture(d_d2_filter, projCoords.xy).rg;
    float var = d_d2.y - d_d2.x * d_d2.x; // E(X-EX)^2 = EX^2-E^2X

    // �����㲻��ʽ, ֱ�ӿɼ�
    if(depth - BIAS < d_d2.x){
        return 1.0;
    }
    else{
        float t_minus_mu = depth - d_d2.x;
        return var/(var + t_minus_mu*t_minus_mu);
    }
}


void main() {
    // ������ ambient
    vec3 ambient = 0.2 * color;
    
    // ������� diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    // ����߹� specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;

    // Blinn-Phong ģ�Ϳ��Ƿ��ߺͰ��ʸ���ļн�
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec;


    float visibility = 1.0f;
    if(SMType == 1) {
        visibility = ShadowCalculation(fs_in.FragPosLightSpace);
    } else if(SMType == 2) {
        visibility = PCF(fs_in.FragPosLightSpace, 1.0);
    } else if(SMType == 3) {
        visibility = PCSS(fs_in.FragPosLightSpace);
    } else if(SMType == 4) {
        visibility = VSM(fs_in.FragPosLightSpace);
    }
    
    FragColor = vec4(ambient + visibility*(diffuse + specular), 1.0);
}