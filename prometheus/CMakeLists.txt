# where to put generated libraries
#set(LIBRARY_OUTPUT_PATH "${BUILD_OUTPUT_ROOT_DIRECTORY}/prometheus")

# where to put generated binaries
#set(EXECUTABLE_OUTPUT_PATH "${BUILD_OUTPUT_ROOT_DIRECTORY}/prometheus")

add_library(PrometheusCore
  check_names.cc
  counter.cc
  detail/builder.cc
  detail/ckms_quantiles.cc
  detail/time_window_quantiles.cc
  detail/utils.cc
  family.cc
  gauge.cc
  histogram.cc
  registry.cc
  serializer.cc
  summary.cc
  text_serializer.cc
)
