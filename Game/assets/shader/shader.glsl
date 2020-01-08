#type vertex
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Camera {
    mat4 projViewMatrix;
} camera;

layout( push_constant ) uniform Transform {
  mat4 transform;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 texCoord;

void main() {
    gl_Position = camera.projViewMatrix * pushConstants.transform * vec4(inPosition, 1.0);
    texCoord = inTexCoord;
}



#type fragment
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D tex;

layout(location = 0) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(tex, texCoord);
}