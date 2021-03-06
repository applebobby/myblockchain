/* Copyright (c) 2012, 2014, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef MYBLOCKCHAIN_TRACE_INCLUDED
#define MYBLOCKCHAIN_TRACE_INCLUDED
/**
  @file

  =====================================================
   Declarations for client-side tracing infrastructure
  =====================================================

  See libmyblockchain/myblockchain_trace.c for a brief description. Trace plugin
  declarations are in plugin_trace.h header.
*/


C_MODE_START

/*
  Disable trace hooks if the infrastructure is not enabled or if
  libmyblockchain code is used from within the (embedded) server.
*/
#if !defined(CLIENT_PROTOCOL_TRACING) \
    || defined(MYBLOCKCHAIN_SERVER) \
    || defined(EMBEDDED_LIBRARY)

#define MYBLOCKCHAIN_TRACE(E, M, ARGS)
#define MYBLOCKCHAIN_TRACE_STAGE(M, S)

#else

#include <myblockchain/plugin_trace.h>
#include <sql_common.h>                         /* for MYBLOCKCHAIN_EXTENSION() */
#include <stdarg.h>


/**
  Per connection protocol tracing state

  For each connection which is traced an instance of this structure
  is pointed by the trace_data member of st_myblockchain_extension structure
  attached to that connection handle.

  If trace_data is NULL, for an initialized connection, then it means
  that tracing in this connection is disabled.
*/

struct st_myblockchain_trace_info
{
  struct st_myblockchain_client_plugin_TRACE *plugin;
  void *trace_plugin_data;
  enum protocol_stage stage;
};

#define TRACE_DATA(M)   (MYBLOCKCHAIN_EXTENSION_PTR(M)->trace_data)


/*
  See libmyblockchain/myblockchain_trace.c for documentation and implementation of
  these functions.
*/

void myblockchain_trace_trace(struct st_myblockchain*,
                       enum trace_event,
                       struct st_trace_event_args);
void myblockchain_trace_start(struct st_myblockchain*);


/**
  The main protocol tracing hook.

  It is placed in places in the libmyblockchain code where trace events occur.
  If tracing of the connection is not disabled, it calls
  myblockchain_trace_trace() function to report the event to the
  trace plugin.

  @param E    trace event (without TRACE_EVENT_ prefix)
  @param M    connection handle (pointer to MYBLOCKCHAIN structure)
  @param ARGS event specific arguments

  Event arguments are processed with appropriate TRACE_ARGS_* macro
  (see below) to put them inside st_trace_event_args structure.
*/

#define MYBLOCKCHAIN_TRACE(E, M, ARGS)                  \
  do {                                           \
  if (NULL == TRACE_DATA(M)) break;              \
  {                                              \
    struct st_trace_event_args event_args=       \
                          TRACE_ARGS_ ## E ARGS; \
    myblockchain_trace_trace(M,                         \
                      TRACE_EVENT_ ## E,         \
                      event_args);               \
  }                                              \
  } while(0)


/**
  A hook to set the current protocol stage.

  @param M  connection handle (pointer to MYBLOCKCHAIN structure)
  @param S  the current stage (without PROTOCOL_STAGE_ prefix)

  If tracing is not disabled, the stage is stored in connection's
  tracing state. A special case is if the current stage is the
  initial CONNECTING one. In that case function myblockchain_trace_start()
  is called to initialize tracing in this connection, provided that
  a trace plugin is loaded.
*/

#define MYBLOCKCHAIN_TRACE_STAGE(M, S)                                   \
  do {                                                            \
    if (TRACE_DATA(M))                                            \
      TRACE_DATA(M)->stage= PROTOCOL_STAGE_ ## S;                 \
    else if (trace_plugin &&                                      \
             (PROTOCOL_STAGE_CONNECTING == PROTOCOL_STAGE_ ## S)) \
      myblockchain_trace_start(M);                                       \
  } while(0)

/*
  Macros to parse event arguments and initialize the
  st_trace_event_args structure accordingly. See description of
  the structure in plugin_trace.h.
*/

#define TRACE_ARGS_SEND_SSL_REQUEST(Size, Packet)   { NULL, 0, NULL, 0, Packet, Size }
#define TRACE_ARGS_SEND_AUTH_RESPONSE(Size, Packet) { NULL, 0, NULL, 0, Packet, Size }
#define TRACE_ARGS_SEND_AUTH_DATA(Size, Packet)     { NULL, 0, NULL, 0, Packet, Size }
#define TRACE_ARGS_AUTH_PLUGIN(PluginName)          { PluginName, 0, NULL, 0, NULL, 0 }
#define TRACE_ARGS_SEND_COMMAND(Command, HdrSize, ArgSize, Header, Args) \
                                                    { NULL, Command, Header, HdrSize, Args, ArgSize }
#define TRACE_ARGS_SEND_FILE(Size, Packet)          { NULL, 0, NULL, 0, Packet, Size }

#define TRACE_ARGS_PACKET_SENT(Size)                { NULL, 0, NULL, 0, NULL, Size }
#define TRACE_ARGS_PACKET_RECEIVED(Size, Packet)    { NULL, 0, NULL, 0, Packet, Size }
#define TRACE_ARGS_INIT_PACKET_RECEIVED(Size, Packet) { NULL, 0, NULL, 0, Packet, Size }

#define TRACE_ARGS_ERROR()                          { NULL, 0, NULL, 0, NULL, 0 }
#define TRACE_ARGS_READ_PACKET()                    { NULL, 0, NULL, 0, NULL, 0 }
#define TRACE_ARGS_CONNECTING()                     { NULL, 0, NULL, 0, NULL, 0 }
#define TRACE_ARGS_CONNECTED()                      { NULL, 0, NULL, 0, NULL, 0 }
#define TRACE_ARGS_DISCONNECTED()                   { NULL, 0, NULL, 0, NULL, 0 }
#define TRACE_ARGS_AUTHENTICATED()                  { NULL, 0, NULL, 0, NULL, 0 }
#define TRACE_ARGS_SSL_CONNECT()                    { NULL, 0, NULL, 0, NULL, 0 }
#define TRACE_ARGS_SSL_CONNECTED()                  { NULL, 0, NULL, 0, NULL, 0 }


#endif /* !defined(CLIENT_PROTOCOL_TRACING) || defined(MYBLOCKCHAIN_SERVER) */

C_MODE_END

#endif
