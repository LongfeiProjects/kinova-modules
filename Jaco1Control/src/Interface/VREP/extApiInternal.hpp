// This file is part of the REMOTE API
// 
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// The REMOTE API is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The REMOTE API is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE REMOTE API IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the REMOTE API.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.1.3 on Sept. 30th 2014

#ifndef __EXTAPIINTERNAL_
#define __EXTAPIINTERNAL_

#include "extApiPlatform.hpp"

#define SIMX_INIT_BUFF_SIZE 500
#define SIMX_MIN_BUFF_INCR 500

#define _REPLY_WAIT_TIMEOUT_IN_MS 5000
#define _MIN_SPLIT_AMOUNT_IN_BYTES 100
#define MAX_EXT_API_CONNECTIONS 255

/* Out buffer for messages */
extern simxUChar* _messageToSend[];
extern simxInt _messageToSend_bufferSize[];
extern simxInt _messageToSend_dataSize[];

/* Temp buffer for split commands to send */
extern simxUChar* _splitCommandsToSend[];
extern simxInt _splitCommandsToSend_bufferSize[];
extern simxInt _splitCommandsToSend_dataSize[];

/* In buffer for messages */
extern simxUChar* _messageReceived[];
extern simxInt _messageReceived_bufferSize[];
extern simxInt _messageReceived_dataSize[];

/* Temp buffer for split commands received */
extern simxUChar* _splitCommandsReceived[];
extern simxInt _splitCommandsReceived_bufferSize[];
extern simxInt _splitCommandsReceived_dataSize[];

/* Temp buffer for last fetched command */
extern simxUChar* _commandReceived[];
extern simxInt _commandReceived_bufferSize[];
extern simxInt _commandReceived_simulationTime[];

/* Other variables */
extern simxInt * _nextConnectionID; // change definition from simxInt  _nextConnectionID[] to simxInt * _nextConnectionID
extern simxInt _replyWaitTimeoutInMs[];

extern simxInt _connectionPort[];
extern simxChar* _connectionIP[];

extern simxInt _minCommunicationDelay[];
extern simxUChar _communicationThreadRunning[];
extern simxInt _nextMessageIDToSend[];
extern simxInt _waitBeforeSendingAgainWhenMessageIDArrived[];
extern simxInt _lastReceivedMessageID[];
extern simxInt _connectionID[];
extern const simxChar* _tempConnectionAddress[MAX_EXT_API_CONNECTIONS];
extern simxInt _tempConnectionPort[];
extern simxUChar _tempDoNotReconnectOnceDisconnected[];

#endif /* __EXTAPIINTERNAL_ */
