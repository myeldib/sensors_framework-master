


macro(TARGET_LINK_ARCHIVES TARGET)
  foreach(archive ${ARGN})
    target_link_libraries(${TARGET} "-Wl,--whole-archive" ${archive} "-Wl,--no-whole-archive" )
  endforeach(archive)
endmacro(TARGET_LINK_ARCHIVES)


macro(SET_RPATH_INSTALL_DIR TARGET)
  # Macro to fix rpath
  #   TARGET   - Install target

  # use, i.e. don't skip the full RPATH for the build tree
  set(CMAKE_SKIP_BUILD_RPATH  FALSE)
  # when building, don't use the install RPATH already
  # (but later on when installing)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH ${TARGET})
  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  # the RPATH to be used when installing, but only if it's not a system directory
  set(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${TARGET}" isSystemDir)
  if("${isSystemDir}" STREQUAL "-1")
     set(CMAKE_INSTALL_RPATH "${TARGET}")
  endif("${isSystemDir}" STREQUAL "-1")
endmacro(SET_RPATH_INSTALL_DIR TARGET)


# test if a value apperas in a list
macro(LIST_CONTAINS var value)
  set(${var})
  foreach(value2 ${ARGN})
    if(${value} STREQUAL ${value2})
      set(${var} TRUE)
    endif(${value} STREQUAL ${value2})
  endforeach(value2)
endmacro(LIST_CONTAINS)

# LIST_FILTER(<list> <regexp_var> [<regexp_var> ...]
#              [OUTPUT_VARIABLE <variable>])
# Removes items from <list> which do not match any of the specified
# regular expressions. An optional argument OUTPUT_VARIABLE
# specifies a variable in which to store the matched items instead of
# updating <list>
# As regular expressions can not be given to macros (see bug #5389), we pass
# variable names whose content is the regular expressions.
macro(LIST_FILTER)
  # Check arguments.
  cmake_parse_arguments(LIST_FILTER "REMOVE" "OUTPUT_VARIABLE" "" ${ARGV})
  list(LENGTH LIST_FILTER_UNPARSED_ARGUMENTS  LIST_FILTER_default_length)
  if("${ARGV0}" STREQUAL "")
    message(FATAL_ERROR "LIST_FILTER: missing list variable.")
  endif("${ARGV0}" STREQUAL "")
  if(${LIST_FILTER_default_length} EQUAL 0)
    message(FATAL_ERROR "LIST_FILTER: missing regular expression variable.")
  endif(${LIST_FILTER_default_length} EQUAL 0)
  # Reset output variable
  if("${LIST_FILTER_OUTPUT_VARIABLE}" STREQUAL "")
    set(LIST_FILTER_OUTPUT_VARIABLE "LIST_FILTER_internal_output")
  endif("${LIST_FILTER_OUTPUT_VARIABLE}" STREQUAL "")
  set(${LIST_FILTER_OUTPUT_VARIABLE})
  # Extract input list from arguments
  list(GET LIST_FILTER_UNPARSED_ARGUMENTS 0 LIST_FILTER_input_list)
  list(REMOVE_AT LIST_FILTER_UNPARSED_ARGUMENTS 0)
  foreach(LIST_FILTER_item ${${LIST_FILTER_input_list}})
    foreach(LIST_FILTER_regexp_var ${LIST_FILTER_UNPARSED_ARGUMENTS})
      foreach(LIST_FILTER_regexp ${${LIST_FILTER_regexp_var}})
        if(${LIST_FILTER_item} MATCHES ${LIST_FILTER_regexp})
          if(NOT ${LIST_FILTER_REMOVE})
            list(APPEND ${LIST_FILTER_OUTPUT_VARIABLE} ${LIST_FILTER_item})
          endif(NOT ${LIST_FILTER_REMOVE})
        else(${LIST_FILTER_item} MATCHES ${LIST_FILTER_regexp})
          if(${LIST_FILTER_REMOVE})
            list(APPEND ${LIST_FILTER_OUTPUT_VARIABLE} ${LIST_FILTER_item})
          endif(${LIST_FILTER_REMOVE})
        endif(${LIST_FILTER_item} MATCHES ${LIST_FILTER_regexp})
      endforeach(LIST_FILTER_regexp ${${LIST_FILTER_regexp_var}})
    endforeach(LIST_FILTER_regexp_var)
  endforeach(LIST_FILTER_item)
  # If OUTPUT_VARIABLE is not specified, overwrite the input list.
  # else, set the variable in the parent scope
  if(${LIST_FILTER_OUTPUT_VARIABLE} STREQUAL "LIST_FILTER_internal_output")
    set(${LIST_FILTER_input_list} ${${LIST_FILTER_OUTPUT_VARIABLE}} PARENT_SCOPE)
  else(${LIST_FILTER_OUTPUT_VARIABLE} STREQUAL "LIST_FILTER_internal_output")
    set(${LIST_FILTER_OUTPUT_VARIABLE} ${${LIST_FILTER_OUTPUT_VARIABLE}} PARENT_SCOPE)
  endif(${LIST_FILTER_OUTPUT_VARIABLE} STREQUAL "LIST_FILTER_internal_output")
endmacro(LIST_FILTER)


#from: http://www.opensource.apple.com/source/curl/curl-68/curl/CMake/Utilities.cmake
# File containing various utilities

# Converts a CMake list to a string containing elements separated by spaces
macro(TO_LIST_SPACES _LIST_NAME OUTPUT_VAR)
  set(NEW_LIST_SPACE)
  foreach(ITEM ${${_LIST_NAME}})
    set(NEW_LIST_SPACE "${NEW_LIST_SPACE} ${ITEM}")
  endforeach()
  #string(STRIP ${NEW_LIST_SPACE} NEW_LIST_SPACE)
  set(${OUTPUT_VAR} "${NEW_LIST_SPACE}" PARENT_SCOPE)
endmacro()

# Appends a lis of item to a string which is a space-separated list, if they don't already exist.
macro(LIST_SPACES_APPEND_ONCE LIST_NAME)
  string(REPLACE " " ";" _LIST ${${LIST_NAME}})
  list(APPEND _LIST ${ARGN})
  list(REMOVE_DUPLICATES _LIST)
  to_list_spaces(_LIST NEW_LIST_SPACE)
  set(${LIST_NAME} "${NEW_LIST_SPACE}" PARENT_SCOPE)
endmacro()

# Convinience function that does the same as LIST(FIND ...) but with a TRUE/FALSE return value.
# Ex: IN_STR_LIST(MY_LIST "Searched item" WAS_FOUND)
macro(IN_STR_LIST LIST_NAME ITEM_SEARCHED RETVAL)
  list(FIND ${LIST_NAME} ${ITEM_SEARCHED} FIND_POS)
  if(${FIND_POS} EQUAL -1)
    set(${RETVAL} FALSE PARENT_SCOPE)
  else()
    set(${RETVAL} TRUE PARENT_SCOPE)
  endif()
endmacro()
