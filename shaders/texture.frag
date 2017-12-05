#version 330 core
in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 textpos;
out vec4 out_color;

uniform sampler2D myTexture;

void main(){
   vec3 L = normalize(vec3(0.0, 5.0, 5.0) - position);
   float c = 0.5*max(dot(normal,L), 0.0);
   vec4 col;
   col.brg = color.brg;
   col.a = 0;
   vec4 texel = texture2D(myTexture, textpos);
  if(texel.a < 0.5)
    discard;
   col = col * texel.rgba;
   out_color.rgba = col.bgra;
}
