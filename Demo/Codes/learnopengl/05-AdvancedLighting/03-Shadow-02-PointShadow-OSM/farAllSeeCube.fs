#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture; // ͼƬ����
uniform samplerCube depthMap; // �����������ͼ
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;
uniform float far_plane; // Զƽ��λ��
uniform int onlyAmbient; // ֻ����������

// ������Ӱ֮��: 1.0
// ���򷵻� 0.0
float ShadowCalculation(vec3 fragPos) {
    
    // ���������������ֵ
    vec3 fragToLight = fragPos - lightPos; 
    float closestDepth = texture(depthMap, fragToLight).r;

    // depthCube.fs ��������������
    closestDepth *= far_plane;
    
    // ���㵱ǰ������
    float currentDepth = length(fragToLight);
    
    // ��Ȳ���
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    
    // Զ��ʼ�տɼ� 
    if(currentDepth > far_plane) {
        shadow = 0.0;
    }
    // ��ʾ shadow map
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
    return shadow;
}

void main() {
    // �����Ƿ�����Ӱ֮��
    float shadow = ShadowCalculation(fs_in.FragPos);  

    // ������ɫ
    vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
    
    // ������ ambient
    vec3 ambient = 0.3 * color;
    
    // ������� diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0); // diff = N \cdot L
    vec3 diffuse = diff * color;
    
    // ����߹� specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
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
    FragColor = vec4(ambient + (1-onlyAmbient) * (1-shadow)*(diffuse + specular)*vec3(0.3), 1.0);
}