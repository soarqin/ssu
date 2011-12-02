option optimize_for = LITE_RUNTIME;
option cc_generic_services = false;

package game.net.data.StoC;

// 0x01
// 玩家注册
message SCUserRegister_0x01 {
  message StructA
  {
    required uint32 abc = 1;
  }
  // 注册结果
  // 0 - 成功  1 - 名字重复  2 - 名字含有非法字符
  required uint32 sa = 1;
  // 登录后用户ID
  optional int32 sb = 2;
  repeated StructA sc = 3;
  optional StructA sd = 4;
  repeated sint32 se = 5;
}
