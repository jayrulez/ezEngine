if (TARGET RHISampleHighLevel AND TARGET RHIDX12)
  add_dependencies(RHISampleHighLevel RHIDX12)
endif()

if (TARGET RHISampleHighLevel AND TARGET RHIVulkan)
  add_dependencies(RHISampleHighLevel RHIVulkan)
endif()