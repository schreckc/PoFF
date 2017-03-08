#version 330 core
in vec3 position;
in vec3 normal;
in vec3 color;
out vec3 out_color;

void main(){
   vec3 L = normalize(vec3(4.0, 4.0, 50.0) - position);
   float c = 0.2*abs(dot(normal,L));
   out_color = (0.6 + c)*color;
}
