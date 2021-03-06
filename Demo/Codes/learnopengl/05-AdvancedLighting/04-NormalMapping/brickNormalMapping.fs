#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture; // ͼƬ����
uniform sampler2D normalTexture; // ��������
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;
uniform bool normalMappingOn;
uniform int onlyAmbient; // ֻ����������

void main() {
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal;
    if(normalMappingOn) {
        normal = normalize(2.0 * texture(normalTexture, fs_in.TexCoords).rgb - 1.0);
    } else {
        normal = normalize(fs_in.Normal);
    }

    // ������ɫ
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    
    // ������ ambient
    vec3 ambient = 0.3 * color;
    
    // ������� diffuse
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
    FragColor = vec4(ambient + (1-onlyAmbient)*(diffuse + specular)*vec3(0.3), 1.0);
}