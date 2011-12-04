option optimize_for = LITE_RUNTIME;
option cc_generic_services = false;

message StructTest {
  enum enumA
  {
    eA = 1;
    eB = 2;
    eC = 3;
  }
  message StructA
  {
    required uint32 abc = 1;
  }
  required uint32 sa = 1;
  optional int32 sb = 2;
  repeated StructA sc = 3;
  optional StructA sd = 4;
  repeated sint32 se = 5;
  required enumA sf = 6;
  repeated enumA sg = 7;
}
