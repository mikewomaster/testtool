#ifndef MODBUSTABLE_H
#define MODBUSTABLE_H

#define UnitNumber 1
#define MaxRefVal 1242
#define MinRefVal 1202
#define RaiseFlag 1
#define FallFlag 0
#define Step 2

#define ModelNameAddr (134 - 1)
#define ModelNameEntires 8

#define SNAddr (142 - 1)
#define SNEntries 8

#define VersionAddress (171 - 1)
#define VersionEntries 8

#define ResetEnableAddress (179 - 1)

#define LoraModbusStartAddr 21
#define LoraModbusEntries 7

#define netBitMapAddr (102 - 1)
#define netBitMapEntires 16

#define vinRef (401 - 1)
#define vin2Ref (411 - 1)
#define voutRef (402 - 1)
#define iinRef (403 - 1)
#define iin2Ref (412 - 1)
#define ioutRef (404 - 1)
#define adjust1 (405 - 1)
#define adjust2 (406 - 1)
#define adjust3 (407 - 1)
#define adjust4 (408 - 1)
#define adjust5 (409 - 1)
#define adjust6 (410 - 1)

#define NeedResetFirst 0

#define currentInputAddr (1001 - 1)
#define voltageInputAddr (1017 - 1)
#define PWMInputAddr (1033 - 1)
#define thermocoupleAddr (1049 - 1)
#define currentOutputAddr (1065 - 1)
#define voltageOutputAddr (1081 - 1)
#define PWMOutputAddr (1097 - 1)
#define IOEntries 1
#define PWMIOEntries 2

#define voltageInputChannelOne 0
#define currentInputChannelOne 1
#define voltageInputChannelTwo 2
#define currentInputChannelTwo 3
#define currentOutputChannel 4
#define voltageOutputChannel 5
#define PWMOne 6
#define PWMTwo 7

#endif // MODBUSTABLE_H
