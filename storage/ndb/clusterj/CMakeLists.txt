# Copyright (c) 2010, 2014, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

MESSAGE(STATUS "including ClusterJ")

IF("${WITH_CLASSPATH}" MATCHES "openjpa-(.+).jar")
  MESSAGE ( STATUS "Found openjpa jar" )
  SET(HAVE_OPENJPA TRUE)
ENDIF()

SET(DEP ndbjtie.jar
        jtie.jar
        clusterj-api.jar
        clusterj-core.jar
        clusterj-tie.jar)

SET(JARS ${CMAKE_CURRENT_BINARY_DIR}/../src/ndbjtie/ndbjtie.jar
         ${CMAKE_CURRENT_BINARY_DIR}/../src/ndbjtie/jtie/jtie.jar
         ${CMAKE_CURRENT_BINARY_DIR}/clusterj-api/clusterj-api.jar
         ${CMAKE_CURRENT_BINARY_DIR}/clusterj-core/clusterj-core.jar
         ${CMAKE_CURRENT_BINARY_DIR}/clusterj-tie/clusterj-tie.jar)

ADD_SUBDIRECTORY(clusterj-api)
ADD_SUBDIRECTORY(clusterj-core)
ADD_SUBDIRECTORY(clusterj-tie)

IF(HAVE_OPENJPA)
  ADD_SUBDIRECTORY(clusterj-openjpa)
  LIST(APPEND DEP clusterjpa.jar)
  LIST(APPEND JARS ${CMAKE_CURRENT_BINARY_DIR}/clusterj-openjpa/clusterjpa.jar)
ENDIF(HAVE_OPENJPA)

  ADD_SUBDIRECTORY(clusterj-test)
  IF(HAVE_OPENJPA)
    ADD_SUBDIRECTORY(clusterj-jpatest)
  ENDIF(HAVE_OPENJPA)

SET(CLUSTERJ_EXPORTS com.myblockchain.clusterj,com.myblockchain.clusterj.query,com.myblockchain.clusterj.annotation,com.myblockchain.clusterj.metadata,com.myblockchain.clusterj.spi,com.myblockchain.clusterj.store,com.myblockchain.clusterj.util,com.myblockchain.clusterj.tie)
CREATE_MANIFEST(clusterj_manifest.mf ${CLUSTERJ_EXPORTS} clusterj)

CREATE_JAR(clusterj
  MERGE_JARS ${JARS}
  MANIFEST ${CMAKE_CURRENT_BINARY_DIR}/clusterj_manifest.mf
  DEPENDENCIES ${DEP} )

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/clusterj-${JAVA_NDB_VERSION}.jar
        DESTINATION ${INSTALL_MYBLOCKCHAINSHAREDIR}/java COMPONENT ClusterJ)

# create maven pom.xml in all subdirectories
SET(CLUSTERJ_PROJECTS api core jdbc jpatest openjpa test tie unit)

IF(WIN32)

  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pom.xml.in
                 ${CMAKE_CURRENT_BINARY_DIR}/pom.xml
                 @ONLY NEWLINE_STYLE WIN32)

  FOREACH(CLUSTERJ_PROJECT ${CLUSTERJ_PROJECTS}) 
    # build all the pom.xml for windows
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/clusterj-${CLUSTERJ_PROJECT}/pom.xml.in
                   ${CMAKE_CURRENT_BINARY_DIR}/clusterj-${CLUSTERJ_PROJECT}/pom.xml
                   @ONLY NEWLINE_STYLE WIN32)
    # copy logging.properties to all clusterj subprojects
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/logging.properties 
                   ${CMAKE_CURRENT_BINARY_DIR}/clusterj-${CLUSTERJ_PROJECT}/
                   COPYONLY)
  ENDFOREACH(CLUSTERJ_PROJECT in CLUSTERJ_PROJECTS)

  # build the mvn_install_ndbjtie for windows
  #CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/mvn_install_ndbjtie.cmd.in
  #               ${CMAKE_CURRENT_SOURCE_DIR}/mvn_install_ndbjtie.cmd
  #               @ONLY NEWLINE_STYLE WIN32)

ELSE(WIN32)

  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pom.xml.in
                 ${CMAKE_CURRENT_BINARY_DIR}/pom.xml
                 @ONLY NEWLINE_STYLE UNIX)

  FOREACH(CLUSTERJ_PROJECT ${CLUSTERJ_PROJECTS}) 
    # build the pom.xml for *nix
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/clusterj-${CLUSTERJ_PROJECT}/pom.xml.in
                   ${CMAKE_CURRENT_BINARY_DIR}/clusterj-${CLUSTERJ_PROJECT}/pom.xml
                   @ONLY NEWLINE_STYLE UNIX)
    # copy logging.properties to all clusterj subprojects
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/logging.properties 
                   ${CMAKE_CURRENT_BINARY_DIR}/clusterj-${CLUSTERJ_PROJECT}
                   COPYONLY)
  ENDFOREACH(CLUSTERJ_PROJECT in CLUSTERJ_PROJECTS)

  # build the mvn_install_ndbjtie for *nix
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/mvn_install_ndbjtie.sh.in
                 ${CMAKE_CURRENT_BINARY_DIR}/mvn_install_ndbjtie.sh
                 @ONLY NEWLINE_STYLE UNIX)

  ADD_CUSTOM_TARGET(mvn_install_ndbjtie ALL
    chmod a+x "${CMAKE_CURRENT_BINARY_DIR}/mvn_install_ndbjtie.sh"
    || echo "") # don't break the build if file was deleted)

ENDIF(WIN32)

