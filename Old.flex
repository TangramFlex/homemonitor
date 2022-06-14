module <ReplaceWithOldAlarmName>.Old

enum Motion int32 {
  Active = 0;
  Inactive = 1;
  SensorError = 2;
}

enum Entry int32 {
  Open = 0;
  Closed = 1;
  SensorError = 2;
}

message struct OldSystemState {
  livingRoomMotion1: Motion;
  livingRoomMotion2: Motion;
  basementMotion1: int32;
  frontDoorEntry: string;
  garageDoorEntry: Entry;
  patioTemp: int32;
}