# Copyright (C)
# 2012 - Paul Weingardt
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
#

IF(MYSQL_INCLUDE_DIR)
    SET(MYSQL_FIND_QUIETLY TRUE)
ENDIF(MYSQL_INCLUDE_DIR)

FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
    /usr/include/mysql
    /usr/local/include/mysql)

SET(MYSQL_NAMES mysqlclient mysqlclient_r)
FIND_LIBRARY(MYSQL_LIBRARY
    NAMES ${MYSQL_NAMES}
    PATHS /usr/lib /usr/local/lib
    PATH_SUFFIXES mysql)

IF(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
    SET(MYSQL_FOUND TRUE)
    SET(MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
ELSE(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
    SET(MYSQL_FOUND FALSE)
ENDIF(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

IF(MYSQL_FOUND)
    IF(NOT MYSQL_FIND_QUIETLY)
        MESSAGE(STATUS "Found MySQL: ${MYSQL_LIBRARY}")
    ENDIF(NOT MYSQL_FIND_QUIETLY)
ELSE(MYSQL_FOUND)
    IF(MYSQL_FIND_REQUIRED)
        MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
        MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
    ELSE(MYSQL_FIND_REQUIRED)
        MESSAGE(STATUS "Optional library MySQL not found")
    ENDIF(MYSQL_FIND_REQUIRED)
ENDIF(MYSQL_FOUND)

MARK_AS_ADVANCED(MYSQL_LIBRARY MYSQL_INCLUDE_DIR MYSQL_FOUND)
