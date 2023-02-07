const std::string RoundPointVert=
  "#version 330 core\n"
  "uniform float pointSize;\n"
  "uniform mat4 modelViewProjectionMatrix;\n"
  "layout(location=0) in vec3 Vertex;\n"
  "layout(location=1) in vec3 Normal;\n"
  "layout(location=2) in vec2 TexCoord;\n"
  "out vec2 tc;\n"
  "void main(void)\n"
  "{\n"
  "  gl_Position=modelViewProjectionMatrix*vec4(Vertex,1);\n"
  "  gl_PointSize=pointSize;\n"
  "  tc=TexCoord;\n"
  "}\n";
