#version 450

uniform sampler2D qt_Texture0;
varying vec4 qt_TexCoord0;

out vec4 out_color;
in vec4 color_from_vshader;

void main() {

        out_color = vec4(1.0, 1.0, 1.0, 1.0);
        //out_color = color_from_vshader;
}
