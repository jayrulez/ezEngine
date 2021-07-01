if (TARGET RHISampleLowLevel AND TARGET RHIDX12)
  add_dependencies(RHISampleLowLevel RHIDX12)
endif()

if (TARGET RHISampleLowLevel AND TARGET RHIVulkan)
  add_dependencies(RHISampleLowLevel RHIVulkan)
endif()