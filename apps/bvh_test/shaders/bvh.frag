#version 450 core

#define FLT_MAX 3.402823466e+38

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 frag;

struct Triangle {
    vec3 v0;
    vec3 v1;
    vec3 v2;
};

struct BvhNode {
    vec3 aabbMin;
    uint leftFirst;
    vec3 aabbMax;
    uint triCount;
};

struct Ray {
    vec3 o;
    vec3 d;
    vec3 rD;
    float t;
};

layout (std430, binding = 0) readonly buffer Triangles {
    Triangle triangles[];
};

layout (std430, binding = 1) readonly buffer TriangleIDs {
    uint triangleIDs[];
};

layout (std430, binding = 2) readonly buffer BvhNodes {
    BvhNode bvhNodes[];
};

// layout (std430, binding = 3) buffer ret_val {
//     float val[1];
// };

uniform float aspectRatio;

// --------------------------------------------
// Random stuff
// --------------------------------------------
// initializes random seed
uint pcg_hash(uint seed);
// steps thru the seed
uint rand_pcg(inout uint rng_state);
// returns a rand float
float rand(inout uint state);

Ray newRay(vec3 o, vec3 d);
vec3 rayAt(Ray ray);
bool bvhNodeIsLeaf(BvhNode node);
void intersectTriangle(inout Ray ray, Triangle triangle);
float intersectBoundingBox(inout Ray ray, vec3 min, vec3 max);
void intersect(inout Ray ray, uint nodeID);

bool closestHit(inout Ray ray);

void main() {
    uint seed = pcg_hash(gl_PrimitiveID);

    float theta = 0.785398;   // vfov 45
    float h = tan(theta / 2);
    float vpHeight = 2.0 * h;
    float vpWidth = aspectRatio * vpHeight;

    vec3 lookFrom = vec3(0, 0, -1);

    vec3 w = normalize(lookFrom - vec3(0, 0, 1));
    vec3 u = normalize(cross(vec3(0, 1, 0), w));
    vec3 v = cross(w, u);

    vec3 o = lookFrom;
    vec3 horizontal = vpWidth * u;
    vec3 vertical = vpHeight * v;
    vec3 llc = o - horizontal / 2 - vertical / 2 - w;

    Ray ray = newRay(o, llc + uv.x * horizontal + uv.y * vertical - o);

    if (closestHit(ray)) {
        frag = vec4(1, 1, 1, 1);
    } else {
        frag = vec4(uv, 0, 1);
    }

}

uint pcg_hash(uint seed) {
  uint state = seed * 747796405u + 2891336453u;
  uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

// Used to advance the PCG state.
uint rand_pcg(inout uint rng_state) {
  uint state = rng_state;
  rng_state = rng_state * 747796405u + 2891336453u;
  uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

// Advances the prng state and returns the corresponding random float.
float rand(inout uint state) {
  uint x = rand_pcg(state);
  state = x;
  return float(x)*uintBitsToFloat(0x2f800004u);
}

Ray newRay(vec3 o, vec3 d) {
    Ray r;
    r.o = o;
    r.d = d;
    r.rD = vec3(1 / d.x, 1 / d.y, 1 / d.z);
    r.t = FLT_MAX;
    return r;
}

vec3 rayAt(Ray ray) {
    return ray.o + (ray.d * ray.t);
}

bool bvhNodeIsLeaf(BvhNode node) {
    return node.triCount > 0;
}

void intersectTriangle(inout Ray ray, Triangle triangle) {
    vec3 edge1 = triangle.v1 - triangle.v0;
    vec3 edge2 = triangle.v2 - triangle.v0;
    vec3 h = cross(ray.d, edge2);
    float a = dot(edge1, h);
    if (abs(a) < 0.0001f) return;
    float f = 1.0f / a;
    vec3 s = ray.o - triangle.v0;
    float u = f * dot(s, h);
    if (u < 0 || u > 1) return;
    vec3 q = cross(s, edge1);
    float v = f * dot(ray.d, q);
    if (v < 0 || u + v > 1) return;
    float t = f * dot(edge2, q);
    if (t < 0.0001f) return;
    if (ray.t < t) return;  // if the ray contains a better/closer intersection, then leave and dont update anything
    ray.t = t;  // only need to update t
}

float intersectBoundingBox(inout Ray ray, vec3 min_, vec3 max_) {
    float tmin, tmax;
    float tx1 = (min_.x - ray.o.x) * ray.rD.x, tx2 = (max_.x - ray.o.x) * ray.rD.x;
    tmin = min(tx1, tx2); 
    tmax = max(tx1, tx2);
    float ty1 = (min_.y - ray.o.y) * ray.rD.y, ty2 = (max_.y - ray.o.y) * ray.rD.y;
    tmin = max(tmin, min(ty1, ty2)); 
    tmax = min(tmax, max(ty1, ty2));
    float tz1 = (min_.z - ray.o.z) * ray.rD.z, tz2 = (max_.z - ray.o.z) * ray.rD.z;
    tmin = max(tmin, min(tz1, tz2)); 
    tmax = min(tmax, max(tz1, tz2));
    if (tmax >= tmin && tmin < ray.t && tmax > 0) return tmin;
    else return FLT_MAX;
}

void intersect(inout Ray ray, uint nodeID) {
    BvhNode node = bvhNodes[nodeID];
    BvhNode stack[64];
    int stackPtr = 0;
    while (true) {
        if (bvhNodeIsLeaf(node)) {
            for (int i = 0; i < node.triCount; i++) {
                intersectTriangle(ray, triangles[triangleIDs[node.leftFirst + i]]);
            }
            if (stackPtr == 0) {
                break;
            } else {
                stackPtr -= 1;
                node = stack[stackPtr];
            }
            continue;
        }
        BvhNode child1 = bvhNodes[node.leftFirst];
        BvhNode child2 = bvhNodes[node.leftFirst + 1];

        float dist1 = intersectBoundingBox(ray, child1.aabbMin, child1.aabbMax);
        float dist2 = intersectBoundingBox(ray, child2.aabbMin, child2.aabbMax);

        if (dist1 > dist2) {
            float temp = dist1;
            dist1 = dist2;
            dist2 = temp;

            BvhNode tempNode = child1;
            child1 = child2;
            child2 = tempNode;
        }

        if (dist1 == FLT_MAX) {
            if (stackPtr == 0) {
                break;
            } else {
                stackPtr -= 1;
                node = stack[stackPtr];
            }
        } else {
            node = child1;
            if (dist2 != FLT_MAX) {
                stack[stackPtr] = child2;
                stackPtr += 1;
            }
        }

    }   
}

bool closestHit(inout Ray ray) {
    intersect(ray, 0);
    if (ray.t == FLT_MAX) return false;
    return true;
}