#version 450 core
in vec2 vUv;
out vec4 FragColor;

uniform vec2 uResolution;
uniform vec2 uMouse; // 鼠标位置
uniform float uTime; // 如需动画可启用

const vec3 lightDir = normalize( vec3( 1.0, 1.0, 1.0 ) );
const float farPlane = 10000.0;

float ground( vec3 p )
{
  return sin( p.x * 0.001 ) * 500.0 + cos( p.z * 0.001 ) * 500.0 - 1000.0;
}

struct HitInfo
{
  vec3 pos;
  int type; // -1:未命中, 0:水面, 1:地面
  float dist;
};

HitInfo rayMarch( vec3 ro, vec3 rd )
{
  float t = 0.0;
  HitInfo hit;
  hit.type = -1;

  for ( int i = 0; i < 256; i++ ) { // 最大步进次数优化
    vec3 p = ro + rd * t;
    float h = ground( p );

    if ( p.y < -800.0 ) { // 水面
      hit.pos = p;
      hit.type = 0;
      hit.dist = t;
      return hit;
    }

    float d = p.y - h; // 到地面的距离
    if ( d < 0.01 ) {  // 地面命中
      hit.pos = p;
      hit.type = 1;
      hit.dist = t;
      return hit;
    }

    // 自适应步长
    t += max( d * 0.5, 0.001 * t );
    if ( t > farPlane )
      break;
  }
  return hit;
}

vec3 calcNormal( vec3 p )
{
  vec2 eps = vec2( 0.01, 0.0 );
  return normalize( vec3( ground( p - eps.xyy ) - ground( p + eps.xyy ),
                          0.02,
                          ground( p - eps.yyx ) - ground( p + eps.yyx ) ) );
}

vec3 rotateRD( vec3 ro, vec3 ta, vec3 rd )
{
  vec3 cw = normalize( ta - ro );
  vec3 cp = vec3( 0, 1, 0 );
  vec3 cu = normalize( cross( cw, cp ) );
  vec3 cv = normalize( cross( cu, cw ) );
  return vec3( rd.x * cu + rd.y * cv + rd.z * cw );
}

vec3 cameraPos = vec3( 0.0, 0.0, -sqrt( 3.0 ) );
vec3 targetPos = vec3( 0.0, 0.0, 50.0 );
vec2 mousePos = vec2( 0.0, 0.0 );
vec3 render( vec2 uv )
{
  // 坐标系转换
  vec2 screenUV = ( uv * 2.0 - 1.0 ) * vec2( uResolution.x / uResolution.y, 1.0 );

  // 构建光线方向
  vec3 rd = normalize( vec3( screenUV, sqrt( 3.0 ) ) );

  {
    float dx = uMouse.x / uResolution.x - mousePos.x;
    float dy = uMouse.y / uResolution.y - mousePos.y;
    targetPos.x -= dx * 10.0;
    targetPos.y -= dy * 10.0;
    mousePos = vec2( uMouse.x / uResolution.x, uMouse.y / uResolution.y );
  }
  rd = rotateRD( cameraPos, targetPos, rd );

  HitInfo hit = rayMarch( cameraPos, rd );

  if ( hit.type < 0 ) {
    return vec3( 0.0, 0.0, 1.0 ); // 天空
  }

  vec3 color;
  vec3 normal;

  if ( hit.type == 1 ) { // 地面
    normal = calcNormal( hit.pos );
    color = vec3( 0.7, 0.7, 0.5 );
  } else { // 水面
    normal = vec3( 0.0, 1.0, 0.0 );
    color = vec3( 0.0, 0.6, 0.7 );
  }

  // 光照计算
  float diff = max( dot( normal, lightDir ), 0.0 );
  return color * diff;
}

void main()
{
  vec3 color = render( vUv );
  FragColor = vec4( color, 1.0 );
}