#version 460 core
out vec4 FragColor;

// gl_FragCoord.z �����������Ϣ
// �����Ա任, ����ȫΪ��ɫ
// ��zֵ��С��ʱ���кܸߵľ���, ��zֵ�ܴ��ʱ���нϵ͵ľ���
// �������С�ĵؿ�������, ���Կ�����ɫ�ὥ���䰵, ��ʾ���ǵ�zֵ���𽥱�С

void main() {
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}