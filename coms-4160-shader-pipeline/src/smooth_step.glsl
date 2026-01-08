// Filter an input value to perform a smooth step. This function should be a
// cubic polynomial with smooth_step(0) = 0, smooth_step(1) = 1, and zero first
// derivatives at f=0 and f=1. 
//
// Inputs:
//   f  input value
// Returns filtered output value
float smooth_step(float f)
{
  /////////////////////////////////////////////////////////////////////////////
  // Replace with your code 
  f = clamp(f, 0.0, 1.0);
  f = -2.0 * f * f * f + 3.0 * f * f;
  return f;
  /////////////////////////////////////////////////////////////////////////////
}

vec3 smooth_step( vec3 f)
{
  /////////////////////////////////////////////////////////////////////////////
  // Replace with your code 
  f = clamp(f, 0.0, 1.0);
  return vec3(
    smooth_step(f.x),
    smooth_step(f.y),
    smooth_step(f.z)
  );
  /////////////////////////////////////////////////////////////////////////////
}
