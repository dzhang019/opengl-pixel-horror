// Filter an input value to perform an even smoother step. This function should
// be a quintic polynomial with improved_smooth_step(0) = 0,
// improved_smooth_step(1) = 1, and zero first _and_ second derivatives at f=0
// and f=1. "Improving Noise" [Perlin 2002].
//
// Inputs:
//   f  input value
// Returns filtered output value
float improved_smooth_step( float f)
{
  /////////////////////////////////////////////////////////////////////////////
  f = clamp(f, 0.0, 1.0);
  f = 6*f*f*f*f*f - 15*f*f*f*f + 10*f*f*f;
  return f;
  /////////////////////////////////////////////////////////////////////////////
}
vec3 improved_smooth_step( vec3 f)
{
  /////////////////////////////////////////////////////////////////////////////
  // Replace with your code 
  f = clamp(f, 0.0, 1.0);
  return vec3(
    improved_smooth_step(f.x),
    improved_smooth_step(f.y),
    improved_smooth_step(f.z)
  );
  /////////////////////////////////////////////////////////////////////////////
}
