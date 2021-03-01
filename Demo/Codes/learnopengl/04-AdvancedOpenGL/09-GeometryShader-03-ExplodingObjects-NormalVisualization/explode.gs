#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 Position;
} gs_in[];

out VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 Position;
} gs_out;

uniform float time;

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

vec3 GetNormal() {
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}

void generateVertexI(int i, vec3 normal) {
    gl_Position = explode(gl_in[i].gl_Position, normal);
    gs_out.TexCoords = gs_in[i].TexCoords;
    gs_out.Normal = gs_in[i].Normal;
    gs_out.Position = gs_in[i].Position;
}

void main() {    
    vec3 normal = GetNormal();

    generateVertexI(0, normal);
    EmitVertex();
    
    generateVertexI(1, normal);
    EmitVertex();
    
    generateVertexI(2, normal);
    EmitVertex();
    
    EndPrimitive();
}