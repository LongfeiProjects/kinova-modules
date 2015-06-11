/*
 * VrepDefinition.h
 *
 *  Created on: Jun 11, 2015
 *      Author: vale
 */

#ifndef VREPDEFINITION_H_
#define VREPDEFINITION_H_

//Regular operation modes
int simx_opmode_oneshot				=0;
int simx_opmode_oneshot_wait		=65536;
int	simx_opmode_continuous			=131072;
int simx_opmode_streaming			=131072;

// Operation modes for heavy data
int simx_opmode_oneshot_split		=196608;
int simx_opmode_continuous_split	=262144;
int simx_opmode_streaming_split		=262144;

// Special operation modes
int simx_opmode_discontinue			=327680;
int simx_opmode_buffer				=393216;
int simx_opmode_remove				=458752;

#endif /* VREPDEFINITION_H_ */
