#version 330 core

uniform mat4 transform;		
uniform mat4 projectionTransform;		
layout (location = 0) in vec3 positionAttribute;
in vec3 line_attribute;
uniform vec3 colorAttribute;
out vec3 passColorAttribute;

void main()
{
	gl_Position = projectionTransform * transform * vec4(positionAttribute, 1.0);
	passColorAttribute = colorAttribute;
};