const std::string BoneTransformVert=
  "#version 330 core\n"
  "uniform mat4 boneTrans[128];\n"
  "layout(location=0) in vec3 Vertex;\n"
  "layout(location=1) in vec3 Normal;\n"
  "layout(location=4) in ivec4 BoneId;\n"
  "layout(location=5) in vec4 BoneWeight;\n"
  "out vec3 VertexOut;\n"
  "out vec3 NormalOut;\n"
  "void main(void)\n"
  "{\n"
  "  mat4 trans=mat4(0.0);\n"
  "  for(int i=0; i<4; i++)\n"
  "    if(BoneId[i]>=0)\n"
  "      trans+=boneTrans[BoneId[i]]*BoneWeight[i];\n"
  "  vec4 vout=trans*vec4(Vertex,1);\n"
  "  VertexOut=vout.xyz/vout.w;\n"
  "  NormalOut=normalize(mat3(trans)*Normal);\n"
  "}\n";
