#include "main.h"
#include "v5dbg/debug.h"

// Global debug server state
v5dbg_server_state_t sState{};

void
autonomous(void)
{
  $ntask $function
}

void
initialize(void)
{
  // Start debug server

  sState = V5Dbg_AllocateServerState();
  V5Dbg_StartServer(&sState);
}

void
disabled(void)
{
}

void
competition_initialize(void)
{
}

void
opcontrol(void)
{
  $ntask;
  $function;

  double x = 0; $expose(x);
  std::string string = "Hello World!"; $expose(string);
  float value = 52.5f; $expose(value);

  while (true)
  {
    $cbreak(x > 20);

    printf("%f %s %f\n", x, string.c_str(), value);

    x++;
    pros::delay(300);
  }
}
