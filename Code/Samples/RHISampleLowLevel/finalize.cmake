if (TARGET RHISampleLowLevel AND TARGET RHIVulkan)
  add_dependencies(RHISampleLowLevel RHIVulkan)
endif()