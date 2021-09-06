const std::string ShadowLightFrag=
  "#version 330 core\n"
  "#extension GL_NV_shadow_samplers_cube : enable\n"
  "struct LightMaterial {\n"
  "  vec3 positionEye;\n"
  "  vec4 position;\n"
  "  vec4 ambient;\n"
  "  vec4 diffuse;\n"
  "  vec4 specular;\n"
  "  float shininess;\n"
  "};\n"
  "//shadow\n"
  "uniform sampler2D diffuseMap;\n"
  "uniform samplerCube depthMap[8];\n"
  "uniform float far_plane,bias;\n"
  "uniform bool softShadow;\n"
  "uniform bool useShadow;\n"
  "//light\n"
  "uniform float invShadowMapSize;\n"
  "uniform int softShadowPass;\n"
  "uniform LightMaterial lights[8];\n"
  "uniform int MAX_LIGHTS;\n"
  "in vec3 vN,v,v0;\n"
  "in vec2 tc;\n"
  "out vec4 FragColor;\n"
  "vec3 gridSamplingDisk[27]=vec3[]\n"
  "(\n"
  "  vec3(1, 1, 1), vec3( 0, 1, 1), vec3(-1, 1, 1),\n"
  "  vec3(1, 0, 1), vec3( 0, 0, 1), vec3(-1, 0, 1),\n"
  "  vec3(1,-1, 1), vec3( 0,-1, 1), vec3(-1,-1, 1),\n"
  "  vec3(1, 1, 0), vec3( 0, 1, 0), vec3(-1, 1, 0),\n"
  "  vec3(1, 0, 0), vec3( 0, 0, 0), vec3(-1, 0, 0),\n"
  "  vec3(1,-1, 0), vec3( 0,-1, 0), vec3(-1,-1, 0),\n"
  "  vec3(1, 1,-1), vec3( 0, 1,-1), vec3(-1, 1,-1),\n"
  "  vec3(1, 0,-1), vec3( 0, 0,-1), vec3(-1, 0,-1),\n"
  "  vec3(1,-1,-1), vec3( 0,-1,-1), vec3(-1,-1,-1)\n"
  ");\n"
  "float calcShadow(vec3 l,int lid)\n"
  "{\n"
  "  float shadow=0.;\n"
  "  float currentDepth=length(v0-l);\n"
  "  float closestDepth=texture(depthMap[lid],v0-l).r*far_plane;\n"
  "  if(currentDepth-bias>closestDepth)\n"
  "    shadow=1.;\n"
  "  return shadow;\n"
  "}\n"
  "float calcSoftShadow(vec3 l,int lid)\n"
  "{\n"
  "  float shadow=0.;\n"
  "  float currentDepth=length(v0-l);\n"
  "  vec3 dir=normalize(v0-l);\n"
  "  for(int pass=0;pass<softShadowPass;pass++)\n"
  "    for(int i=0;i<27;++i) {\n"
  "      float closestDepth=texture(depthMap[lid],dir+gridSamplingDisk[i]*invShadowMapSize*pass).r*far_plane;   // undo mapping [0;1]\n"
  "      if(currentDepth-bias>closestDepth)\n"
  "        shadow+=1.0;\n"
  "    }\n"
  "  shadow/=27*softShadowPass;\n"
  "  return shadow;\n"
  "}\n"
  "void main(void)\n"
  "{\n"
  "  vec3 N=normalize(vN);\n"
  "  vec4 colorMap=texture(diffuseMap,tc);\n"
  "  vec4 finalColor=vec4(0.0,0.0,0.0,0.0);\n"
  "  for (int i=0;i<MAX_LIGHTS;i++) {\n"
  "    vec3 L=normalize(lights[i].positionEye-v);\n"
  "    vec3 E=normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)\n"
  "    vec3 R=normalize(-reflect(L,N));\n"
  "    //calculate Ambient Term:\n"
  "    vec4 Iamb=lights[i].ambient*colorMap;\n"
  "    //calculate Diffuse Term:\n"
  "    vec4 Idiff=lights[i].diffuse*colorMap*max(dot(N,L),0.0);\n"
  "    Idiff=clamp(Idiff,0.0,1.0);\n"
  "    //calculate Specular Term:\n"
  "    vec4 Ispec=lights[i].specular*colorMap*pow(max(dot(R,E),0.0),lights[i].shininess);\n"
  "    Ispec=clamp(Ispec,0.0,1.0);\n"
  "    //shadow\n"
  "    if(useShadow) {\n"
  "      float s;\n"
  "      if(softShadow)\n"
  "        s=calcSoftShadow(lights[i].position.xyz,i);\n"
  "      else s=calcShadow(lights[i].position.xyz,i);\n"
  "      finalColor+=Iamb+(1-s)*(Idiff+Ispec);\n"
  "    } else finalColor+=Iamb+Idiff+Ispec;\n"
  "  }\n"
  "  //write Total Color:\n"
  "  FragColor=finalColor;\n"
  "}\n";