# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/yougo/OneDrive/Desktop/KErrigan/unrealengine/build/c_lib/cryptopp-cmake/cryptopp"
  "C:/Users/yougo/OneDrive/Desktop/KErrigan/unrealengine/build/_deps/cryptopp-build"
  "C:/Users/yougo/OneDrive/Desktop/KErrigan/unrealengine/build/_deps/cryptopp-subbuild/cryptopp-populate-prefix"
  "C:/Users/yougo/OneDrive/Desktop/KErrigan/unrealengine/build/_deps/cryptopp-subbuild/cryptopp-populate-prefix/tmp"
  "C:/Users/yougo/OneDrive/Desktop/KErrigan/unrealengine/build/_deps/cryptopp-subbuild/cryptopp-populate-prefix/src/cryptopp-populate-stamp"
  "C:/Users/yougo/OneDrive/Desktop/KErrigan/unrealengine/build/_deps/cryptopp-subbuild/cryptopp-populate-prefix/src"
  "C:/Users/yougo/OneDrive/Desktop/KErrigan/unrealengine/build/_deps/cryptopp-subbuild/cryptopp-populate-prefix/src/cryptopp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/yougo/OneDrive/Desktop/KErrigan/unrealengine/build/_deps/cryptopp-subbuild/cryptopp-populate-prefix/src/cryptopp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/yougo/OneDrive/Desktop/KErrigan/unrealengine/build/_deps/cryptopp-subbuild/cryptopp-populate-prefix/src/cryptopp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
