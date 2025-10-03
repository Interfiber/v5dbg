#include "main.h"
#include "v5dbg/stack.h"
#include "v5dbg/server.h"

void on_center_button() {
}

v5dbg_server_state_t serverState{};

void initialize() {
  serverState = V5Dbg_AllocateServerState();

  V5Dbg_StartServer(&serverState);
}

void disabled() {}

void competition_initialize() {}

void autonomous() {}

void printData(const std::string &d)
{
  $function

  printf("%s\n", d.c_str());
}

void run()
{
  $function

  pros::delay(1000);
  printData("Hello World");
}

void opcontrol() {
  V5Dbg_Init();
  $function

  while (true) {
    run();

    pros::delay(20);
  }
}