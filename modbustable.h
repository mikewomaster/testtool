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
#define FormatFlash (16 - 1)

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

// nb parameters
#define APNAddress (4001 - 1)
#define APNEntries 16

#define UserAddress (4017 - 1)
#define UserEntries 8

#define NBPwdAddress (4025 - 1)
#define NBPwdEntries 8

#define NBIoTModelAddress (4033 - 1)
#define NBIoTEntries 1

#define NBPLMNAddress (4034 - 1)
#define NBPLMNEntries 3

#define NBStatusAddress (4105 - 1)
#define NBStatusEntries 1

#define NBIPAddress (5002 - 1)
#define NBIPEntries 8

#define NBEnableAddress (821 - 1)
#define NBEnableEntries 1

#define NBConnTestAddress (4106 - 1)
#define NBConnTestEntries 1

#define EightEntries 8
#define OneEntry 1

#define NBRegistrationAddress (4042 -1)
#define NBProviderAddress (4050 - 1)
#define NBIMEIAddress (4058 -1)
#define NBIMSIAddress (4064 - 1)
#define NBPLMNAddress2 (4072 -1)
#define NBBANDAddress (4080 - 1)
#define NBRSSIAddress (4088 - 1)
#define NBSIMStatus (4096 - 1)

// mqtt parameters
#define mqttIPAddress (4501 - 1)
#define mqttIPEntries 16

#define mqttPortAddress (4517 - 1)
#define mqttPortEntries 1

#define mqttTopicAddress (4518 - 1)
#define mqttTopicEntreis 32

#define mqttClinetIDAddress (4550 - 1)
#define mqttClinetIDEntries 16

#define mqttIntervalAddress (4566 - 1)
#define mqttIntervalEntries 2

#define mqttUserAddress (4568 - 1)
#define mqttUserEntries 8

#define mqttPassWordAddress (4576 - 1)
#define mqttPassWordEntries 8

#define mqttRtuTimeOutAddress (4584 - 1)
#define mqttRTUTimeOutEntries 1

#define mqttStatusAddress (5501 - 1)
#define mqttStatusEntries 1

#define TCPIPAddress (661 - 1)
#define TCPIPEntries 16

#define TCPPortAddress (677 - 1)
#define TCPPortEntries 1

#define MQTTSubTopicAddress (4584 - 1)
#define MQTTSubTopicEntries 32

#define MQTTServerConnectAddress (4616 - 1)
#define MQTTServerConnectEntries 1

// led parameters
#define EnergyOneEntry 1
#define Entry2 2
#define Entry16 16

#define EnergyVoltageAddress (601 - 1)
#define EnergyAmpAddress (602 - 1)
#define EnergyPowerAddress (603 - 1)
#define EnergyVRMSAddress (604 - 1)
#define EnergyIRMSAddress (605 - 1)
#define EnergyPFAddress (606 -1)
#define ACFrequencyAddress (307 - 1)

#define controlTestOnAddress (803 - 1)
#define controlTestOuputAddress (804 - 1)

//M-Bus
#define systemModeAddress (14 - 1)
#define mBusVoltageAddress (12 - 1)

#define cellularAPNAddress (4001 - 1)
#define cellularUserName (4017 - 1)
#define cellularPassWord (4025 - 1)
#define cellularPLMN (4034 - 1)

#define meterGap 200

#define meterModelBaseAddress (301 - 1)
#define addressModeBaseAddress (309 - 1)
#define meterAddressBaseAddress (310 - 1)

#define meterTagBaseAddress (318 - 1)
#define meterIdBaseAddress (398 - 1)
#define meterMagnitudeBaseAddress (418 - 1)

#define meterStatus (11 - 1)

#define meterPollStart (5000 - 1)
#define meterPollSN (5001 - 1)
#define meterPollAdressMode (5009 - 1)
#define meterPollPriAddress (5010 - 1)
#define meterPollSecAddress (5011 - 1)
#define meterPollStatus (5019 - 1)
#define meterPollManu (5020 - 1)
#define meterPollType (5028 - 1)
#define meterPollVersion (5029 - 1)
#define meterPollBaudRate (5030 - 1)
#define meterPollAlarmCode (5031 - 1)
#define meterPollChannel (5032 - 1)

#define meterPollGap (271)

#define mbusStart (1 - 1)

#endif // MODBUSTABLE_H
