set(fpzip_source
  codec.h
  error.cpp
  fpe.h fpe.inl
  front.h
  pccodec.h pccodec.inl
  pcdecoder.h pcdecoder.inl
  pcencoder.h pcencoder.inl
  pcmap.h pcmap.inl
  rcdecoder.cpp rcdecoder.h rcdecoder.inl
  rcencoder.cpp rcencoder.h rcencoder.inl
  rcmodel.h
  rcqsmodel.cpp rcqsmodel.h rcqsmodel.inl
  read.cpp read.h
  types.h
  version.cpp
  write.cpp write.h)

add_library(fpzip ${fpzip_source})
add_library(fpzip::fpzip ALIAS fpzip)

if(HAVE_LIBM_MATH)
  target_link_libraries(fpzip PRIVATE m)
endif()

if(WIN32 AND BUILD_SHARED_LIBS)
  # Define FPZIP_SOURCE when compiling libfpzip to export symbols to Windows DLL
  list(APPEND fpzip_public_defs FPZIP_SHARED_LIBS)
  list(APPEND fpzip_private_defs FPZIP_SOURCE)
endif()

target_compile_definitions(fpzip
  PRIVATE ${fpzip_private_defs}
  PUBLIC ${fpzip_public_defs}
)

target_include_directories(fpzip
  PUBLIC
    $<BUILD_INTERFACE:${FPZIP_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_property(TARGET fpzip PROPERTY VERSION ${FPZIP_VERSION})
set_property(TARGET fpzip PROPERTY SOVERSION ${FPZIP_VERSION_MAJOR})
set_property(TARGET fpzip PROPERTY OUTPUT_NAME ${FPZIP_LIBRARY_PREFIX}fpzip)

install(TARGETS fpzip EXPORT fpzip-targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
