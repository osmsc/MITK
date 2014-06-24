if( MITK_USE_Python AND NOT MITK_USE_SYSTEM_PYTHON )
 if(NOT DEFINED Python_DIR)
  set(proj Python)
  set(proj_DEPENDENCIES )
  set(Python_DEPENDS ${proj})

  set(PYTHON_MAJOR_VERSION 2)
  set(PYTHON_MINOR_VERSION 7)
  set(PYTHON_PATCH_VERSION 3)

  # download the source code
  ExternalProject_Add(Python-src
    URL "https://www.python.org/ftp/python/${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}.${PYTHON_PATCH_VERSION}/Python-${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}.${PYTHON_PATCH_VERSION}.tgz"
    #URL_MD5 "d41d8cd98f00b204e9800998ecf8427e"
    #DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
    SOURCE_DIR  "${CMAKE_BINARY_DIR}/Python-${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}.${PYTHON_PATCH_VERSION}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )

  set(additional_cmake_cache_args )
  list(APPEND additional_cmake_cache_args
      -DBUILTIN_ARRAY:BOOL=ON
      -DBUILTIN_AUDIOOP:BOOL=ON
      -DBUILTIN_BINASCII:BOOL=ON
      -DBUILTIN_BISECT:BOOL=ON
      -DBUILTIN_BSDB:BOOL=ON
      -DBUILTIN_BSSDB:BOOL=ON
      -DBUILTIN_BZ2:BOOL=ON
      -DBUILTIN_CMATH:BOOL=ON
      -DBUILTIN_COLLECTIONS:BOOL=ON
      -DBUILTIN_CODECS_CN:BOOL=ON
      -DBUILTIN_CODECS_HK:BOOL=ON
      -DBUILTIN_CODECS_ISO2022:BOOL=ON
      -DBUILTIN_CODECS_JP:BOOL=ON
      -DBUILTIN_CODECS_KR:BOOL=ON
      -DBUILTIN_CODECS_TW:BOOL=ON
      -DBUILTIN_CPICKLE:BOOL=ON
      -DBUILTIN_CRYPT:BOOL=ON
      -DBUILTIN_CSTRINGIO:BOOL=ON
      -DBUILTIN_CSV:BOOL=ON
      -DBUILTIN_CTYPES:BOOL=OFF
      #-DBUILTIN_CTYPES_TEST:BOOL=OFF
      #-DBUILTIN_CURSES:BOOL=ON
      -DBUILTIN_DATETIME:BOOL=ON
      -DBUILTIN_DBM:BOOL=ON
      -DBUILTIN_ELEMENTTREE:BOOL=ON
      -DBUILTIN_FCNTL:BOOL=ON
      -DBUILTIN_FUNCTOOLS:BOOL=ON
      -DBUILTIN_FUTURE_BUILTINS:BOOL=ON
      -DBULTIN_GDBM:BOOL=ON
      -DBUILTIN_GRP:BOOL=ON
      -DBUILTIN_HASHLIB:BOOL=ON
      -DBUILTIN_HEAPQ:BOOL=ON
      -DBUILTIN_HOTSHOT:BOOL=ON
      -DBUILTIN_IO:BOOL=ON
      -DBUILTIN_ITERTOOLS:BOOL=ON
      -DBUILTIN_JSON:BOOL=ON
      -DBUILTIN_LOCALE:BOOL=ON
      -DBUILTIN_LSPROF:BOOL=ON
      -DBUILTIN_MATH:BOOL=ON
      -DBUILTIN_MMAP:BOOL=ON
      -DBUILTIN_MULTIBYTECODEC:BOOL=ON
      -DBUILTIN_MD5:BOOL=ON
      -DBUILTIN_MULTIPROCESSING:BOOL=ON
      -DBUILTIN_NIS:BOOL=ON
      -DBUILTIN_NIT:BOOL=ON
      -DBUILTIN_OPERATOR:BOOL=ON
      -DBUILTIN_PARSER:BOOL=ON
      -DBUILTIN_POSIX:BOOL=ON
      -DBUILTIN_PWD:BOOL=ON
      -DBUILTIN_PYEXPAT:BOOL=ON
      -DBUILTIN_READLINE:BOOL=ON
      -DBUILTIN_RESOURCE:BOOL=ON
      -DBULTIN_RANDOM:BOOL=ON
      -DBUILTIN_SCPROXY:BOOL=ON
      -DBUILTIN_SELECT:BOOL=ON
      -DBUILTIN_SHA:BOOL=ON
      -DBUILTIN_SHA256:BOOL=ON
      -DBUILTIN_SHA512:BOOL=ON
      -DBUILTIN_SOCKET:BOOL=ON
      -DBUILTIN_SPWD:BOOL=ON
      -DBUILTIN_SQLITE3:BOOL=ON
      -DBUILTIN_SSL:BOOL=ON
      -DBUILTIN_STROP:BOOL=ON
      -DBUILTIN_STRUCT:BOOL=ON
      -DBUILTIN_SUBPROCESS:BOOL=ON
      -DBUILTIN_SYSLOG:BOOL=ON
      -DBUILTIN_TERMIOS:BOOL=ON
      #-DBUILTIN_TESTCAPI:BOOL=OFF
      -DBUILTIN_TIME:BOOL=ON
      -DBUILTIN_TKINTER:BOOL=ON
      -DBUILTIN_UNICODEDATA:BOOL=ON
      -DBUILTIN_WINREG:BOOL=ON
      -DBUILTIN_ZLIB:BOOL=ON
      )

  # CMake build environment for python
  ExternalProject_Add(${proj}
    GIT_REPOSITORY "git://github.com/davidsansome/python-cmake-buildsystem.git"
    GIT_TAG "3c5864f210a8d0ae1196be7c691252e16e459f59"
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}-src
    PREFIX ${proj}-cmake
    BINARY_DIR ${proj}-build
    INSTALL_DIR ${proj}-install
    CMAKE_ARGS
      ${ep_common_args}
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    CMAKE_CACHE_ARGS
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      #-DBUILD_TESTING:BOOL=OFF
      -DBUILD_SHARED:BOOL=ON
      -DBUILD_STATIC:BOOL=OFF
      -DUSE_SYSTEM_LIBRARIES:BOOL=ON
      ${additional_cmake_cache_args}
    DEPENDS
      Python-src ${${proj}_DEPENDENCIES}
  )

  set(Python_DIR "${CMAKE_BINARY_DIR}/${proj}-install")
  if(UNIX)
    set(PYTHON_EXECUTABLE "${CMAKE_BINARY_DIR}/${proj}-build/bin/python${CMAKE_EXECUTABLE_SUFFIX}")
    set(PYTHON_INCLUDE_DIR "${Python_DIR}/include/python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}")
    set(PYTHON_LIBRARY "${CMAKE_BINARY_DIR}/${proj}-build/lib/${CMAKE_SHARED_LIBRARY_PREFIX}python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  else(WIN32)
    set(PYTHON_EXECUTABLE "${Python_DIR}/bin/python${CMAKE_EXECUTABLE_SUFFIX}")
    set(PYTHON_INCLUDE_DIR "${Python_DIR}/include")
    set(PYTHON_LIBRARY "${Python_DIR}/libs/python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}.lib")
  endif()
 else()
  mitkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
 endif()
endif()

