std::string ThickLineVert=
  "#version 330 core\n"
  "uniform mat4 modelViewProjectionMatrix;\n"
  "layout(location=0) in vec3 Vertex;\n"
  "layout(location=1) in vec3 Normal;\n"
  "layout(location=2) in vec2 TexCoord;\n"
  "void main(void) {"
  "  gl_Position=modelViewProjectionMatrix*vec4(Vertex,1);\n"
  "}\n";
