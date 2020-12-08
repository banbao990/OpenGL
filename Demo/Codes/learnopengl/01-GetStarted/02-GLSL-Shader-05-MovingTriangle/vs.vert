#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
uniform float offset;

void main() {
    // ������¼����ʱ�����ھ��ȵ��µ����
    // ���Խ� interval �������õĴ�һ��, Ȼ���жϽ���Ƿ��� [-1.75, 1.75] ��
    // ������, ���� aPos.x = -1.5f(��Ļ��)
    // ����, ��������
    // ���� 1.75f �ǿ���׼ȷ��ʾ��, ����������Ҳ����
    
    // ע��������Ҫ����ͬһ����������, ��Щ�������, ��Щ�����ұߵ�����
    // 2.0 ��ʾ�����ٶ�
    const float SPEED_CONTROL = 2.0f;
    // offset �����С�պ���������ڵ�����Ļ�ڵ�����
    const float INTERVAL = 1.75f;
    const float INTERVALS = INTERVAL*2;
    // �����εĿ��Ϊ 0.5
    float temp = offset/SPEED_CONTROL;
    temp = temp - INTERVALS*floor(temp/INTERVALS) - INTERVAL;
    // ���ϼ����� temp �������� [-INTERVAL, INTERVAL] ��    

    gl_Position = vec4(aPos.x + temp, aPos.yz, 1.0); 
    ourColor = aColor;
}