#version 330 core


uniform mat4 mvp;


layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

out vec3 color;
out vec3 position;

void main(){
  vec4 pos = vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0);
  gl_Position = vec4(mvp * pos);
  color = vertexColor;
  position = vertexPosition;
}




