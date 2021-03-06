#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

void main() {
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
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}

// Phong ģ�͵�����
// �ھ���߹�����ı�Ե������һ�������ԵĶϲ�
// ������������ԭ���ǹ۲������ͷ���������ļнǲ��ܴ���90��
// �������Ľ��Ϊ����, �����������Ϊ0.0
// ����ܻ����, �����������߼нǴ��� 90 ��ʱ��Ӧ�ò�����յ��κι�Ŷ�, �����ⲻ��ʲô����
// Ȼ��, �����뷨����ֻ���������������
// ��������������ʱ��, ������ߺ͹�Դ�нǴ���90��, ��Դ�ᴦ�ڱ��ձ�����·�
// ���ʱ����յ������������ȷ��Ϊ 0.0
// ����, �ڿ��Ǿ���߹�ʱ, ���ǲ����ĽǶȲ����ǹ�Դ�뷨�ߵļн�, ���������뷴����������ļн�

// ������ڵ�����ķ���ȷǳ�Сʱ
// �������ľ���߹�뾶��������Щ�෴����Ĺ��߶����Ȳ����㹻���Ӱ��
// ����������¾Ͳ��ܺ������ǶԾ��������Ĺ���

// ���ʸ��������ʹ�ò��۹۲������ĸ�����
// �����������淨��֮��ļнǶ����ᳬ��90��(���ǹ�Դ�ڱ�������)

// һЩ����
// �����������淨�ߵļн�ͨ����С�ڹ۲��뷴�������ļн�
// ��������úͷ�����ɫ���Ƶ�Ч��
// �ͱ�����ʹ�� Blinn-Phong ģ��ʱ�����淴������ø���һ��
// ͨ�����ǻ�ѡ�������ɫʱ����ȷ����� 2 �� 4 ��