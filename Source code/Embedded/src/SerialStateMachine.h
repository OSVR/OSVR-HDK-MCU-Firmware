/*
 * SerialStateMachine.h
 *
 * Created: 9/1/2013 6:57:04 PM
 *  Author: Sensics
 */


#ifndef SERIALSTATEMACHINE_H_
#define SERIALSTATEMACHINE_H_

extern bool CommandReady; // true if a command is ready to be executed

void InitSerialState(void);
void ProcessIncomingChar(char CharReceived);
void ProcessCommand(void);



#endif /* SERIALSTATEMACHINE_H_ */