#version 330 core

uniform mat4 mvp;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

out vec3 color;
out vec3 normal;
out vec3 position;

void main(){
  vec4 pos = vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0);
  gl_Position = vec4(mvp * pos);	
  color = vertexColor;
  normal = vertexNormal;
  position = vertexPosition;
}




