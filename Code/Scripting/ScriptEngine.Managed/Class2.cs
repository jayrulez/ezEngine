using System;

namespace ezEngine.Managed
{
  public static class Class2
  {
    private static int Count = 0;
    public static int GetCount()
    {
      return Count++;
    }
  }
}
