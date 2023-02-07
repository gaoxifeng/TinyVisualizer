const std::string DefaultFrag=
  "#version 330 core\n"
  "uniform vec4 diffuse;\n"
  "uniform bool useTexture;\n"
  "uniform sampler2D diffuseMap;\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "void main(void)\n"
  "{\n"
  "  if(useTexture)\n"
  "    FragColor=diffuse*texture(diffuseMap,tc);\n"
  "  else FragColor=diffuse;\n"
  "}\n";
