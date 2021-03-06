#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseTexture; // ͼƬ����
uniform sampler2D normalTexture; // ��������
uniform sampler2D depthTexture; // �߶�����
uniform bool blinn;
uniform bool normalMappingOn;
uniform bool parallaxMappingOn;
uniform int onlyAmbient; // ֻ����������
uniform vec3 lightPos; // ����ֱ�Ӵ��ݸ����� shader
uniform vec3 viewPos;

const float height_scale = 1.0f; // ���ԵĶϲ�

// parallax mapping
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
    float height =  texture(depthTexture, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}

// steep parallax mapping
vec2 ParallaxMapping2(vec2 texCoords, vec3 viewDir) {

    // �������(��������)
    const float numLayers = 10;

    // ����
    float layerDepth = 1.0 / numLayers;

    // ��ǰ������
    float currentLayerDepth = 0.0;

    // ����ǰ������
    vec2 P = viewDir.xy * height_scale;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthTexture, currentTexCoords).r;

    // �ӵ�ǰ�����, ���Ź��߷����ҵ���һ�����С�ڵ�ǰ����ȵĵ�
    // �߶ȸ� => ��ȵ�
    // û������, ���Ǹ����ͼ �� README �е�ͼ

    // ����Ǹ߶�ͼ�Ļ�, Ӧ���Ǵ���������, ���Ź��߷����ҵ���һ���ڵ���
    while(currentLayerDepth < currentDepthMapValue) {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthTexture, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    return currentTexCoords;
}

void main() {

    vec3 lightDir;
    vec3 viewDir;
    vec3 normal;

    if(normalMappingOn) {
        viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    } else {
        viewDir = normalize(viewPos - fs_in.FragPos);
    }

    vec2 nTexCoords; // �Ӳ���ͼƫ�ƺ����������
    if(parallaxMappingOn) {
         nTexCoords = ParallaxMapping2(fs_in.TexCoords,  viewDir);
         // ������Ե�Ĳ���(����ƫ�ƺ����)
         if(   nTexCoords.x > 1.0 || nTexCoords.y > 1.0
            || nTexCoords.x < 0.0 || nTexCoords.y < 0.0) {
            discard;
         }
    } else {
         nTexCoords = fs_in.TexCoords;
    }

    if(normalMappingOn) {
        normal = normalize(2.0 * texture(normalTexture, nTexCoords).rgb - 1.0);
        lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    } else {
        normal = normalize(fs_in.Normal);
        lightDir = normalize(lightPos - fs_in.FragPos);
    }

    // ������ɫ
    vec3 color = texture(diffuseTexture, nTexCoords).rgb;

    // ������ ambient
    vec3 ambient = 0.3 * color;

    // ������� diffuse
    float diff = max(dot(lightDir, normal), 0.0); // diff = N \cdot L
    vec3 diffuse = diff * color;

    // ����߹� specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn) {
        // Blinn-Phong ģ�Ϳ��Ƿ��ߺͰ��ʸ���ļн�
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    } else {
        // Phong ģ�Ϳ��Ƿ�����ߺ����ߵļн�
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = spec * color;
    // ��Ӱ��Ӱ�컷����
    // assuming bright white light color
    FragColor = vec4(ambient + (1-onlyAmbient)*(diffuse + specular)*vec3(0.3), 1.0);
}


// a^Ta = 1, M^T = M^{-1}
// =>
// (Ma)^T(Ma) = a^TM^TMa = a^T(M^TM)a = a^T(M^{-1}M)a = a^Ta=1