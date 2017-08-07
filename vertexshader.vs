#version 450

attribute vec4 qt_Vertex;
attribute vec4 qt_MultiTexCoord0;
uniform mat4 qt_ModelViewProjectionMatrix;
varying vec4 qt_TexCoord0;

uniform mat4 mvp_matrix;

in vec4 position;
in vec4 color;
out vec4 color_from_vshader;


void main() {
    gl_Position = mvp_matrix * position;
    color_from_vshader = color;

}
