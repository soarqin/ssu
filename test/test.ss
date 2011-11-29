option optimize_for = LITE_RUNTIME;
option cc_generic_services = false;

package game.net.data.StoC;

// 0x01
// 玩家注册
message SCUserRegister_0x01 {
  message Test
  {
    required uint32 abc = 1;
  }
  // 注册结果
  // 0 - 成功  1 - 名字重复  2 - 名字含有非法字符
  required uint32 result = 1;
  // 登录后用户ID
  optional uint32 playerId = 2;
  repeated Test tests = 3;
  optional Test test2 = 4;
}
