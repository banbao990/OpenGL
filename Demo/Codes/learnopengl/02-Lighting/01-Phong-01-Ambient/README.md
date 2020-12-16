# README

+ 想要看取消环境光的效果的话，直接把 `01.frag` 修改为如下即可

```c++
#version 460 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    FragColor = vec4(lightColor * objectColor, 1.0);
}
```

