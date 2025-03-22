#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int ObjectID;

uniform vec3 color;
uniform int id;

void main()
{
    FragColor = vec4(color, 1.0);
    ObjectID = id;
}