const std::string DebugDrawTexCoordFrag=
  "#version 330 core\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "void main(void)\n"
  "{\n"
  "  FragColor=vec4(tc,0,1);\n"
  "}\n";
