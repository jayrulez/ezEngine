#include <RHIDX11PCH.h>

EZ_STATICLINK_LIBRARY(RHIDX11)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(RHIDX11_Device_Implementation_GraphicsDeviceDX11);
}
