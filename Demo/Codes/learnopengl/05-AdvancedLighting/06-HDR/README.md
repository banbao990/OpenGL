# README

+ https://github.com/banbao990/CV/tree/main/HDR
+ 纹理文件 wood.png 在如下文件夹中
    + 05-AdvancedLighting/01-BlinnPhong



## 色调映射 Tone Mapping

+ Reinhard 色调映射

$$
\dfrac{hdrColor}{hdrColor + 1.0}
$$

+ exposure 指数色调映射

$$
1.0 - e^{-hdrColor\times exposure}
$$