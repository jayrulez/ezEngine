if (TARGET RHISample AND TARGET RHIDX12)
  add_dependencies(RHISample RHIDX12)
endif()

if (TARGET RHISample AND TARGET RHIVulkan)
  add_dependencies(RHISample RHIVulkan)
endif()