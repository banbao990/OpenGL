#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
uniform float offset;

void main() {
    // 如果害怕计算的时候由于精度导致的误差
    // 可以将 interval 区间设置的大一点, 然后判断结果是否在 [-1.75, 1.75] 内
    // 若不在, 设置 aPos.x = -1.5f(屏幕外)
    // 若在, 正常绘制
    // 但是 1.75f 是可以准确表示的, 因此误差问题也还好
    
    // 注意这里需要处理同一个三角形中, 有些点在左边, 有些点在右边的问题
    // 2.0 表示控制速度
    const float SPEED_CONTROL = 2.0f;
    // offset 区间大小刚好是满足存在点在屏幕内的区间
    const float INTERVAL = 1.75f;
    const float INTERVALS = INTERVAL*2;
    // 三角形的宽度为 0.5
    float temp = offset/SPEED_CONTROL;
    temp = temp - INTERVALS*floor(temp/INTERVALS) - INTERVAL;
    // 以上计算让 temp 处于区间 [-INTERVAL, INTERVAL] 中    

    gl_Position = vec4(aPos.x + temp, aPos.yz, 1.0); 
    ourColor = aColor;
}