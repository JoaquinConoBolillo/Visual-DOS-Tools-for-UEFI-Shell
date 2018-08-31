/** @file
  SMM Periodic Timer Dispatch Protocol as defined in PI 1.1 Specification
  Volume 4 System Management Mode Core Interface.

  This protocol provides the parent dispatch service for the periodical timer SMI source generator.

  Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  @par Revision Reference:
  This protocol is from PI Version 1.1.

**/

#ifndef _SMM_PERIODIC_TIMER_DISPATCH2_H_
#define _SMM_PERIODIC_TIMER_DISPATCH2_H_

#include <Pi/PiSmmCis.h>
#include <Protocol/MmPeriodicTimerDispatch.h>

#define EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL_GUID EFI_MM_PERIODIC_TIMER_DISPATCH_PROTOCOL_GUID

///
/// Example: A chipset supports periodic SMIs on every 64ms or 2 seconds.
///   A child wishes schedule a period SMI to fire on a period of 3 seconds, there
///   are several ways to approach the problem:
///   1. The child may accept a 4 second periodic rate, in which case it registers with
///        Period = 40000
///        SmiTickInterval = 20000
///      The resulting SMI will occur every 2 seconds with the child called back on
///      every 2nd SMI.
///      NOTE: the same result would occur if the child set SmiTickInterval = 0.
///   2. The child may choose the finer granularity SMI (64ms):
///        Period = 30000
///        SmiTickInterval = 640
///      The resulting SMI will occur every 64ms with the child called back on
///      every 47th SMI.
///      NOTE: the child driver should be aware that this will result in more
///        SMIs occuring during system runtime which can negatively impact system
///        performance.
///
typedef struct {
  ///
  /// The minimum period of time in 100 nanosecond units that the child gets called. The 
  /// child will be called back after a time greater than the time Period.
  ///
  UINT64  Period;
  ///
  /// The period of time interval between SMIs. Children of this interface should use this 
  /// field when registering for periodic timer intervals when a finer granularity periodic 
  /// SMI is desired.
  ///
  UINT64  SmiTickInterval;
} EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT;

///
/// The DispatchFunction will be called with Context set to the same value as was passed into 
/// Register() in RegisterContext and with CommBuffer pointing to an instance of 
/// EFI_SMM_PERIODIC_TIMER_CONTEXT and CommBufferSize pointing to its size.
///
typedef EFI_MM_PERIODIC_TIMER_CONTEXT EFI_SMM_PERIODIC_TIMER_CONTEXT;

typedef struct _EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL  EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL;

/**
  Register a child SMI source dispatch function for SMM periodic timer.

  This service registers a function (DispatchFunction) which will be called when at least the 
  amount of time specified by RegisterContext has elapsed. On return, DispatchHandle 
  contains a unique handle which may be used later to unregister the function using UnRegister().
  The DispatchFunction will be called with Context set to the same value as was passed into 
  this function in RegisterContext and with CommBuffer pointing to an instance of 
  EFI_SMM_PERIODIC_TIMER_CONTEXT and CommBufferSize pointing to its size.

  @param[in]  This               Pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL instance.
  @param[in]  DispatchFunction   Function to register for handler when at least the specified amount
                                 of time has elapsed. 
  @param[in]  RegisterContext    Pointer to the dispatch function's context.
                                 The caller fills this context in before calling
                                 the register function to indicate to the register
                                 function the period at which the dispatch function
                                 should be invoked.
  @param[out] DispatchHandle     Handle generated by the dispatcher to track the function instance. 

  @retval EFI_SUCCESS            The dispatch function has been successfully
                                 registered and the SMI source has been enabled.
  @retval EFI_DEVICE_ERROR       The driver was unable to enable the SMI source.
  @retval EFI_INVALID_PARAMETER  RegisterContext is invalid. The period input value
                                 is not within valid range.
  @retval EFI_OUT_OF_RESOURCES   There is not enough memory (system or SMM) to manage this child.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_PERIODIC_TIMER_REGISTER2)(
  IN CONST EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL  *This,
  IN       EFI_SMM_HANDLER_ENTRY_POINT2               DispatchFunction,
  IN CONST EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT    *RegisterContext,
  OUT      EFI_HANDLE                                 *DispatchHandle
  );

/**
  Unregisters a periodic timer service.

  This service removes the handler associated with DispatchHandle so that it will no longer be 
  called when the time has elapsed.

  @param[in] This                Pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL instance.
  @param[in] DispatchHandle      Handle of the service to remove.

  @retval EFI_SUCCESS            The service has been successfully removed.
  @retval EFI_INVALID_PARAMETER  The DispatchHandle was not valid.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_PERIODIC_TIMER_UNREGISTER2)(
  IN CONST EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL  *This,
  IN       EFI_HANDLE                                 DispatchHandle
  );

/**
  Returns the next SMI tick period supported by the chipset.

  The order returned is from longest to shortest interval period.

  @param[in]     This             Pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL instance.
  @param[in,out] SmiTickInterval  Pointer to pointer of next shorter SMI interval
                                  period supported by the child. This parameter works as a get-first,
                                  get-next field.The first time this function is called, *SmiTickInterval
                                  should be set to NULL to get the longest SMI interval.The returned
                                  *SmiTickInterval should be passed in on subsequent calls to get the
                                  next shorter interval period until *SmiTickInterval = NULL.

  @retval EFI_SUCCESS             The service returned successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_PERIODIC_TIMER_INTERVAL2)(
  IN CONST EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL  *This,
  IN OUT UINT64                                       **SmiTickInterval
  );

///
/// Interface structure for the SMM Periodic Timer Dispatch Protocol
///
/// This protocol provides the parent dispatch service for the periodical timer SMI source generator.
///
struct _EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL {
  EFI_SMM_PERIODIC_TIMER_REGISTER2    Register;
  EFI_SMM_PERIODIC_TIMER_UNREGISTER2  UnRegister;
  EFI_SMM_PERIODIC_TIMER_INTERVAL2    GetNextShorterInterval;
};

extern EFI_GUID gEfiSmmPeriodicTimerDispatch2ProtocolGuid;

#endif

